#include <sys/socket.h>   
#include "arp.h"   
#include <stdlib.h>   
#include <stdio.h>   
#include <string.h>   
#include <arpa/inet.h>   
#include <unistd.h>   
#include <sys/ioctl.h>   
void print_ARPMsg(struct arp_packet *packet, char* my_mac)   
{   
    unsigned char *mac;   
        struct in_addr ip;   
     char buffer[1024];
		 char buf[1024];
		 memset (buffer, 0, sizeof (buffer));
		 memset (buf, 0, sizeof (buf));
   // printf("Source MAC\t\tSource IP\t\tDestinate MAC\t\tDestinate IP\n");   
       
    mac = packet->eth_src_addr;   
    ip.s_addr = packet->inet_src_addr;   
   // printf("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\t", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);   
    sprintf(buffer,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\t", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);   
   // printf("%s\t\t", inet_ntoa(ip));   
          
    mac = packet->eth_dst_addr;   
    ip.s_addr = packet->inet_dst_addr;   
    //printf("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\t\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);   
    sprintf(buf,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\t", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);   
   // printf("%s\n", inet_ntoa(ip)); 
		//fprintf (stdout, "buffer = %s\n", buffer);
		//fprintf (stdout, "buf = %s\n", buf);
		strcpy (my_mac, buffer);
}   
void print_data(unsigned char *buf)   
{   
    int i;   
    if(buf != NULL)   
    {   
        for(i=0; i<60 ; i++)   
        {   
            if(  i!=0 && i%16==0 )   
                fprintf(stdout,"\n" );
            fprintf(stdout,"%02X", buf[i]);   
        }   
        fprintf(stdout, "\n");   
    }   
}   
unsigned char* BuildArpRequestPacket(unsigned char* Eth_Src_Addr,   
        unsigned long Inet_Src_Addr,unsigned long Inet_Dst_Addr)   
{   
    static struct arp_packet packet;   
       
    memset (packet.ethd.h_dest, 0xff, ETH_ALEN);   
    memcpy (packet.ethd.h_source, Eth_Src_Addr, ETH_ALEN);   
    packet.ethd.h_proto = htons (ETH_P_ARP);   
    memset (&(packet.arphd), 0, sizeof (struct arphdr));   
    packet.arphd.ar_hrd = htons (ARPHRD_ETHER);   
    packet.arphd.ar_pro = htons (ETH_P_IP);   
    packet.arphd.ar_hln = ETH_ALEN;               /* 6 */  
    packet.arphd.ar_pln = 4;   
    packet.arphd.ar_op = htons (ARPOP_REQUEST);   
       
    memcpy(packet.eth_src_addr, Eth_Src_Addr, 6);   
    packet.inet_src_addr = Inet_Src_Addr;   
    memset(packet.eth_dst_addr, 0, 6);   
    packet.inet_dst_addr = Inet_Dst_Addr;   
       
    memset(packet.padding, 0, 18);   
       
    return (unsigned char*)&packet;   
}   
int get_local_mac(char *dev,unsigned char *mac)   
{   
#if 0 
	int sfd;
	struct ifreq ifreq1;
	struct sockaddr_in *addr = NULL;
	unsigned char mac[6];
	sfd = socket(PF_INET,SOCK_DGRAM,0);
	
	strcpy(ifreq1.ifr_name,dev);//eth0网卡名，也可搜索得到本地网卡名称
	ioctl(sfd,SIOCGIFHWADDR,&ifreq1);
	
	memcpy(mac,ifreq1.ifr_hwaddr.sa_data,6);
	ioctl(sfd,SIOCGIFADDR,&ifreq1);
	close(sfd);
#endif
#if 1 
    int s = socket(AF_INET, SOCK_DGRAM, 0);    
    struct ifreq req;    
    int err;    
    strcpy(req.ifr_name, dev);    
    err = ioctl(s, SIOCGIFHWADDR, &req);    
    if (err == -1)    
	{
    		close(s);    
        	return err;    
	}
    memcpy(mac, req.ifr_hwaddr.sa_data,6);    
    close(s);    
    return 0;    
#endif
}   
unsigned long get_local_ip(char *dev)   
{   
    int s;    
    struct ifreq req;    
    int err;    
    strcpy(req.ifr_name, dev);    
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)   
    {   
        perror("socket()");   
        return -1;   
    }   
    err = ioctl(s, SIOCGIFADDR, &req);    
    close(s);    
    if (err == -1)    
        return err;    
    return ((struct sockaddr_in*)&req.ifr_addr)->sin_addr.s_addr;   
}   
int get_ifindex(char *dev)   
{   
    int s;     
    struct ifreq req;    
    int err;    
    strcpy(req.ifr_name, dev);    
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)   
    {   
        perror("socket()");   
        return -1;   
    }   
    err = ioctl(s, SIOCGIFINDEX, &req);    
    close(s);    
    if (err == -1)    
        return err;    
           
    return req.ifr_ifindex;   
}  
