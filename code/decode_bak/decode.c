#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>
#include <time.h>
#include <net/if.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "decode.h"

#define DEBUG

char buf[20];

typedef unsigned int tSfPolicyId;

tSfPolicyId runtimePolicyId = 0;


/*
 * Function: DecodeTCP(u_int8_t *, const u_int32_t, Packet *)
 *
 * Purpose: Decode the TCP transport layer
 *
 * Arguments: pkt => ptr to the packet data
 *            len => length from here to the end of the packet
 *            p   => Pointer to packet decode struct
 *
 * Returns: void function
 */
void DecodeTCP(const u_int8_t * pkt, const u_int32_t len, Packet * p)
{
    u_int32_t hlen;

    if(len < 20) {
        p->tcph = NULL;
        return;
    }

    /* lay TCP on top of the data cause there is enough of it! */
    p->tcph = (TCPHdr *) pkt;

    /* multiply the payload offset value by 4 */
    hlen = TCP_OFFSET(p->tcph) << 2;

    if(hlen < 20) {
        p->tcph = NULL;
        return;
    }

    if(hlen > len) {
        p->tcph = NULL;
        return;
    }

    /* stuff more data into the printout data struct */
    p->sp = ntohs(p->tcph->th_sport);
    p->dp = ntohs(p->tcph->th_dport);

    /* if options are present, decode them */
    p->tcp_options_len = hlen - 20;

    /* set the data pointer and size */
    p->data = (u_int8_t *) (pkt + hlen);

    if(hlen < len) {
        p->dsize = (u_short)(len - hlen);
    }
    else {
        p->dsize = 0;
    }

	char context[20]="hello";
	//Stream5Process(p, context);

#ifdef DEBUG
    printf(">>>TCP packet>>>\n");
    printf("p->th_sport:%u\n", p->sp);
    printf("p->th_dport:%u\n\n", p->dp);
#endif
}


/*
 * Function: DecodeUDP(u_int8_t *, const u_int32_t, Packet *)
 *
 * Purpose: Decode the UDP transport layer
 *
 * Arguments: pkt => ptr to the packet data
 *            len => length from here to the end of the packet
 *            p   => pointer to decoded packet struct  
 *
 * Returns: void function
 */

#if 0
void DecodeUDP(u_int8_t * pkt, const u_int32_t len, Packet * p)
{
    struct pseudoheader 
    {
        u_int32_t sip, dip;
        u_int8_t  zero;
        u_int8_t  protocol;
        u_int16_t udplen;
    };
    u_short csum;
    u_int16_t uhlen;
    struct pseudoheader ph;

    if(len < sizeof(UDPHdr))
    {
        if(pv.verbose_flag)
        {
            ErrorMessage("Truncated UDP header (%d bytes)\n", len);
        }

        if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts) 
        {
            SnortEventqAdd(GENERATOR_SNORT_DECODE, DECODE_UDP_DGRAM_LT_UDPHDR, 
                    1, DECODE_CLASS, 3, DECODE_UDP_DGRAM_LT_UDPHDR_STR, 0);
        }

        p->udph = NULL;
        pc.discards++;

        return;
    }

    /* set the ptr to the start of the UDP header */
    p->udph = (UDPHdr *) pkt;

    uhlen = ntohs(p->udph->uh_len); 
    
    /* verify that the header len is a valid value */
    if(uhlen < UDP_HEADER_LEN)
    {
        if(pv.verbose_flag)
        {
            ErrorMessage("Invalid UDP Packet, length field < 8\n");
        }
        
        if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts)
        {
            SnortEventqAdd(GENERATOR_SNORT_DECODE, 
                    DECODE_UDP_DGRAM_INVALID_LENGTH, 1, DECODE_CLASS, 3, 
                    DECODE_UDP_DGRAM_INVALID_LENGTH_STR, 0);
        }
        p->udph = NULL;
        pc.discards++;

        return;
    }

    /* make sure there are enough bytes as designated by length field */
    if(len < uhlen)
    {
        if(pv.verbose_flag)
        {
            ErrorMessage("Short UDP packet, length field > payload length\n");
        }

        if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts) 
        {
            SnortEventqAdd(GENERATOR_SNORT_DECODE, 
                    DECODE_UDP_DGRAM_SHORT_PACKET, 1, DECODE_CLASS, 3, 
                    DECODE_UDP_DGRAM_SHORT_PACKET_STR, 0);
        }

        p->udph = NULL;
        pc.discards++;

        return;
    }

    if(pv.checksums_mode & DO_UDP_CHECKSUMS)
    {
        /* look at the UDP checksum to make sure we've got a good packet */
        ph.sip = (u_int32_t)(p->iph->ip_src.s_addr);
        ph.dip = (u_int32_t)(p->iph->ip_dst.s_addr);
        ph.zero = 0;
        ph.protocol = p->iph->ip_proto;
        ph.udplen = p->udph->uh_len; 

        if(p->udph->uh_chk)
        {
            csum = in_chksum_udp((u_int16_t *)&ph, (u_int16_t *)(p->udph), uhlen);
        }
        else
        {
            csum = 0;
        }

        if(csum)
        {
            p->csum_flags |= CSE_UDP;
            DEBUG_WRAP(DebugMessage(DEBUG_DECODE, "Bad UDP Checksum\n"););
        }
        else
        {
            DEBUG_WRAP(DebugMessage(DEBUG_DECODE, "UDP Checksum: OK\n"););
        }
    }

    DEBUG_WRAP(DebugMessage(DEBUG_DECODE, "UDP header starts at: %p\n", p->udph););

    /* fill in the printout data structs */
    p->sp = ntohs(p->udph->uh_sport);
    p->dp = ntohs(p->udph->uh_dport);

    p->data = (u_int8_t *) (pkt + UDP_HEADER_LEN);
    
    /* length was validated up above */
    p->dsize = uhlen - UDP_HEADER_LEN; 

    return;
}



