push.c                                                                                              0000644 0000000 0000000 00000004145 12453430065 010705  0                                                                                                    ustar   root                            root                                                                                                                                                                                                                   //  Task ventilator
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
                                                                                                                                                                                                                                                                                                                                                                                                                           zhelpers.h                                                                                          0000644 0000000 0000000 00000010752 12453364575 011604  0                                                                                                    ustar   root                            root                                                                                                                                                                                                                   /*  =====================================================================
    zhelpers.h
    Helper header file for example applications.
    =====================================================================
*/

#ifndef __ZHELPERS_H_INCLUDED__
#define __ZHELPERS_H_INCLUDED__

//  Include a bunch of headers that we will need in the examples

#if (defined (WIN32))
#   include <windows.h>
#endif

#include <zmq.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <assert.h>
#include <signal.h>

//  Version checking, and patch up missing constants to match 2.1
#if ZMQ_VERSION_MAJOR == 2
#   error "Please upgrade to ZeroMQ/3.2 for these examples"
#endif

//  Provide random number from 0..(num-1)
#if (defined (WIN32))
#   define randof(num)  (int) ((float) (num) * rand () / (RAND_MAX + 1.0))
#else
#   define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))
#endif


//  Receive 0MQ string from socket and convert into C string
//  Caller must free returned string. Returns NULL if the context
//  is being terminated.
static char *
s_recv (void *socket) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
 //   if (size == -1)
 //       return NULL;
 //   if (size > 255)
 //       size = 255;
    buffer [size] = 0;
    return strdup (buffer);
}

//  Convert C string to 0MQ string and send to socket
static int
s_send (void *socket, char *string) {
    int size = zmq_send (socket, string, strlen (string), 0);
    return size;
}

//  Sends string as 0MQ string, as multipart non-terminal
static int
s_sendmore (void *socket, char *string) {
    int size = zmq_send (socket, string, strlen (string), ZMQ_SNDMORE);
    return size;
}

//  Receives all message parts from socket, prints neatly
//
static void
s_dump (void *socket)
{
    puts ("----------------------------------------");
    while (1) {
        //  Process all parts of the message
        zmq_msg_t message;
        zmq_msg_init (&message);
        int size = zmq_msg_recv (&message, socket, 0);

        //  Dump the message as text or binary
        char *data = (char*)zmq_msg_data (&message);
        int is_text = 1;
        int char_nbr;
        for (char_nbr = 0; char_nbr < size; char_nbr++)
            if ((unsigned char) data [char_nbr] < 32
            ||  (unsigned char) data [char_nbr] > 127)
                is_text = 0;

        printf ("[%03d] ", size);
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if (is_text)
                printf ("%c", data [char_nbr]);
            else
                printf ("%02X", (unsigned char) data [char_nbr]);
        }
        printf ("\n");

        int more;        //  Multipart detection
        more = 0;
        size_t more_size = sizeof (more);
        zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
        zmq_msg_close (&message);
        if (!more)
            break;      //  Last message part
    }
}

//  Set simple random printable identity on socket
//
static void
s_set_id (void *socket)
{
    char identity [10];
    sprintf (identity, "%04X-%04X", randof (0x10000), randof (0x10000));
    zmq_setsockopt (socket, ZMQ_IDENTITY, identity, strlen (identity));
}


//  Sleep for a number of milliseconds
static void
s_sleep (int msecs)
{
#if (defined (WIN32))
    Sleep (msecs);
#else
    struct timespec t;
    t.tv_sec  =  msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep (&t, NULL);
#endif
}

//  Return current system clock as milliseconds
static int64_t
s_clock (void)
{
#if (defined (WIN32))
    SYSTEMTIME st;
    GetSystemTime (&st);
    return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

//  Print formatted string to stdout, prefixed by date/time and
//  terminated with a newline.

static void
s_console (const char *format, ...)
{
    time_t curtime = time (NULL);
    struct tm *loctime = localtime (&curtime);
    char *formatted = (char*)malloc (20);
    strftime (formatted, 20, "%y-%m-%d %H:%M:%S ", loctime);
    printf ("%s", formatted);
    free (formatted);

    va_list argptr;
    va_start (argptr, format);
    vprintf (format, argptr);
    va_end (argptr);
    printf ("\n");
}

#endif  //  __ZHELPERS_H_INCLUDED__
                      Makefile                                                                                            0000644 0000000 0000000 00000000226 12453435665 011231  0                                                                                                    ustar   root                            root                                                                                                                                                                                                                   
all: pull.c push.c 
	gcc -o push push.c /usr/local/lib/libzmq.so;
	gcc -o pull pull.c /usr/local/lib/libzmq.so;
clean:
	rm -rf  *.o server recv.data
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          