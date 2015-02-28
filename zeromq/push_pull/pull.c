//  Task worker
//  Connects PULL socket to tcp://localhost:5557
//  Collects workloads from ventilator via that socket
//  Connects PUSH socket to tcp://localhost:5558
//  Sends results to sink via that socket

#include "zhelpers.h"

int main (void)
{
    //  Socket to receive messages on
    void *context = zmq_ctx_new ();
    void *receiver = zmq_socket (context, ZMQ_PULL);
    zmq_connect (receiver, "tcp://192.168.1.120:5557");

    void *res = zmq_socket (context, ZMQ_PUSH);
    zmq_connect (res, "tcp://192.168.1.120:5558");

    //  Process tasks forever
	while (1) {
        char *string = s_recv (receiver);
		printf("task:%s start\n", string);

		char *buf = (char *)malloc(20*1024);
		FILE *fp = fopen("test.data", "r");

		int i=0;
		while(fgets(buf, 20*1024, fp)){
			char tmp[20];
			s_send(res, buf);
			memset(buf, 0, 20*1024);
		}

		printf("task:%s end\n", string);
		if (atoi(string) >= 20){
			s_send(res, "end");
			//break;
		}
		fclose(fp);
        free(string);
		free(buf);
	}

    zmq_close (receiver);
    zmq_close (res);
    zmq_ctx_destroy (context);
    return 0;
}