/*
 * Function: DecodeICMP(u_int8_t *, const u_int32_t, Packet *)
 *
 * Purpose: Decode the ICMP transport layer
 *
 * Arguments: pkt => ptr to the packet data
 *            len => length from here to the end of the packet
 *            p   => pointer to the decoded packet struct
 *
 * Returns: void function
 */
void DecodeICMP(u_int8_t * pkt, const u_int32_t len, Packet * p)
{
    u_int16_t csum;
    u_int16_t orig_p_caplen;

    if(len < ICMP_HEADER_LEN)
    {
        if(pv.verbose_flag)
        {
            ErrorMessage("WARNING: Truncated ICMP header "
                         "(%d bytes)\n", len);
        }
        
        p->icmph = NULL;
        pc.discards++;

        return;
    }

    /* set the header ptr first */
    p->icmph = (ICMPHdr *) pkt;

    switch (p->icmph->type)
    {
        case ICMP_ECHOREPLY:
        case ICMP_DEST_UNREACH:
        case ICMP_SOURCE_QUENCH:
        case ICMP_REDIRECT:
        case ICMP_ECHO:
        case ICMP_ROUTER_ADVERTISE:
        case ICMP_ROUTER_SOLICIT:
        case ICMP_TIME_EXCEEDED:
        case ICMP_PARAMETERPROB:
        case ICMP_INFO_REQUEST:
        case ICMP_INFO_REPLY:
            if (len < 8)
            {
                if(pv.verbose_flag)
                {
                    ErrorMessage("Truncated ICMP header(%d bytes)\n", len);
                }

                if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts)
                {
                    SnortEventqAdd(GENERATOR_SNORT_DECODE, 
                            DECODE_ICMP_DGRAM_LT_ICMPHDR, 1, DECODE_CLASS, 3, 
                            DECODE_ICMP_DGRAM_LT_ICMPHDR_STR, 0);
                }

                p->icmph = NULL;
                pc.discards++;

                return;
            }

            break;

        case ICMP_TIMESTAMP:
        case ICMP_TIMESTAMPREPLY:
            if (len < 20)
            {
                if(pv.verbose_flag)
                {
                    ErrorMessage("Truncated ICMP header(%d bytes)\n", len);
                }

                if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts)
                {
                    SnortEventqAdd(GENERATOR_SNORT_DECODE, 
                            DECODE_ICMP_DGRAM_LT_TIMESTAMPHDR, 1, DECODE_CLASS,
                            3, DECODE_ICMP_DGRAM_LT_TIMESTAMPHDR_STR, 0);
                }

                p->icmph = NULL;
                pc.discards++;

                return;
            }

            break;

        case ICMP_ADDRESS:
        case ICMP_ADDRESSREPLY:
            if (len < 12)
            {
                if(pv.verbose_flag)
                {
                    ErrorMessage("Truncated ICMP header(%d bytes)\n", len);
                }


                if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts)
                {
                    SnortEventqAdd(GENERATOR_SNORT_DECODE, 
                            DECODE_ICMP_DGRAM_LT_ADDRHDR, 1, DECODE_CLASS, 3, 
                            DECODE_ICMP_DGRAM_LT_ADDRHDR_STR, 0);
                }

                p->icmph = NULL;
                pc.discards++;

                return;
            }

            break;
    }


    if(pv.checksums_mode & DO_ICMP_CHECKSUMS)
    {
        csum = in_chksum_icmp((u_int16_t *)p->icmph, len);

        if(csum)
        {
            p->csum_flags |= CSE_ICMP;

            DEBUG_WRAP(DebugMessage(DEBUG_DECODE, "Bad ICMP Checksum\n"););
        }
        else
        {
            DEBUG_WRAP(DebugMessage(DEBUG_DECODE,"ICMP Checksum: OK\n"););
        }
    }

    p->dsize = (u_short)(len - ICMP_HEADER_LEN);
    p->data = pkt + ICMP_HEADER_LEN;

    DEBUG_WRAP(DebugMessage(DEBUG_DECODE, "ICMP type: %d   code: %d\n", 
                p->icmph->code, p->icmph->type););

    switch(p->icmph->type)
    {
        case ICMP_ECHOREPLY:
            /* setup the pkt id ans seq numbers */
            p->dsize -= sizeof(struct idseq);
            p->data += sizeof(struct idseq);
            break;

        case ICMP_ECHO:
            /* setup the pkt id and seq numbers */
            p->dsize -= sizeof(struct idseq);   /* add the size of the 
                                                 * echo ext to the data
                                                 * ptr and subtract it 
                                                 * from the data size */
            p->data += sizeof(struct idseq);
            break;

        case ICMP_DEST_UNREACH:
            {
                /* if unreach packet is smaller than expected! */
                if(len < 16)
                {
                    if(pv.verbose_flag)
                    {
                        ErrorMessage("Truncated ICMP-UNREACH "
                                     "header (%d bytes)\n", len);
                    }

                    /* if it is less than 8 we are in trouble */
                    if(len < 8)
                        break;
                }

                orig_p_caplen = len - 8;

                if(!DecodeIPOnly(pkt + 8, orig_p_caplen, p))
                {
                    if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts)
                    {
                        SnortEventqAdd(GENERATOR_SNORT_DECODE, 
                                DECODE_IPV4_DGRAM_UNKNOWN, 1, DECODE_CLASS, 3,
                                DECODE_IPV4_DGRAM_UNKNOWN_STR, 0);
                    }
                }
            }

            break;

        case ICMP_REDIRECT:
            {
                /* if unreach packet is smaller than expected! */
                if(p->dsize < 28)
                {
                    if(pv.verbose_flag)
                    {
                        ErrorMessage("Truncated ICMP-REDIRECT "
                                     "header (%d bytes)\n", len);
                    }
                        
                    /* if it is less than 8 we are in trouble */
                    if(p->dsize < 8)
                        break;
                }

                orig_p_caplen = p->dsize - 8;

                if(!DecodeIPOnly(pkt + 8, orig_p_caplen, p))
                {
                    if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts)
                    {
                        SnortEventqAdd(GENERATOR_SNORT_DECODE, 
                                DECODE_IPV4_DGRAM_UNKNOWN, 1, DECODE_CLASS, 3, 
                                DECODE_IPV4_DGRAM_UNKNOWN_STR, 0);
                    }
                }
            }

            break;
    }

    return;
}



