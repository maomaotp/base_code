// cpp_dev_test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string>
#include "biloba_core_lib.h"
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <direct.h>
#include "zmq.h"
#include "zmq_utils.h" 

extern "C" {
#include "cjson.c"
}

#define VOISE_PWD	"G:\\liuq\\tts\\nginx-1.7.9\\html\\"
//#define VOISE_PWD	"E:\\tmp\\"
#define SERVER_URL	"http://202.142.26.210:8080"
#define BROKER_IP   "tcp://123.57.41.242:5560"

#define FORCE_UTF8 1

#define RETURN_SUCCESS 0
#define RETURN_ERROR -1

using namespace std;
using namespace BilobaTTS;

bool WriteWave(const char *fname, int n_samples, short *data);

typedef struct _SpeakerInfo{
	string name;
	string language;
	string gender;
}SpeakerInfo;

void *resp;

static string fixed_bgmusic[11] =
{
	"",
	"bgm0",
	"bgm1",
	"bgm2",
	"bgm3",
	"bgm4",
	"bgm5",
	"bgm6",
	"bgm7",
	"bgm8",
	"bgm9",

};
static string *bgmusic_list = fixed_bgmusic;
static int n_bgmusic = 11;

static SpeakerInfo fixed_speakers[8] =
{
	{ "LHD", "CHS", "M" },
	{ "WH", "CHS", "F" },
	{ "QJCS", "CHS", "F" },
	{ "YUJ", "CHS", "F" },
	{ "Cath1", "ENG", "F" },
	{ "Roger", "ENG", "M" },
	{ "Nancy", "ENG", "F" },
	{ "John", "ENG", "M" },

};
static SpeakerInfo *speaker_list = fixed_speakers;
static int n_speakers = 8;

std::string UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β��
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}

void set_mp3_file_name(char* filename, size_t fn_size) {
	time_t t = time(0);
	char file[20];
	char file_pass[100];
	char file_time[20];

	strftime(file, sizeof(file), "%m%d", localtime(&t));
	strftime(file_time, sizeof(file_time), "%m%d%S", localtime(&t));

	sprintf(filename, "%s/%s", file, file_time);
	sprintf(file_pass, "%s\\%s", VOISE_PWD, file);
	if (_mkdir(file_pass) == 0){
		printf("create file error\n");
	}
}

string SelectBGMusic(int i)
{
	return (i < 0 || i >= n_bgmusic) ? "" : bgmusic_list[i];
}

SpeakerInfo *SelectSpeaker(int i)
{
	if (i<0 || i>n_speakers)
		return NULL;
	else
		return &speaker_list[i];
}

static void stop_tts_service() {
	BilobaStopLog();
	CoreBilobaQuit();
}

static char *s_recv(void *socket) {
	char buffer[2048];
	int size = zmq_recv(socket, buffer, 2047, 0);
	buffer[size] = 0;
	return strdup(buffer);
}

static void Wav2MP3(string wavfn, string mp3fn) {
	char cmdln[1024];
	sprintf_s(cmdln, "lame.exe --quiet -V2 %s %s", wavfn.c_str(), mp3fn.c_str());
	system(cmdln);
}

static void s_return_err(void *resp, int err_code){
	char buf[50];
	sprintf(buf, "{\"code\":%d}", err_code);
	printf("error:%d\n", err_code);
	zmq_send(resp, buf, strlen(buf) + 1, 0);
}

static void init_tts_service() {
	BilobaSetupLog(NULL, "test", 128, 0, LOG_MODE_BOTH);  //��ʼ����־
	CoreBilobaInit(NULL, NULL, "config\\voice\\bgm.cfg", "config\\voice\\Reverb.cfg");//��ʼ������

	return;
}

