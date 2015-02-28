//  Task ventilator
//  Binds PUSH socket to tcp://localhost:5557
//  Sends batch of tasks to workers via that socket

#include "zhelpers.h"
#include <pthread.h>
#include <sys/time.h>

void *context;
struct timeval start,stop,diff;

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval*y)
{
	int nsec;
	if(x->tv_sec>y->tv_sec){
		return -1;
	}
	if( (x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec)){
		return -1;
	}
	result->tv_sec = (y->tv_sec-x->tv_sec);
	result->tv_usec = (y->tv_usec-x->tv_usec); 

	if(result->tv_usec<0){
		result->tv_sec--;
		result->tv_usec+=1000000;
	}
	return 0;
}

void *work_task(void* arg)
{
    //  Socket to send messages on
    void *sender = zmq_socket (context, ZMQ_PUSH);
    zmq_bind (sender, "tcp://*:5557");

    printf ("Sending tasks to workers…\n");

	int task_nbr = 0;
    for (task_nbr = 0; task_nbr <= 20; task_nbr++) {
        char string [10];
        sprintf (string, "%d", task_nbr);
        s_send (sender, string);
		//sleep(1);
    }

    zmq_close (sender);
}

void *recv_result(void *arg)
{
	//recv result
	void *receiver = zmq_socket (context, ZMQ_PULL);
	zmq_bind (receiver, "tcp://*:5558");

	int i=0;
	char * buf = (char *)malloc(20*1024);
	FILE *fp = fopen("recv.data", "a");
	while(1){
		memset(buf, 0, 20*1024);
		zmq_recv (receiver, buf, 20*1024, 0);
		fputs(buf,fp);
		if (strncmp(buf, "end", 3) == 0){
			printf("recved end\n");
			break;
		}
	}
    free (buf);
	fclose(fp);

	gettimeofday(&stop,0); 
	timeval_subtract(&diff,&start,&stop);
	printf("all time-> ms:%d s:%d \n", (diff.tv_usec)/1000, diff.tv_sec);

    zmq_close (receiver);
    return 0;
}

int main (void)
{
	context = zmq_ctx_new ();
	pthread_t id[2];
	//get start time
	gettimeofday(&start,0); 

	if( pthread_create(&id[0], 0, work_task,0) ) {
		printf("pthread error\n");
		exit(-1);
	}

	if( pthread_create(&id[1], 0, recv_result,0) ) {
		printf("pthread error\n");
		exit(-1);
	}

	pthread_join(id[0], NULL);
	pthread_join(id[1], NULL);


    zmq_ctx_destroy (context);

	return 0;
}
