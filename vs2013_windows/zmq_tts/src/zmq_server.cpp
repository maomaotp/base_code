// cpp_dev_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "biloba_core_lib.h"
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <direct.h>
#include "zmq.h"
#include "zmq_utils.h" 
#include <process.h>

extern "C" {
#include "cjson.c"
}

#define VOISE_PWD	"G:\\liuq\\tts\\nginx-1.7.9\\html\\"
//#define VOISE_PWD	"E:\\tmp\\"
#define SERVER_URL	"http://202.142.26.210:8080"
//#define BROKER_IP   "tcp://123.57.41.242:5560"
#define BROKER_IP   "tcp://202.142.26.210:5560"

#define FORCE_UTF8 1

#define RETURN_SUCCESS 0
#define RETURN_ERROR -1

using namespace std;
using namespace BilobaTTS;

int WriteWave(const char *fname, int n_samples, short *data);

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

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
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
	strftime(file_time, sizeof(file_time), "%H%M%S", localtime(&t));

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
	int size = zmq_recv(socket, buffer, 2000, 0);
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
	BilobaSetupLog(NULL, "test", 128, 0, LOG_MODE_BOTH);  //初始化日志
	CoreBilobaInit(NULL, NULL, "config\\voice\\bgm.cfg", "config\\voice\\Reverb.cfg");//初始化引擎

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

	//cJSON *language = cJSON_GetObjectItem(format, "language");
	cJSON *speaker = cJSON_GetObjectItem(format, "speaker");
	cJSON *volume = cJSON_GetObjectItem(format, "Volume");
	cJSON *speed = cJSON_GetObjectItem(format, "Speed");
	cJSON *pitch = cJSON_GetObjectItem(format, "Pitch");
	cJSON *background = cJSON_GetObjectItem(format, "background_music");
	if (text == NULL || text->valuestring == NULL) {
		s_return_err(resp, 1003);
		return RETURN_ERROR;
	}

	int index = speaker ? speaker->valueint : 1;
	
	//背景音乐
	int back_gd = (background) ? background->valueint : 3;

	string bgmusic = SelectBGMusic(back_gd);

	SpeakerInfo *speaker_info = SelectSpeaker(index);
	if (!speaker_info){
		s_return_err(resp, 1004);
		return RETURN_ERROR;
	}

	//printf("speaker_info->name == %s  index=%d\n\n\n\n\n", speaker_info->name, index);

	string gb_txt;
	gb_txt = FORCE_UTF8 ? UTF8_To_string(text->valuestring) : text->valuestring;

	task->SetGender( speaker_info->gender );
	task->SetLanguage(speaker_info->language);
	task->SetName( speaker_info->name );
	task->SetVolume( volume ? volume->valueint : 3);
	task->SetSpeed( speed ? speed->valueint : 3);
	task->SetPitch( pitch ? pitch->valueint : 3);
	task->SetBGMusic(bgmusic, -1);
	//设置合成文本
	task->SetText(gb_txt);

	

	printf("SetText:\n\n\n%s\n\n\n", gb_txt.c_str());

	//printf("speaker_info->gender == %s\nindex=%d\nspeaker_info->name=%s\n\n", speaker_info->gender, index, speaker_info->name);

	cJSON_Delete(root);
	return RETURN_SUCCESS;
}

void handle_zmq_task(){
	char wav_path[100];
	char mp3_path[100];

	char *res_json_t = (char *)malloc(2048);
	if (!res_json_t) {
		printf("malloc error");
		return;
	}

	char *recv_buf = (char *)malloc(3000);
	if (!recv_buf) {
		printf("malloc error");
		return;
	}

	while (true) {
		printf("\n\n\n..........wait client ............\n\n");
		memset(recv_buf, 0, sizeof(recv_buf));
		//接收请求
		recv_buf = s_recv(resp);

		//生成文件名
		char file_name[50];

		set_mp3_file_name(file_name, sizeof(file_name));

		CoreTTSTask *task = new CoreTTSTask(); //建立合成任务
		if (!task){
			s_return_err(resp, 1005);
			continue;
		}

		//设置合成参数
		if (set_speach_parameter(recv_buf, task) != 0) {
			continue;
		}

		task->SequentialTTS_Prepare(); //准备序列化合成

		BilobaResult ret_code = RET_SUCCEED;
		int n_segs = task->GetNumberSegmentCount(ret_code); //取得序列化合成的语音片段数

		//发送URL,构建json格式数据
		if (send_url_voice(n_segs, file_name, res_json_t) != 0) {
			s_return_err(resp, 1006);
			continue;
		}

		task->SequentialTTS_Start();  //启动序列合成

		if (n_segs > 0 && ret_code == RET_SUCCEED){
			for (int i = 0; i < n_segs; i++){
				//循环获取合成的语音片段
				WavRecord *wav = task->SequentialTTS_GetSegment(i, 5, ret_code);
				if (!wav) {
					s_return_err(resp, 1007);
					continue;
				}
				if (ret_code == RET_SUCCEED){
					//将合成的每个片段生成MP3文件
					sprintf(wav_path, "%s\\%s_%d.wav", VOISE_PWD, file_name, i + 1);
					sprintf(mp3_path, "%s\\%s_%d.mp3", VOISE_PWD, file_name, i + 1);

					WriteWave(wav_path, wav->n_samples, (short *)wav->wav_buff);

					//wav转mp3
					Wav2MP3(wav_path, mp3_path);
					//删除wav文件
					remove(wav_path);
					//生成第一个文件片段后发送响应消息
					if (i == 0) {
						int send_size = zmq_send(resp, res_json_t, strlen(res_json_t) + 1, 0);
					}
				}
				else{
					s_return_err(resp, 1007);
				}
			}
		}
		else {
			s_return_err(resp, 1008);
		}

	    BilobaResult ret;
		string utt_str = task->GetSynUtterString(ret);
		std::cout << "Front end output:\n" << utt_str << std::endl;

		task->SequentialTTS_Exit();
		delete task;
	}

	free(recv_buf);
	free(res_json_t);
}

unsigned int __stdcall handle_http_task(void *param) {
}


int main(int argc, _TCHAR* argv[])
{
	//初始化tts引擎
	init_tts_service();

	//初始化zmq，连接到broker
	if (init_zmq_server() < 0) {
		return RETURN_ERROR;
	}

	//接收任务
	handle_zmq_task();

	//关闭tts引擎
	stop_tts_service();

	return RETURN_SUCCESS;
}
