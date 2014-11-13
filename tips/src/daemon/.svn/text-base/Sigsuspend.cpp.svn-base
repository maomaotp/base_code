#include "Sigsuspend.h"

/* Thread specied var, the status*/  
static __thread int g_bSuspend;  

void suspend_handler(int signum)  
{  
	g_bSuspend = 1;  

	sigset_t nset;  
	pthread_sigmask(0, NULL, &nset);  
	/* make sure that the resume is not blocked*/  
	sigdelset(&nset, SIGRESUME);  
	while(g_bSuspend) sigsuspend(&nset);  
}  

void resume_handler(int signum)  
{  
	g_bSuspend = 0;  
}  

int suspend( pthread_t thread)  
{  
	return pthread_kill( thread, SIGSUSPEND);  
}  

int resume( pthread_t thread)  
{  
	return pthread_kill (thread, SIGRESUME);  
}  

int InitSigsuspend()
{  
	/*suspend*/
	struct sigaction suspendsa = {0};  
	suspendsa.sa_handler =  suspend_handler;  
	sigaddset(&suspendsa.sa_mask, SIGRESUME);  
	sigaction( SIGSUSPEND, &suspendsa, NULL);  

	/*resume*/ 
	struct sigaction resumesa = {0};  
	resumesa.sa_handler = resume_handler;  
	sigaddset(&resumesa.sa_mask, SIGSUSPEND);  
	sigaction( SIGRESUME, &resumesa, NULL);  

	return 0;  
}
