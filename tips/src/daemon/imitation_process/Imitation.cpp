#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
using namespace std;

int main(int argc, char** argv)
{
	  	FILE *fp;
  		char id_buf[10];
  		pid_t pid;
  		memset(id_buf,0,10);
  		pid = getpid();
  		sprintf(id_buf,"%d",pid);
  		if(NULL == (fp = fopen("/usr/local/tipschecker/etc/tipscheck/netblockid","w+")))
  		{
    			perror("fopen");
    			return 0;
  		}
  		fwrite(id_buf,strlen(id_buf),1,fp);
  		fclose(fp);
	while(1)
		{
			sleep(1);
		}
	return 0;
}
