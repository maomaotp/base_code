#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pcap.h>
//#include <glib.h>
#define PCAP_FILE_NAME "mypcapfile.pcap"
static pcap_t *_pcap_handle = NULL;
static int _pcap_datalink_type = 0;
static file_flag = 0;
static netshark_flag = 1;
pcap_dumper_t * dumpfp = NULL;
static struct stat file_stat;
static void pcap_packet_callback(u_char * args, const struct pcap_pkthdr *header, const u_char * packet)
{
	static int i = 0;
	i++;
	printf("%d\n",i);
	if(0 == netshark_flag)
	{
		exit(-1);
	}
	if(0 == file_flag)
	{
		dumpfp = pcap_dump_open(_pcap_handle, PCAP_FILE_NAME);
		if(dumpfp == NULL)
		{
			printf("Error on opening output file\n");
			exit(-1);
		}
		file_flag = 1;
	}
#if 1
	if(1 == file_flag)
	{
		stat(PCAP_FILE_NAME,&file_stat);
		if(file_stat.st_size < (16*1024*1024))
		{
			pcap_dump((u_char*)dumpfp, header, packet);
		//	file_flag = 0;
		//	netshark_flag = 0;
		}
		else
		{
			pcap_dump_close(dumpfp);
			file_flag = 0;
			netshark_flag = 0;
		}
	}
#endif
}
int main()
{
	char errBuf[PCAP_ERRBUF_SIZE], * device;
	device = pcap_lookupdev(errBuf);
//	device = "eth2";
    if(device)
  	{
      	printf("success: device: %s\n", device);
    }
    else 
    {
   		printf("error: %s\n", errBuf);
       	exit(-1);
    }
    _pcap_handle = pcap_open_live(device, 65535, 1, 0, errBuf);
//    _pcap_datalink_type = pcap_datalink(_pcap_handle);
	if (_pcap_handle != NULL) 
	{
		pcap_loop(_pcap_handle, -1, &pcap_packet_callback, NULL);
	}
	return 0;
}
