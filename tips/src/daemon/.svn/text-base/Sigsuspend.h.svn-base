#ifndef _Sigsuspend_H__
#define _Sigsuspend_H__
#include <stdio.h>  
#include <pthread.h>  
#include <unistd.h>  
#include <signal.h>  

#define SIGRESUME SIGUSR2  
#define SIGSUSPEND SIGUSR1  

void suspend_handler(int signum);  
void resume_handler(int signum);  

int suspend( pthread_t thread);
int resume( pthread_t thread);

int InitSigsuspend();

#endif
