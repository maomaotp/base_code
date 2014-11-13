#include "tools.h"

void shield_stderr()
{
	int fd1=0,fd2=0;
	dup2(1,fd1);
	dup2(2,fd2);

	close(1);
	close(2);

	return;
}

void comeback_stderr()
{
	freopen("/dev/tty","w",stdout);
	freopen("/dev/tty","w",stderr);
	return;
}



void itoa(int num,char *buffer)
{
	int i = 0;
	int j = 0;
	char tmp;
	do{
		buffer[i] = (num % 10) + '0';
		++i;
	}while(num/=10);
	
	j = (strlen(buffer));
	
	for(i = 0;i < (j / 2);i++){
		tmp = buffer[i];
		buffer[i] = buffer[j - i - 1];
		buffer[j - i - 1] = tmp;
	}
	return ;
}
void time_convert_string(char *struct_time,int time,int flag)
{
	char time_buffer[ToolBuffSize];
	memset(time_buffer,0,sizeof(time_buffer));
	itoa(time,time_buffer);
	if(flag)
		strcat(struct_time,time_buffer);
	else
		strcpy(struct_time,time_buffer);
	return;
}

int my_pow(int num,int n)
{
	int sum = 1;	
	int i = 1;
	for(i = 1;i <= n;i++)
	{
		sum *= num;
	}
	return sum;
}

int hex2d(char *p)
{
	int sum = 0;
	unsigned int i = 0;
	for(i = 0;i < strlen(p);i++)	
	{
		switch(p[i])
		{
			case 'A':
				sum += (10 * my_pow(16,(strlen(p) - i -1)));
				break;
			case 'B':
				sum += (11 * my_pow(16,(strlen(p) - i -1)));
				break;
			case 'C':
				sum += (12 * my_pow(16,(strlen(p) - i -1)));
				break;
			case 'D':
				sum += (13 * my_pow(16,(strlen(p) - i -1)));
				break;
			case 'E':
				sum += (14 * my_pow(16,(strlen(p) - i -1)));
				break;
			case 'F':
				sum += (15 * my_pow(16,(strlen(p) - i -1)));
				break;
			default:
				if(p[i] >= '0' && p[i] <= '9')
				{
					sum += ((p[i] - '0') * my_pow(16,(strlen(p) - i -1)));		
				}
		}
	}
	return sum;
}

bool check_num(char *buffer,int n)
{
	int i = 0;
	for(i = 0;i < n;i++) {
		if((buffer[i] >= '0') && (buffer[i] <= '9')) {
			continue;
		}
		else {
			return false;
		}
	}

	return true;
}
