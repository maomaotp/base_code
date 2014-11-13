#include "getmac.h"

void CGetMac::ip_change_mac (uint32_t ip, char* mac)
{
		map <uint32_t, struct mac>::iterator map_it = mac_map.begin();

		while (mac_map.end() != map_it)
		{
			if (map_it->first == ip)
			{
					strcpy (mac, map_it->second.mac);
					return;
			}
			map_it ++;
		}
	//	getmac ((char*)"eth3",ip,mac);
		getmac ((char*)"eth1",ip,mac);
		strcpy(mac_map[ip].mac,mac);
		return;
}


void CGetMac::getmac (char *device, uint32_t ip, char* ip_mac)
{
	 int                 sockfd;   
    struct sockaddr_ll  my_etheraddr;   
    unsigned char       *pktbuf;   
    unsigned char       ackbuf[60];   
       
    unsigned char       my_mac[6];   
    unsigned long       my_ip;   
    int                 my_ifindex;   
       
    int                 nfound;   
    struct timeval      time_out;   
    fd_set              readset,writeset;   
       
    get_local_mac((char*)"eth1", my_mac);   
   // get_local_mac((char*)"eth3", my_mac);   
    my_ip = get_local_ip((char*)"eth1");   
   // my_ip = get_local_ip((char*)"eth3");   
       
    /*  
     * Create sockaddr_ll( link layer ) struct  
     */  
    memset(&my_etheraddr, 0, sizeof(my_etheraddr));   
    my_etheraddr.sll_family = AF_PACKET;   
    my_etheraddr.sll_protocol = htons(ETH_P_ARP);   
    my_etheraddr.sll_ifindex = get_ifindex((char*)"eth1");   
   // my_etheraddr.sll_ifindex = get_ifindex((char*)"eth3");   
       
    /*  
     * Create pf_packet socket  
     */  
    sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));   
    if (sockfd < 0)   
    {   
        perror("socket");   
        exit(-1);   
    }   
    // bind    
    if (bind(sockfd, (struct sockaddr*)&my_etheraddr, sizeof(my_etheraddr)))   
    {   
        perror("bind");   
        exit(-1);   
    }   
       
    /*  
     * Build arp packet  
     */  
    pktbuf = BuildArpRequestPacket(my_mac, my_ip, ip);   
       
    //printf("Send ARP packet:\n");   
  //  print_data(pktbuf);   
    print_ARPMsg((struct arp_packet*)pktbuf,ip_mac);   
       
    if (sendto(sockfd, pktbuf, 80, 0, (struct sockaddr *)&my_etheraddr,sizeof(my_etheraddr)) <= 0)   
    {   
        perror("sendto");   
        exit(-1);   
    }   
       
    /*  
     * Receive arp packet  
     */  
    // set timeout value   
    time_out.tv_sec = 3;   
    time_out.tv_usec = 0;   
       
    FD_ZERO( &readset );   
    FD_ZERO( &writeset );   
    FD_SET( sockfd,&readset );   
    nfound = select( sockfd+1,&readset,&writeset,NULL,&time_out );   
       
    if (nfound <= 0)   
    {   
       // printf("Can't connect to %s\n", inet_ntoa());   
        exit(-1);   
    }   
    recvfrom (sockfd, ackbuf, 60, 0, NULL, 0);   
   // printf("Receive ARP packet:\n");   
   // print_data(ackbuf);   
    print_ARPMsg((struct arp_packet*)ackbuf, ip_mac);   
       
    close(sockfd);  
}