/*
 * Function: DecodeARP(u_int8_t *, u_int32_t, Packet *)
 *
 * Purpose: Decode ARP stuff
 *
 * Arguments: pkt => ptr to the packet data
 *            len => length from here to the end of the packet
 *            p   => pointer to decoded packet struct
 *
 * Returns: void function
 */
void DecodeARP(u_int8_t * pkt, u_int32_t len, Packet * p)
{

    pc.arp++;
    p->ah = (EtherARP *) pkt;

    if(len < sizeof(EtherARP))
    {
        if(pv.verbose_flag)
            ErrorMessage("Truncated packet\n");

        if((runMode == MODE_IDS) && pv.decoder_flags.decode_alerts)
        {
            SnortEventqAdd(GENERATOR_SNORT_DECODE, DECODE_ARP_TRUNCATED, 1, 
                    DECODE_CLASS, 3, DECODE_ARP_TRUNCATED_STR, 0);
        }

        pc.discards++;
        return;
    }

    return;
}

#endif

void DecodeIP(const u_int8_t* pkt, const u_int32_t len, Packet* p)
{
    u_int32_t hlen;
    u_int32_t ip_len; /* length from the start of the ip hdr to the pkt end */

    /* lay the IP struct */
    p->iph = (IPHdr *) pkt;
#ifdef DEBUG
    printf(">>>>>IP packet decode>>>>>>>>\n");
    printf("src_ip:%u------%s\n", p->iph->ip_src.s_addr, inet_ntoa(p->iph->ip_src));
    printf("src_ip:%u------%s\n", p->iph->ip_dst.s_addr, inet_ntoa(p->iph->ip_dst));
    printf("ip verhl:%u\n", p->iph->ip_verhl);
    printf("ip len:%u\n", ntohs(p->iph->ip_len));
    printf("ip_ttl:%u\n", p->iph->ip_ttl);
    printf("ip_packet_protocol:%u\n\n", p->iph->ip_proto);
#endif

    /* do a little validation */
    if(len < IP_HEADER_LEN) //IP_HEADER_LEN == 20
    {
        p->iph = NULL;
        return;
    }

    if(IP_VER(p->iph) != 4)
    {
        p->iph = NULL;
        return;
    }

    /* set the IP datagram length */
    ip_len = ntohs(p->iph->ip_len);

    /* set the IP header length */
    hlen = IP_HLEN(p->iph) << 2;

    /* header length sanity check */
    if(hlen < IP_HEADER_LEN) {
        p->iph = NULL;
        return;
    }

    if (ip_len > len) {
        ip_len = len;
    }

    if(ip_len < hlen) {
        p->iph = NULL;
        return;
    }

    /* set the remaining packet length */
    ip_len -= hlen;

    /* check for fragmented packets */
    p->frag_offset = ntohs(p->iph->ip_off);

    /* 
     * get the values of the reserved, more 
     * fragments and don't fragment flags 
     */
    p->rf = (u_int8_t)((p->frag_offset & 0x8000) >> 15);
    p->df = (u_int8_t)((p->frag_offset & 0x4000) >> 14);
    p->mf = (u_int8_t)((p->frag_offset & 0x2000) >> 13);

    /* mask off the high bits in the fragment offset field */
    p->frag_offset &= 0x1FFF;

    if(p->frag_offset || p->mf) {
        /* set the packet fragment flag */
        p->frag_flag = 1;
    }
   
    /* if this packet isn't a fragment */
    if(!(p->frag_flag)) {
        switch(p->iph->ip_proto)
        {
            case IPPROTO_TCP:
                DecodeTCP(pkt + hlen, ip_len, p);
                return;

            case IPPROTO_UDP:
                //DecodeUDP(pkt + hlen, ip_len, p);
                return;

            case IPPROTO_ICMP:
                //DecodeICMP(pkt + hlen, ip_len, p);
                return;

            default:
                return;
        }
    }
    else {
        /* set the payload pointer and payload size */
        p->data = pkt + hlen;
        p->dsize = (u_short) ip_len;
    }
}

