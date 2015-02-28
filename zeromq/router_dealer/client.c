//  Hello World client
//  Connects REQ socket to tcp://localhost:5559
//  Sends "Hello" to server, expects "World" back

#include "zhelpers.h"

int main (void)
{
    void *context = zmq_ctx_new ();

    //  Socket to talk to server
    void *requester = zmq_socket (context, ZMQ_REQ);
    //zmq_connect (requester, "tcp://192.168.1.120:5559");
    zmq_connect (requester, "tcp://123.57.41.242:5559");

	//char task[] = "{\"task\":{\"Name\":\"Roger\",\"Pitch\":300,\"speaker\":\"QJCS\",\"Speed\":3,\"gender\":\"F\",\"language\":\"CHS\",\"Pitch\": 300, \"Volume\": 3, \"background_music\": 4},\"text\":\" 灵伴科技灵伴合成云能够将文本高效准确的转化为清晰流畅的自然语音，开发者通过简单调用，即可在产品中实现多语种多音色的语音合成。灵伴合成云提供标准的开发接口，完美支持桌面端和移动端的主流操作系统\"}";
	//char task[] = "{\"task\":{\"speaker\":1,\"language\":\"ENG\",\"background_music\":1,\"Volume\":3,\"Speed\":3,\"Pitch\":3},\"text\":\"北京灵伴未来科技有限公司，成立于2014年2月。公司致力于语音、语言、视觉分析理解技术的研发，并开展多行业领域的深度应用业务。灵伴语音云是灵伴科技旗下完全自主知识产权的语音技术应用开放平台，欢迎广大开发者来体验和使北京灵伴未来科技有限公司，成立于2014年2月。公司致力于语音、语言、视觉分析理解 术的研发，并开展多行业领域的深度应用业务。灵伴语音云是灵伴科技旗下完全自主知识产权的语音技术应用开放平台\"}";
	char task[] = "{\"task\":{\"speaker\":1,\"language\":\"ENG\",\"background_music\":1,\"Volume\":3,\"Speed\":3,\"Pitch\":3},\"text\":\"你好\"}";

    s_send (requester, task);
	printf("send:%s\n", task);
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	int size = zmq_recv (requester, buffer, sizeof(buffer), 0);
	buffer[size] = 0;
	printf("\n%s \n\n\n", buffer);

	/*	
	FILE *fp = fopen("tts.data", "wb");

	char *buffer = (char *)malloc(2000*1024);
	if (!buffer) {
		printf("malloc error");
		return -1;
	}
	
	memset(buffer, 0, 2000*1024);
	
	int length = 2000*1024;
	void *tmp;
	
	int size = zmq_recv (requester, buffer, 2000*1024, 0);
	fwrite(buffer, size, 1, fp);
	printf("write size = %d\n", size);

	fclose(fp);
	*/
	
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}