static int init_zmq_server() {
	void *context = zmq_ctx_new();
	resp = zmq_socket(context, ZMQ_REP);

	if (zmq_connect(resp, BROKER_IP) != 0){
		printf("zmq connect to server failed\n");
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}

static int send_url_voice(int n_segs, char *file_name, char *res_json_t) {
	cJSON * pJsonRoot = NULL;
	pJsonRoot = cJSON_CreateObject();
	if (pJsonRoot == NULL) {
		s_return_err(resp, 1006);
		return RETURN_ERROR;
	}

	cJSON_AddNumberToObject(pJsonRoot, "segs", n_segs);
	cJSON_AddStringToObject(pJsonRoot, "url", SERVER_URL);
	cJSON_AddStringToObject(pJsonRoot, "filename", file_name);
	cJSON_AddStringToObject(pJsonRoot, "format", "mp3");

	sprintf(res_json_t, "[%s]", cJSON_Print(pJsonRoot));
	cJSON_Delete(pJsonRoot);

	return RETURN_SUCCESS;
}

static int get_speak_name(char *language) {
	return (strncmp(language, "CHS", 3) == 0) ? 1 : 4;
}

static int set_speach_parameter(char *recv_buf, CoreTTSTask* task) {
	cJSON *root;

	root = cJSON_Parse(recv_buf);
	if (!root) {
		s_return_err(resp, 1001);
		return RETURN_ERROR;
	}
	cJSON *format = cJSON_GetObjectItem(root, "task");
	if (!format){
		s_return_err(resp, 1002);
		return RETURN_ERROR;
	}
	cJSON *text = cJSON_GetObjectItem(root, "text");

	cJSON *language = cJSON_GetObjectItem(format, "language");
	//cJSON *name = cJSON_GetObjectItem(format, "Name");
	cJSON *volume = cJSON_GetObjectItem(format, "Volume");
	cJSON *speed = cJSON_GetObjectItem(format, "Speed");
	cJSON *pitch = cJSON_GetObjectItem(format, "Pitch");
	cJSON *background = cJSON_GetObjectItem(format, "background_music");
	if (text == NULL || language == NULL) {
		return RETURN_ERROR;
	}

	//��ȡ������name
	int index = get_speak_name(language->valuestring);
	//��������
	int back_gd = (background) ? background->valueint : 3;

	string bgmusic = SelectBGMusic(back_gd);

	SpeakerInfo *speaker_info = SelectSpeaker(index);
	if (!speaker_info){
		s_return_err(resp, 1004);
		return RETURN_ERROR;
	}

	string gb_txt;
	gb_txt = FORCE_UTF8 ? UTF8_To_string(text->valuestring) : text->valuestring;

	task->SetGender( speaker_info->gender );
	task->SetLanguage( language ? language->valuestring : "ENG" );
	task->SetName( speaker_info->name );
	task->SetVolume( volume ? volume->valueint: 3);
	task->SetSpeed( speed ? speed->valueint : 3);
	task->SetPitch( pitch ? pitch->valueint : 3);
	task->SetBGMusic(bgmusic, -1);
	//���úϳ��ı�
	task->SetText(gb_txt);

	printf("SetText:\n\n\n%s\n\n\n", gb_txt.c_str());

	//printf("speaker_info->gender == %s\nindex=%d\nspeaker_info->name=%s\n\n", speaker_info->gender, index, speaker_info->name);

	cJSON_Delete(root);
	return RETURN_SUCCESS;
}

void parse_zmq_task(){
	char wav_path[100];
	char mp3_path[100];

	char *res_json_t = (char *)malloc(2048);
	if (!res_json_t) {
		printf("malloc error");
		return;
	}

	char *recv_buf = (char *)malloc(2048);
	if (!recv_buf) {
		printf("malloc error");
		return;
	}

	while (true) {
		printf("\n\n\n..........wait client ............\n\n");
		memset(recv_buf, 0, sizeof(recv_buf));
		//��������
		recv_buf = s_recv(resp);

		//�����ļ���
		char file_name[50];

		set_mp3_file_name(file_name, sizeof(file_name));

		CoreTTSTask *task = new CoreTTSTask(); //�����ϳ�����
		if (!task){
			s_return_err(resp, 1005);
			continue;
		}

		//���úϳɲ���
		if (set_speach_parameter(recv_buf, task) != 0) {
			continue;
		}

		task->SequentialTTS_Prepare(); //׼�����л��ϳ�

		BilobaResult ret_code = RET_SUCCEED;
		int n_segs = task->GetNumberSegmentCount(ret_code); //ȡ�����л��ϳɵ�����Ƭ����

		//����URL,����json��ʽ����
		if (send_url_voice(n_segs, file_name, res_json_t) != 0) {
			continue;
		}

		task->SequentialTTS_Start();  //�������кϳ�

		if (n_segs > 0 && ret_code == RET_SUCCEED){
			for (int i = 0; i < n_segs; i++){

				//ѭ����ȡ�ϳɵ�����Ƭ��
				WavRecord *wav = task->SequentialTTS_GetSegment(i, 5, ret_code);
				if (!wav) continue;
				if (ret_code == RET_SUCCEED){
					//���ϳɵ�ÿ��Ƭ������MP3�ļ�
					sprintf(wav_path, "%s\\%s_%d.wav", VOISE_PWD, file_name, i + 1);
					sprintf(mp3_path, "%s\\%s_%d.mp3", VOISE_PWD, file_name, i + 1);

					WriteWave(wav_path, wav->n_samples, (short *)wav->wav_buff);
					//wavתmp3
					Wav2MP3(wav_path, mp3_path);
					//ɾ��wav�ļ�
					remove(wav_path);
					//���ɵ�һ���ļ�Ƭ�κ�����Ӧ��Ϣ
					if (i == 1) {
						int send_size = zmq_send(resp, res_json_t, strlen(res_json_t) + 1, 0);
					}
				}
			}
		}

		task->SequentialTTS_Exit();
		delete task;
	}

	free(recv_buf);
	free(res_json_t);
}

int main(int argc, _TCHAR* argv[])
{
	//��ʼ��tts����
	init_tts_service();

	//��ʼ��zmq�����ӵ�broker
	if (init_zmq_server() < 0) {
		return RETURN_ERROR;
	}

	//��������
	parse_zmq_task();

	//�ر�tts����
	stop_tts_service();

	return RETURN_SUCCESS;
}