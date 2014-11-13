#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
//#include "glib.h"

//char * cmdstr = "netshark -i 192.168.0.12/24 -o 202.101.22.21/24 -n 21,433,80 -w 15,53,20 -p TCP -s 100 -t 60 -g 1203010088";


int parsecmd(const char* cmdstr, netshark_t * netshark_list)
{
	char cmdbuf[1024];
	char* ptr = NULL;
	char* tail_ptr = NULL;
	int offset = 0, c = 0;
	char argv[3];
	memset(cmdbuf, 0, sizeof(cmdbuf));
	memcpy(cmdbuf, cmdstr, strlen(cmdstr));
	ptr = cmdbuf;
	while(' ' == *ptr)
	{
		ptr++;
	}
	tail_ptr = ptr + strlen(ptr) - 1;
	while(' ' == *tail_ptr)
	{
		*tail_ptr-- = '\0';
	}
	if(!strncmp(ptr, "netshark",8))
	{
		char* temp_v[30]={0};
		int temp_c = 1;
		char * delim =" ";
		//	printf("%s\n",ptr);
		char* temp_str = strtok(ptr,delim);
		//	printf("temp_str=%s\n",temp_str);
		while((temp_str=strtok(NULL,delim)) != NULL)
		{
		//	printf("temp_v[%d]=%s\n",temp_c, temp_str);
			temp_v[temp_c]= temp_str;
		//	printf("temp_v[%d]=%s\n",temp_c, temp_v[temp_c]);
			temp_c++;
		}
	//	printf("temp_c=%d\n",temp_c);
#if 1
		while(1)
		{
			int option_index = 0;
			static struct option long_options={0};
			c = getopt_long(temp_c, (char* const *)temp_v, "i:o:n:w:p:s:t:g:",(const struct option*) &long_options, &option_index);
			if(c == -1)
				break;
			switch(c)
			{
				case 'i':
						printf("i with arg %s\n", optarg);
						break;
				case 'o':
						printf("o with arg %s\n", optarg);
						break;
				case 'n':
						printf("n with arg %s\n", optarg);
						break;
				case 'w':
						printf("w with arg %s\n", optarg);
						break;
				case 'p':
						printf("p with arg %s\n", optarg);
						break;
				case 's':
						printf("s with arg %s\n", optarg);
						break;
				case 't':
						printf("t with arg %s\n", optarg);
						break;
				case 'g':
						printf("g with arg %s\n", optarg);
						break;
				default:
						break;
			}
		}
	}
#endif
}
