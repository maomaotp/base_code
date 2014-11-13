#include "tipsmanager.h"
using namespace std;
int TipsManager::Get_Mem()
{
	int i = 0;
	char buffer[1024];
	FILE* fp = popen ("awk '{print $2}' /proc/meminfo","r");
	memset (buffer, 0, sizeof (buffer));

	while (fgets(buffer, sizeof (buffer), fp))
	{
		++ i;
		if (1 == i)
		{
			Memtotal = new char[strlen(buffer) + 1];
			strcpy (Memtotal, buffer);
			Memtotal[strlen(Memtotal) - 1] = '\0';	
		}
		else if (2 == i)
		{
			Memfree = new char[strlen(buffer) + 1 ];
			strcpy (Memfree, buffer);
			Memfree[strlen(Memfree) - 1] = '\0';
		}
		else if (3 == i)
		{
			Buffers = new char[strlen(buffer) + 1];
			strcpy (Buffers, buffer);
			Buffers[strlen(Buffers) - 1] = '\0';
		}
		else if (4 == i)
		{
			Cached = new char[strlen(buffer) + 1];
			strcpy (Cached, buffer);
			Cached[strlen(Cached) - 1] = '\0';
		}
		if(4 == i)
			break;
	}
	total = atoi (Memtotal);
	cached = atoi (Cached);
	buffers = atoi (Buffers);
	memfree = atoi (Memfree);
	delete Memtotal;
	delete Memfree;
	delete Buffers;
	delete Cached;
	char temp[50];
	memset (temp, 0, sizeof(temp));
	pclose(fp);
	if (cached > total)
	{
		mem = 100 - memfree / total * 100;
		return mem;
	}
	else
	{
		mem = 100 - (memfree + cached + buffers) / total * 100;
		return mem;
	}
}
int TipsManager::Get_Cpu()
{
	char str[5][1024];
	char number[5][1024];
	int i = 0, count = 0;
	char buffer[1024];
	memset (buffer, 0, sizeof (buffer));
	for (i = 0; i < 5; i++)
	{
			memset (str[i], 0, sizeof (str[i]));
			memset (number[i], 0, sizeof (number[i]));		
	}
	i = 0;
	FILE* fp = popen ("vmstat", "r");
	//fread(buffer,1024,1,fp);
	//cout << "buffer is " << endl;
	//cout << buffer;

	while (fgets (buffer, sizeof(buffer), fp))
	{
		++i;
		cout <<  i <<"::::"<<buffer << endl;
		if (1 == i)
				continue;
		char temp[1024];
		memset (temp, 0, sizeof (temp));
		strcpy (temp, buffer);
		char* token = strtok (temp, " ");
		cout << i<<"token:::::"<<token<<endl;
		int j = 0, num = 0, cnt = 0, sub = 0;
 		while((token = strtok(NULL, " ")))
		{
			cout << i<<"token:::::"<<token<<endl;
			if (strstr (token, "us"))
			{
				strcpy(str[j++], token);
				str[j - 1][strlen(str[j - 1])] = '\0';
 				while((token = strtok(NULL, " ")))
				{
					cout << i<<"token:::::"<<token<<endl;
					strcpy(str[j++], token);
					str[j - 1][strlen(str[j - 1])] = '\0';
				}
				count = cnt;
				break;
			}
			if(count == 0)
				cnt++;
			if(3 == i)
			{
				sub++;
				if(sub == count + 1)
				{
					cout << i<<"toke2222222222n:::::"<<token<<endl;
					strcpy (number[num++], token);
					number[num - 1][strlen(number[num -1])] = '\0';
 					while((token = strtok(NULL, " ")))
					{
						cout << i<<"toke2222222222n:::::"<<token<<endl;
						strcpy (number[num++], token);
						number[num - 1][strlen(number[num -1])] = '\0';
					}
				goto compute;
				}
			}		
		}
	}
compute:
		pclose(fp);
		double cpu[5];
		char buf[1024];
//hyj		int h = 0;
		memset (buf, 0, sizeof(buf));
		for(i = 0; i < 5; i++)
		{
			//for(h=0;h < 5;h++)
			//{
			//	cout << h << endl << number[h] << endl;
			//	cout << number[h] << endl;
			//}
			//break;
			if(number[i] == NULL)
					cpu[i] = 0;
			else
					cpu[i] = (double)atoi(number[i]);
		}
		for(i=0;i < 5;i++)
		{
			cout << i <<"::::"<< cpu[i] << endl;
		}
		double tm = (cpu[0] + cpu[1] + cpu[3] + cpu[4]) / (cpu[0] + cpu[1] + cpu[2] + cpu[3] + cpu[4]) * 100;
		this->cpu = (int)tm;
		return this->cpu;
}
double TipsManager::Get_Disk()
{	
	int sign = 0;
	int ret;
	struct statfs st;
	double totalBlock, /*hyj totalSize,*/ freeSize;
	FILE* fp = popen ("df -hl", "r");
	char buf[1024];
	memset (buf, 0, sizeof (buf));
	if (NULL == fp)
		fprintf(stderr, "Error\n");
	while (fgets (buf, sizeof (buf), fp))
	{
		sign++;
		if (1 == sign)
			continue;
		buf[strlen(buf) - 1] = '\0';
		if (!strchr(buf, ' '))
			continue;
		char* p = strrchr (buf, ' ');
		ret = statfs (p + 1, &st);
		if (-1 == sign)
			fprintf (stderr, "Statfs error:%s\n", strerror(errno));
		totalBlock = st.f_bsize;
		freeSize += totalBlock * st.f_bfree;
	}
	pclose(fp);
	this->disk = freeSize / (1024 * 1024 * 1024);
	return this->disk;
}
void TipsManager::status_Init(struct status_struct *status)
{
	Get_Mem();
	//cout <<"Mem =========" <<mem <<endl;
	Get_Cpu();
	//cout <<"cpu ==========" <<cpu <<endl;
	Get_Disk();	
	//cout <<"disk =========" <<disk <<endl;
	status->mem = this->mem;
	status->cpu = this->cpu;
	status->disk = this->disk;	
}