void decodeEthPkt(Packet *p, const struct pcap_pkthdr* pkthdr, const u_int8_t* pkt)
{
    u_int32_t pkt_len;
    u_int32_t cap_len;

	bzero((char *) p, sizeof(Packet));

    pkt_len = pkthdr->len;
    cap_len = pkthdr->caplen;

    p->pkth = pkthdr;
    p->pkt = pkt;
    p->eh = (EtherHdr *)pkt;

    switch(htons(p->eh->ether_type)) {
        case ETHERNET_TYPE_PPPoE_DISC:
        
        case ETHERNET_TYPE_PPPoE_SESS:
            return;
        
        case ETHERNET_TYPE_IP:
            DecodeIP(p->pkt + ETHERNET_HEADER_LEN, cap_len-ETHERNET_HEADER_LEN, p);
            return;
        
        case ETHERNET_TYPE_ARP:
        
        case ETHERNET_TYPE_REVARP:
        
        case ETHERNET_TYPE_8021Q:
        
        default:
            return;
    }

    return ;
}

void mycallback(u_char* argument, const struct pcap_pkthdr* pkthdr, const u_char* pkt)
{
    Packet p;
	
	assert(pkthdr && pkt);

#ifdef DEBUG
   printf(">>>packet length:%d\n", pkthdr->len);
   printf(">>>number of bytes:%d\n", pkthdr->caplen);
   printf(">>>recieved time:%s\n", ctime((const time_t *)&pkthdr->ts.tv_sec));
#endif

   p.packet_flags = 0;
   decodeEthPkt(&p, pkthdr, pkt);
   assert(p.pkth && p.pkt);
}

void capturePk()
{
    char pcap_err[20];

    while(1) {
        pcap_t* handle = NULL;
        //the third parameter is promisc
        if((handle = pcap_open_live(buf, 1600, 1, 0, pcap_err)) == NULL) {
            printf("pcap_open_live:%s\n", pcap_err);
            return;
        }
        pcap_loop(handle, -1, mycallback, NULL);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2) {
        printf("argv error!\n");
        exit(1);
    }

    strcpy(buf, argv[1]);
    printf("IDS start interface:%s\n", buf);

    capturePk();
    return 0;
}

