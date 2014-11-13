#include "wtpcap.h"
void Pcap::WtPcapFileH(FILE *fp,char *PcapMemBuff)
{
	struct pcap_file_head pcapH;	
	memset(&pcapH,0,sizeof(pcapH));
	pcapH.magic = PMAGIC;		
	pcapH.version_major = PVMAJOR;
	pcapH.version_minor = PVMINOR;
	pcapH.thiszone = THISZONE;
	pcapH.sigfigs = SIGFIGS;
	pcapH.snaplen = SNAPLEN;
	pcapH.linktype = LINKETHER;
	if(fp && !PcapMemBuff) {
		fwrite(&pcapH,sizeof(pcapH),1,fp);
	}
	else {
		memcpy(PcapMemBuff,&pcapH,sizeof(pcapH)); 
	}
	return;
}
void Pcap::WtPcapMacFrame(char *PcapMemBuff,struct macframes *MF)
{
	memcpy(PcapMemBuff + PcapPacketHeadSize,MF,sizeof(macframes));
	return ;
}
void Pcap::WtPcapData(char *PcapMemBuff,void *addr,int ip_len)
{
	memcpy(PcapMemBuff + PcapPacketHeadSize + MacFrameLen,addr,ip_len);
	return;
}
void Pcap::WtPcapDataH(char *PcapMemBuff,int Caplen,int Truelen)
{
	struct pcap_packet_head packH;
	memset(&packH,0,sizeof(packH));
	gettimeofday((struct timeval*)&(packH.ts), NULL);
	packH.caplen = Caplen;
	packH.len = Truelen;
	memcpy(PcapMemBuff,&packH,sizeof(packH));
	return ;
}	
