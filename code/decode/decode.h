#ifndef _DECODE_H_
#define _DECODE_H_

#include <arpa/inet.h>

#define TR_ALEN  6
#define IPARP_SAP  Oxaa

#define TRH_MR_LEN(trhmr)    ((ntohs((trhmr)->bcast_len_dir_lf_res) & 0x1F00) >> 8)
#define IP_HLEN(iph)         ((iph)->ip_verhl & 0x0f)
#define IP_VER(iph)          (((iph)->ip_verhl & 0xf0) >> 4)
#define IP_HLEN(iph)         ((iph)->ip_verhl & 0x0f)
#define TCP_OFFSET(tcph)    (((tcph)->th_offx2 & 0xf0) >> 4)
#define bzero(addr, count)       memset((addr), 0, (count))
#define TCP_ISFLAGSET(tcph, flags) (((tcph)->th_flags & (flags)) == (flags))
#define EXTRACT_32BITS(p)  ({ uint32_t __tmp; memmove(&__tmp, (p), sizeof(uint32_t)); (uint32_t) ntohl(__tmp);})


#define ETHERNET_TYPE_IP              0x0800
#define ETHERNET_TYPE_ARP             0x0806
#define ETHERNET_TYPE_REVARP          0x8035
#define ETHERNET_TYPE_8021Q           0x8100
#define ETHERNET_TYPE_PPPoE_SESS      0x8864
#define ETHERNET_TYPE_PPPoE_DISC      0x8863

#define ETHERNET_HEADER_LEN             14
#define IP_HEADER_LEN                   20



/* packet status flags */
#define PKT_REBUILT_FRAG     0x00000001  /* is a rebuilt fragment */
#define PKT_REBUILT_STREAM   0x00000002  /* is a rebuilt stream */
#define PKT_STREAM_UNEST_UNI 0x00000004  /* is from an unestablished stream and
                                          * we've only seen traffic in one direction */
#define PKT_STREAM_EST       0x00000008  /* is from an established stream */

#define PKT_STREAM_INSERT    0x00000010  /* this packet has been queued for stream reassembly */
#define PKT_STREAM_TWH       0x00000020  /* packet completes the 3-way handshake */
#define PKT_FROM_SERVER      0x00000040  /* this packet came from the server
                                            side of a connection (TCP) */
#define PKT_FROM_CLIENT      0x00000080  /* this packet came from the client
                                            side of a connection (TCP) */

#define PKT_PDU_HEAD         0x00000100  /* start of PDU */
#define PKT_PDU_TAIL         0x00000200  /* end of PDU */
#define PKT_UNSURE_ENCAP     0x00000400  /* packet may have incorrect encapsulation layer. */
                                         /* don't alert if "next layer" is invalid. */
#define PKT_HTTP_DECODE      0x00000800  /* this packet has normalized http */

#define PKT_IGNORE           0x00001000  /* this packet should be ignored, based on port */
#define PKT_TRUST            0x00002000  /* this packet should fallback to being whitelisted if no other verdict was specified */
#define PKT_ALLOW_MULTIPLE_DETECT 0x00004000  /* packet has either pipelined mime attachements */
                                              /* or pipeline http requests */
#define PKT_PAYLOAD_OBFUSCATE     0x00008000

#define PKT_STATELESS        0x00010000  /* Packet has matched a stateless rule */
#define PKT_PASS_RULE        0x00020000  /* this packet has matched a pass rule */
#define PKT_IP_RULE          0x00040000  /* this packet is being evaluated against an IP rule */
#define PKT_IP_RULE_2ND      0x00080000  /* this packet is being evaluated against an IP rule */

#define PKT_LOGGED           0x00100000  /* this packet has been logged */
#define PKT_PSEUDO           0x00200000  /* is a pseudo packet */
#define PKT_MODIFIED         0x00400000  /* packet had normalizations, etc. */


#define SEQ_LT(a,b)  ((int)((a) - (b)) <  0)
#define SEQ_LEQ(a,b) ((int)((a) - (b)) <= 0)
#define SEQ_GT(a,b)  ((int)((a) - (b)) >  0)
#define SEQ_GEQ(a,b) ((int)((a) - (b)) >= 0)
#define SEQ_EQ(a,b)  ((int)((a) - (b)) == 0)

typedef struct _Trh_mr
{
    u_int16_t bcast_len_dir_lf_res;
    u_int16_t rset[8];
}Trh_mr;

typedef struct _Trh_llc
{
    u_int8_t dsap;
    u_int8_t ssap;
    u_int8_t protid[3];
    u_int8_t ethertype;
}Trh_llc;


typedef struct _ip {
    int16_t family;
    int16_t bits;

    union
    {
        u_int8_t  u6_addr8[16];
        u_int16_t u6_addr16[8];
        u_int32_t u6_addr32[4];
    } ip;
} sfip_t;

typedef struct _VlanTagHdr
{
    u_int16_t vth_pri_cfi_vlan;
    u_int16_t vth_proto;  /* protocol field... */
} VlanTagHdr;

typedef struct _EtherHdr
{
    u_int8_t ether_dst[6];
    u_int8_t ether_src[6];
    u_int16_t ether_type;

} EtherHdr;

typedef struct _Trh_hdr
{
    u_int8_t ac;        /* access control field */
    u_int8_t fc;        /* frame control field */
    u_int8_t daddr[TR_ALEN];    /* src address */
    u_int8_t saddr[TR_ALEN];    /* dst address */
}Trh_hdr;

typedef struct _IPHdr
{
    u_int8_t ip_verhl;      /* version & header length */
    u_int8_t ip_tos;        /* type of service */
    u_int16_t ip_len;       /* datagram length */
    u_int16_t ip_id;        /* identification  */
    u_int16_t ip_off;       /* fragment offset */
    u_int8_t ip_ttl;        /* time to live field */
    u_int8_t ip_proto;      /* datagram protocol */
    u_int16_t ip_csum;      /* checksum */
    struct in_addr ip_src;  /* source IP */
    struct in_addr ip_dst;  /* dest IP */
}IPHdr;


typedef struct _TCPHdr
{
    u_int16_t th_sport;     /* source port */
    u_int16_t th_dport;     /* destination port */
    u_int32_t th_seq;       /* sequence number */
    u_int32_t th_ack;       /* acknowledgement number */
    u_int8_t th_offx2;     /* offset and reserved */
    u_int8_t th_flags;
    u_int16_t th_win;       /* window */
    u_int16_t th_sum;       /* checksum */
    u_int16_t th_urp;       /* urgent pointer */

}TCPHdr;


typedef struct _IPv4Hdr
{
    uint8_t ip_verhl;      /* version & header length */
    uint8_t ip_tos;        /* type of service */
    uint16_t ip_len;       /* datagram length */
    uint16_t ip_id;        /* identification  */
    uint16_t ip_off;       /* fragment offset */
    uint8_t ip_ttl;        /* time to live field */
    uint8_t ip_proto;      /* datagram protocol */
    uint16_t ip_csum;      /* checksum */
    sfip_t ip_src;          /* source IP */
    sfip_t ip_dst;          /* dest IP */
} IP4Hdr;

typedef struct _IPv6Hdr
{
    uint32_t vcl;      /* version, class, and label */
    uint16_t len;      /* length of the payload */
    uint8_t  next;     /* next header
                         * Uses the same flags as
                         * the IPv4 protocol field */
    uint8_t  hop_lmt;  /* hop limit */
    sfip_t ip_src;
    sfip_t ip_dst;
} IP6Hdr;

/* IPv6 address */
#ifndef s6_addr
struct in6_addr
{
    union
    {
        uint8_t u6_addr8[16];
        uint16_t u6_addr16[8];
        uint32_t u6_addr32[4];
    } in6_u;
#define s6_addr         in6_u.u6_addr8
#define s6_addr16       in6_u.u6_addr16
#define s6_addr32       in6_u.u6_addr32
};
#endif

typedef struct _IP6RawHdr
{
    uint32_t ip6_vtf;               /* 4 bits version, 8 bits TC,
                                        20 bits flow-ID */
    uint16_t ip6_payload_len;               /* payload length */
    uint8_t  ip6_next;                /* next header */
    uint8_t  ip6_hoplim;               /* hop limit */

    struct in6_addr ip6_src;      /* source address */
    struct in6_addr ip6_dst;      /* destination address */
} IP6RawHdr;

#define ip6flow  ip6_vtf
#define ip6plen  ip6_payload_len
#define ip6nxt   ip6_next
#define ip6hlim  ip6_hoplim
#define ip6hops  ip6_hoplim

#define IPRAW_HDR_VER(p_rawiph) \
   (ntohl(p_rawiph->ip6_vtf) >> 28)

#define IP6_HDR_LEN 40

#ifndef IP_PROTO_HOPOPTS
# define IP_PROTO_HOPOPTS    0
#endif

#define IP_PROTO_NONE       59
#define IP_PROTO_ROUTING    43
#define IP_PROTO_FRAGMENT   44
#define IP_PROTO_AH         51
#define IP_PROTO_DSTOPTS    60
#define IP_PROTO_ICMPV6     58
#define IP_PROTO_IPV6       41
#define IP_PROTO_IPIP       4

#define IP6F_OFFSET_MASK    0xfff8  /* mask out offset from _offlg */
#define IP6F_MF_MASK        0x0001  /* more-fragments flag */

#define IP6F_OFFSET(fh) ((ntohs((fh)->ip6f_offlg) & IP6F_OFFSET_MASK) >> 3)
#define IP6F_RES(fh) (fh)->ip6f_reserved
#define IP6F_MF(fh) (ntohs((fh)->ip6f_offlg) & IP6F_MF_MASK )

/* to store references to IP6 Extension Headers */
typedef struct _IP6Option
{
    uint8_t type;
    const uint8_t *data;
} IP6Option;

/* Generic Extension Header */
typedef struct _IP6Extension
{
    uint8_t ip6e_nxt;
    uint8_t ip6e_len;
    /* options follow */
    uint8_t ip6e_pad[6];
} IP6Extension;

typedef struct _IP6HopByHop
{
    uint8_t ip6hbh_nxt;
    uint8_t ip6hbh_len;
    /* options follow */
    uint8_t ip6hbh_pad[6];
} IP6HopByHop;

typedef struct _IP6Dest
{
    uint8_t ip6dest_nxt;
    uint8_t ip6dest_len;
    /* options follow */
    uint8_t ip6dest_pad[6];
} IP6Dest;

typedef struct _IP6Route
{
    uint8_t ip6rte_nxt;
    uint8_t ip6rte_len;
    uint8_t ip6rte_type;
    uint8_t ip6rte_seg_left;
    /* type specific data follows */
} IP6Route;

typedef struct _IP6Route0
{
    uint8_t ip6rte0_nxt;
    uint8_t ip6rte0_len;
    uint8_t ip6rte0_type;
    uint8_t ip6rte0_seg_left;
    uint8_t ip6rte0_reserved;
    uint8_t ip6rte0_bitmap[3];
    struct in6_addr ip6rte0_addr[1];  /* Up to 23 IP6 addresses */
} IP6Route0;

/* Fragment header */
typedef struct _IP6Frag
{
    uint8_t   ip6f_nxt;     /* next header */
    uint8_t   ip6f_reserved;    /* reserved field */
    uint16_t  ip6f_offlg;   /* offset, reserved, and flag */
    uint32_t  ip6f_ident;   /* identification */
} IP6Frag;

typedef struct _ICMP6
{
    uint8_t type;
    uint8_t code;
    uint16_t csum;

} ICMP6Hdr;

typedef struct _ICMP6TooBig
{
    uint8_t type;
    uint8_t code;
    uint16_t csum;
    uint32_t mtu;
} ICMP6TooBig;

typedef struct _ICMP6RouterAdvertisement
{
    uint8_t type;
    uint8_t code;
    uint16_t csum;
    uint8_t num_addrs;
    uint8_t addr_entry_size;
    uint16_t lifetime;
    uint32_t reachable_time;
    uint32_t retrans_time;
} ICMP6RouterAdvertisement;

typedef struct _ICMP6RouterSolicitation
{
    uint8_t type;
    uint8_t code;
    uint16_t csum;
    uint32_t reserved;
} ICMP6RouterSolicitation;

typedef struct _ICMP6NodeInfo
{
    uint8_t type;
    uint8_t code;
    uint16_t csum;
    uint16_t qtype;
    uint16_t flags;
    uint64_t nonce;
} ICMP6NodeInfo;

#define ICMP6_UNREACH 1
#define ICMP6_BIG    2
#define ICMP6_TIME   3
#define ICMP6_PARAMS 4
#define ICMP6_ECHO   128
#define ICMP6_REPLY  129
#define ICMP6_SOLICITATION 133
#define ICMP6_ADVERTISEMENT 134
#define ICMP6_NODE_INFO_QUERY 139
#define ICMP6_NODE_INFO_RESPONSE 140

/* Minus 1 due to the 'body' field  */
#define ICMP6_MIN_HEADER_LEN (sizeof(ICMP6Hdr) )

#ifdef _MSC_VER
  /* Visual C++ pragma to enable warning messages about nonstandard bit field type */
  #pragma warning( default : 4214 )
#endif


/* Can't add any fields not in the real header here
   because of how the decoder uses structure overlaying */
#ifdef _MSC_VER
  /* Visual C++ pragma to disable warning
   * messages about nonstandard bit field type
   */
  #pragma warning( disable : 4214 )
#endif

#ifndef IPPROTO_IPIP
#define IPPROTO_IPIP 4
#endif

/* GRE related stuff */
typedef struct _GREHdr
{
    uint8_t flags;
    uint8_t version;
    uint16_t ether_type;

} GREHdr;

#ifdef GRE

#ifndef IPPROTO_GRE
#define IPPROTO_GRE 47
#endif

#define GRE_TYPE_TRANS_BRIDGING 0x6558
#define GRE_TYPE_PPP            0x880B

#define GRE_HEADER_LEN 4
#define GRE_CHKSUM_LEN 2
#define GRE_OFFSET_LEN 2
#define GRE_KEY_LEN 4
#define GRE_SEQ_LEN 4
#define GRE_SRE_HEADER_LEN 4

#define GRE_CHKSUM(x)  (x->flags & 0x80)
#define GRE_ROUTE(x)   (x->flags & 0x40)
#define GRE_KEY(x)     (x->flags & 0x20)
#define GRE_SEQ(x)     (x->flags & 0x10)
#define GRE_SSR(x)     (x->flags & 0x08)
#define GRE_RECUR(x)   (x->flags & 0x07)
#define GRE_VERSION(x)   (x->version & 0x07)
#define GRE_FLAGS(x)     (x->version & 0xF8)
#define GRE_PROTO(x)  ntohs(x->ether_type)

/* GRE version 1 used with PPTP */
#define GRE_V1_HEADER_LEN 8
#define GRE_V1_ACK_LEN 4
#define GRE_V1_FLAGS(x)  (x->version & 0x78)
#define GRE_V1_ACK(x)    (x->version & 0x80)

typedef struct _ERSpanType2Hdr
{
    uint16_t ver_vlan;
    uint16_t flags_spanId;
    uint32_t pad;
} ERSpanType2Hdr;

typedef struct _ERSpanType3Hdr
{
    uint16_t ver_vlan;
    uint16_t flags_spanId;
    uint32_t timestamp;
    uint16_t pad0;
    uint16_t pad1;
    uint32_t pad2;
    uint32_t pad3;
} ERSpanType3Hdr;

#define ERSPAN_VERSION(x) ((ntohs(x->ver_vlan) & 0xf000) >> 12)
#define ERSPAN_VLAN(x) (ntohs(x->ver_vlan) & 0x0fff)
#define ERSPAN_SPAN_ID(x) (ntohs(x->flags_spanId) & 0x03ff)
#define ERSPAN3_TIMESTAMP(x) (x->timestamp)

#endif  /* GRE */


/* more macros for TCP offset */
#define TCP_OFFSET(tcph)        (((tcph)->th_offx2 & 0xf0) >> 4)
#define TCP_X2(tcph)            ((tcph)->th_offx2 & 0x0f)

#define TCP_ISFLAGSET(tcph, flags) (((tcph)->th_flags & (flags)) == (flags))

/* we need to change them as well as get them */
#define SET_TCP_OFFSET(tcph, value)  ((tcph)->th_offx2 = (unsigned char)(((tcph)->th_offx2 & 0x0f) | (value << 4)))
#define SET_TCP_X2(tcph, value)  ((tcph)->th_offx2 = (unsigned char)(((tcph)->th_offx2 & 0xf0) | (value & 0x0f)))

#ifdef _MSC_VER
  /* Visual C++ pragma to enable warning messages
   * about nonstandard bit field type
   */
  #pragma warning( default : 4214 )
#endif


typedef struct _UDPHdr
{
    uint16_t uh_sport;
    uint16_t uh_dport;
    uint16_t uh_len;
    uint16_t uh_chk;

}       UDPHdr;


typedef struct _ICMPHdr
{
    uint8_t type;
    uint8_t code;
    uint16_t csum;
    union
    {
        struct
        {
            uint8_t pptr;
            uint8_t pres1;
            uint16_t pres2;
        } param;

        struct in_addr gwaddr;

        struct idseq
        {
            uint16_t id;
            uint16_t seq;
        } idseq;

        uint32_t sih_void;

        struct pmtu
        {
            uint16_t ipm_void;
            uint16_t nextmtu;
        } pmtu;

        struct rtradv
        {
            uint8_t num_addrs;
            uint8_t wpa;
            uint16_t lifetime;
        } rtradv;
    } icmp_hun;

#define s_icmp_pptr       icmp_hun.param.pptr
#define s_icmp_gwaddr     icmp_hun.gwaddr
#define s_icmp_id         icmp_hun.idseq.id
#define s_icmp_seq        icmp_hun.idseq.seq
#define s_icmp_void       icmp_hun.sih_void
#define s_icmp_pmvoid     icmp_hun.pmtu.ipm_void
#define s_icmp_nextmtu    icmp_hun.pmtu.nextmtu
#define s_icmp_num_addrs  icmp_hun.rtradv.num_addrs
#define s_icmp_wpa        icmp_hun.rtradv.wpa
#define s_icmp_lifetime   icmp_hun.rtradv.lifetime

    union
    {
        /* timestamp */
        struct ts
        {
            uint32_t otime;
            uint32_t rtime;
            uint32_t ttime;
        } ts;

        /* IP header for unreach */
        struct ih_ip
        {
            IPHdr *ip;
            /* options and then 64 bits of data */
        } ip;

        struct ra_addr
        {
            uint32_t addr;
            uint32_t preference;
        } radv;

        uint32_t mask;

        char    data[1];

    } icmp_dun;
#define s_icmp_otime      icmp_dun.ts.otime
#define s_icmp_rtime      icmp_dun.ts.rtime
#define s_icmp_ttime      icmp_dun.ts.ttime
#define s_icmp_ip         icmp_dun.ih_ip
#define s_icmp_radv       icmp_dun.radv
#define s_icmp_mask       icmp_dun.mask
#define s_icmp_data       icmp_dun.data

}        ICMPHdr;


typedef struct _ARPHdr
{
    uint16_t ar_hrd;       /* format of hardware address   */
    uint16_t ar_pro;       /* format of protocol address   */
    uint8_t ar_hln;        /* length of hardware address   */
    uint8_t ar_pln;        /* length of protocol address   */
    uint16_t ar_op;        /* ARP opcode (command)         */
}       ARPHdr;



typedef struct _EtherARP
{
    ARPHdr ea_hdr;      /* fixed-size header */
    uint8_t arp_sha[6];    /* sender hardware address */
    uint8_t arp_spa[4];    /* sender protocol address */
    uint8_t arp_tha[6];    /* target hardware address */
    uint8_t arp_tpa[4];    /* target protocol address */
}         EtherARP;


#ifndef NO_NON_ETHER_DECODER
typedef struct _EtherEapol
{
    uint8_t  version;  /* EAPOL proto version */
    uint8_t  eaptype;  /* EAPOL Packet type */
    uint16_t len;  /* Packet body length */
}         EtherEapol;

typedef struct _EAPHdr
{
    uint8_t code;
    uint8_t id;
    uint16_t len;
}         EAPHdr;

typedef struct _EapolKey
{
  uint8_t type;
  uint8_t length[2];
  uint8_t counter[8];
  uint8_t iv[16];
  uint8_t index;
  uint8_t sig[16];
}       EapolKey;
#endif  // NO_NON_ETHER_DECODER

typedef struct _Options
{
    uint8_t code;
    uint8_t len; /* length of the data section */
    const uint8_t *data;
} Options;

/* PPPoEHdr Header; EtherHdr plus the PPPoE Header */
typedef struct _PPPoEHdr
{
    unsigned char ver_type;     /* pppoe version/type */
    unsigned char code;         /* pppoe code CODE_* */
    unsigned short session;     /* session id */
    unsigned short length;      /* payload length */
                                /* payload follows */
} PPPoEHdr;

/* PPPoE tag; the payload is a sequence of these */
typedef struct _PPPoE_Tag
{
    unsigned short type;    /* tag type TAG_* */
    unsigned short length;    /* tag length */
                            /* payload follows */
} PPPoE_Tag;

#define MPLS_HEADER_LEN    4
#define NUM_RESERVED_LABELS    16

typedef struct _MplsHdr
{
    uint32_t label;
    uint8_t  exp;
    uint8_t  bos;
    uint8_t  ttl;
} MplsHdr;

#define PGM_NAK_ERR -1
#define PGM_NAK_OK 0
#define PGM_NAK_VULN 1

typedef struct _PGM_NAK_OPT
{
    uint8_t type;     /* 02 = vuln */
    uint8_t len;
    uint8_t res[2];
    uint32_t seq[1];    /* could be many many more, but 1 is sufficient */
} PGM_NAK_OPT;

typedef struct _PGM_NAK
{
    uint32_t  seqnum;
    uint16_t  afil1;
    uint16_t  res1;
    uint32_t  src;
    uint16_t  afi2;
    uint16_t  res2;
    uint32_t  multi;
    PGM_NAK_OPT opt;
} PGM_NAK;

typedef struct _PGM_HEADER
{
    uint16_t srcport;
    uint16_t dstport;
    uint8_t  type;
    uint8_t  opt;
    uint16_t checksum;
    uint8_t  gsd[6];
    uint16_t length;
    PGM_NAK  nak;
} PGM_HEADER;

/* GTP basic Header  */
typedef struct _GTPHdr
{
    uint8_t  flag;              /* flag: version (bit 6-8), PT (5), E (3), S (2), PN (1) */
    uint8_t  type;              /* message type */
    uint16_t length;            /* length */

} GTPHdr;

#define LAYER_MAX  32

// REMEMBER match any changes you make here in:
// dynamic-plugins/sf_engine/sf_snort_packet.h
#if 0
typedef struct _Packet
{
    const DAQ_PktHdr_t *pkth;    // packet meta data
    const uint8_t *pkt;         // raw packet data

    //vvv------------------------------------------------
    // TODO convenience stuff to be refactored for layers
    //^^^------------------------------------------------

    //vvv-----------------------------
    EtherARP *ah;
    const EtherHdr *eh;         /* standard TCP/IP/Ethernet/ARP headers */
    const VlanTagHdr *vh;
    EthLlc *ehllc;
    EthLlcOther *ehllcother;
    const PPPoEHdr *pppoeh;     /* Encapsulated PPP of Ether header */
    const GREHdr *greh;
    uint32_t *mpls;

    const IPHdr *iph, *orig_iph;/* and orig. headers for ICMP_*_UNREACH family */
    const IPHdr *inner_iph;     /* if IP-in-IP, this will be the inner IP header */
    const IPHdr *outer_iph;     /* if IP-in-IP, this will be the outer IP header */
    const TCPHdr *tcph, *orig_tcph;
    const UDPHdr *udph, *orig_udph;
    const UDPHdr *inner_udph;   /* if Teredo + UDP, this will be the inner UDP header */
    const UDPHdr *outer_udph;   /* if Teredo + UDP, this will be the outer UDP header */
    const ICMPHdr *icmph, *orig_icmph;

    const uint8_t *data;        /* packet payload pointer */
    const uint8_t *ip_data;     /* IP payload pointer */
    const uint8_t *outer_ip_data;  /* Outer IP payload pointer */
    //^^^-----------------------------

    void *ssnptr;               /* for tcp session tracking info... */
    void *fragtracker;          /* for ip fragmentation tracking info... */

    //vvv-----------------------------
    IP4Hdr *ip4h, *orig_ip4h;
    IP6Hdr *ip6h, *orig_ip6h;
    ICMP6Hdr *icmp6h, *orig_icmp6h;

    IPH_API* iph_api;
    IPH_API* orig_iph_api;
    IPH_API* outer_iph_api;
    IPH_API* outer_orig_iph_api;

    int family;
    int orig_family;
    int outer_family;
    //^^^-----------------------------

    uint32_t preprocessor_bits; /* flags for preprocessors to check */
    uint32_t preproc_reassembly_pkt_bits;

    uint32_t packet_flags;      /* special flags for the packet */

    uint32_t xtradata_mask;

    uint16_t proto_bits;

    //vvv-----------------------------
    uint16_t dsize;             /* packet payload size */
    uint16_t ip_dsize;          /* IP payload size */
    uint16_t alt_dsize;         /* the dsize of a packet before munging (used for log)*/
    uint16_t actual_ip_len;     /* for logging truncated pkts (usually by small snaplen)*/
    uint16_t outer_ip_dsize;    /* Outer IP payload size */
    //^^^-----------------------------

    uint16_t frag_offset;       /* fragment offset number */
    uint16_t ip_frag_len;
    uint16_t ip_options_len;
    uint16_t tcp_options_len;

    //vvv-----------------------------
    uint16_t sp;                /* source port (TCP/UDP) */
    uint16_t dp;                /* dest port (TCP/UDP) */
    uint16_t orig_sp;           /* source port (TCP/UDP) of original datagram */
    uint16_t orig_dp;           /* dest port (TCP/UDP) of original datagram */
    //^^^-----------------------------
    // and so on ...

    int16_t application_protocol_ordinal;

    uint8_t frag_flag;          /* flag to indicate a fragmented packet */
    uint8_t mf;                 /* more fragments flag */
    uint8_t df;                 /* don't fragment flag */
    uint8_t rf;                 /* IP reserved bit */

    uint8_t ip_option_count;    /* number of options in this packet */
    uint8_t tcp_option_count;
    uint8_t ip6_extension_count;
    uint8_t ip6_frag_index;

    uint8_t error_flags;        /* flags indicate checksum errors, bad TTLs, etc. */
    uint8_t encapsulated;
    uint8_t GTPencapsulated;
    uint8_t next_layer;         /* index into layers for next encap */

#ifndef NO_NON_ETHER_DECODER
    const Fddi_hdr *fddihdr;    /* FDDI support headers */
    Fddi_llc_saps *fddisaps;
    Fddi_llc_sna *fddisna;
    Fddi_llc_iparp *fddiiparp;
    Fddi_llc_other *fddiother;

    const Trh_hdr *trh;         /* Token Ring support headers */
    Trh_llc *trhllc;
    Trh_mr *trhmr;

    Pflog1Hdr *pf1h;            /* OpenBSD pflog interface header - version 1 */
    Pflog2Hdr *pf2h;            /* OpenBSD pflog interface header - version 2 */
    Pflog3Hdr *pf3h;            /* OpenBSD pflog interface header - version 3 */
    Pflog4Hdr *pf4h;            /* OpenBSD pflog interface header - version 4 */

    const EtherEapol *eplh;     /* 802.1x EAPOL header */
    const EAPHdr *eaph;
    const uint8_t *eaptype;
    EapolKey *eapolk;
#endif

    // nothing after this point is zeroed ...
    Options ip_options[IP_OPTMAX];         /* ip options decode structure */
    Options tcp_options[TCP_OPTLENMAX];    /* tcp options decode struct */
    IP6Option ip6_extensions[IP6_EXTMAX];  /* IPv6 Extension References */

    const uint8_t *ip_frag_start;
    const uint8_t *ip_options_data;
    const uint8_t *tcp_options_data;

    const IP6RawHdr* raw_ip6h;  // innermost raw ip6 header
    Layer layers[LAYER_MAX];    /* decoded encapsulations */

    IP4Hdr inner_ip4h, inner_orig_ip4h;
    IP6Hdr inner_ip6h, inner_orig_ip6h;
    IP4Hdr outer_ip4h, outer_orig_ip4h;
    IP6Hdr outer_ip6h, outer_orig_ip6h;

    MplsHdr mplsHdr;

    PseudoPacketType pseudo_type;    // valid only when PKT_PSEUDO is set
    uint16_t max_dsize;

    /**policyId provided in configuration file. Used for correlating configuration
     * with event output
     */
    uint16_t configPolicyId;

    uint32_t iplist_id;
    unsigned char iprep_layer;

    uint8_t ps_proto;  // Used for portscan and unified2 logging

} Packet;
#endif

#define PKT_ZERO_LEN offsetof(Packet, ip_options)

#define PROTO_BIT__NONE     0x0000
#define PROTO_BIT__IP       0x0001
#define PROTO_BIT__ARP      0x0002
#define PROTO_BIT__TCP      0x0004
#define PROTO_BIT__UDP      0x0008
#define PROTO_BIT__ICMP     0x0010
#define PROTO_BIT__TEREDO   0x0020
#define PROTO_BIT__GTP      0x0040
#define PROTO_BIT__OTHER    0x8000
#define PROTO_BIT__ALL      0xffff

#define IsIP(p) (IPH_IS_VALID(p))
#define IsTCP(p) (IsIP(p) && p->tcph)
#define IsUDP(p) (IsIP(p) && p->udph)
#define IsICMP(p) (IsIP(p) && p->icmph)
#define GET_PKT_SEQ(p) (ntohl(p->tcph->th_seq))

/* Macros to deal with sequence numbers - p810 TCP Illustrated vol 2 */
#define SEQ_LT(a,b)  ((int)((a) - (b)) <  0)
#define SEQ_LEQ(a,b) ((int)((a) - (b)) <= 0)
#define SEQ_GT(a,b)  ((int)((a) - (b)) >  0)
#define SEQ_GEQ(a,b) ((int)((a) - (b)) >= 0)
#define SEQ_EQ(a,b)  ((int)((a) - (b)) == 0)

#define BIT(i) (0x1 << (i-1))

typedef struct s_pseudoheader
{
    uint32_t sip, dip;
    uint8_t  zero;
    uint8_t  protocol;
    uint16_t len;

} PSEUDO_HDR;

/* Default classification for decoder alerts */
#define DECODE_CLASS 25

typedef struct _DecoderFlags
{
    char decode_alerts;   /* if decode.c alerts are going to be enabled */
    char oversized_alert;   /* alert if garbage after tcp/udp payload */
    char oversized_drop;   /* alert if garbage after tcp/udp payload */
    char drop_alerts;     /* drop alerts from decoder */
    char tcpopt_experiment;  /* TcpOptions Decoder */
    char drop_tcpopt_experiment; /* Drop alerts from TcpOptions Decoder */
    char tcpopt_obsolete;    /* Alert on obsolete TCP options */
    char drop_tcpopt_obsolete; /* Drop on alerts from obsolete TCP options */
    char tcpopt_ttcp;        /* Alert on T/TCP options */
    char drop_tcpopt_ttcp;   /* Drop on alerts from T/TCP options */
    char tcpopt_decode;      /* alert on decoder inconsistencies */
    char drop_tcpopt_decode; /* Drop on alerts from decoder inconsistencies */
    char ipopt_decode;      /* alert on decoder inconsistencies */
    char drop_ipopt_decode; /* Drop on alerts from decoder inconsistencies */

    /* To be moved to the frag preprocessor once it supports IPv6 */
    char ipv6_bad_frag_pkt;
    char bsd_icmp_frag;
    char drop_bad_ipv6_frag;

} DecoderFlags;

#define        ALERTMSG_LENGTH 256

// chained decoders
#if 0
void DecodeARP(const uint8_t *, uint32_t, Packet *);
void DecodeEthLoopback(const uint8_t *, uint32_t, Packet *);
void DecodeVlan(const uint8_t *, const uint32_t, Packet *);
void DecodePppPktEncapsulated(const uint8_t *, const uint32_t, Packet *);
void DecodePPPoEPkt(const uint8_t *, const uint32_t, Packet *);
void DecodeIP(const uint8_t *, const uint32_t, Packet *);
void DecodeIPV6(const uint8_t *, uint32_t, Packet *);
void DecodeTCP(const uint8_t *, const uint32_t, Packet *);
void DecodeUDP(const uint8_t *, const uint32_t, Packet *);
void DecodeICMP(const uint8_t *, const uint32_t, Packet *);
void DecodeICMP6(const uint8_t *, const uint32_t, Packet *);
void DecodeICMPEmbeddedIP(const uint8_t *, const uint32_t, Packet *);
void DecodeICMPEmbeddedIP6(const uint8_t *, const uint32_t, Packet *);
void DecodeIPOptions(const uint8_t *, uint32_t, Packet *);
void DecodeTCPOptions(const uint8_t *, uint32_t, Packet *);
void DecodeTeredo(const uint8_t *, uint32_t, Packet *);
void DecodeAH(const uint8_t *, uint32_t, Packet *);
void DecodeESP(const uint8_t *, uint32_t, Packet *);
void DecodeGTP(const uint8_t *, uint32_t, Packet *);

void DecoderAlertEncapsulated(Packet *, int, const char *, const uint8_t *, uint32_t);

#ifndef NO_NON_ETHER_DECODER
void DecodeTRPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeFDDIPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeLinuxSLLPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeIEEE80211Pkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeSlipPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeI4LRawIPPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeI4LCiscoIPPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeChdlcPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodePflog(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeOldPflog(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodePppPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodePppSerialPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);
void DecodeEncPkt(Packet *, const DAQ_PktHdr_t*, const uint8_t *);

// chained decoders
void DecodeEAP(const uint8_t *, const uint32_t, Packet *);
void DecodeEapol(const uint8_t *, uint32_t, Packet *);
void DecodeEapolKey(const uint8_t *, uint32_t, Packet *);
void DecodeIPX(const uint8_t *, uint32_t, Packet *);
#endif  // NO_NON_ETHER_DECODER
#endif

void BsdFragHashInit(int max);
void BsdFragHashCleanup(void);
void BsdFragHashReset(void);

#if defined(WORDS_MUSTALIGN) && !defined(__GNUC__)
uint32_t EXTRACT_32BITS (u_char *);
#endif /* WORDS_MUSTALIGN && !__GNUC__ */

extern void UpdateDecodeRulesArray(uint32_t sid, int bOn, int bAll);

/*Decode functions that need to be called once the policies are set */
extern void DecodePolicySpecific(Packet *);

/* XXX not sure where this guy needs to live at the moment */
typedef struct _PortList
{
    int ports[32];   /* 32 is kind of arbitrary */

    int num_entries;

} PortList;

void InitSynToMulticastDstIp( struct _SnortConfig * );
void SynToMulticastDstIpDestroy( void );

#define SFTARGET_UNKNOWN_PROTOCOL -1

static inline int PacketWasCooked(Packet* p)
{
    return ( p->packet_flags & PKT_PSEUDO ) != 0;
}

static inline bool IsPortscanPacket(const Packet *p)
{
    return ((p->packet_flags & PKT_PSEUDO) && (p->pseudo_type == PSEUDO_PKT_PS));
}

static inline uint8_t GetEventProto(const Packet *p)
{
    if (IsPortscanPacket(p))
        return p->ps_proto;
    return IPH_IS_VALID(p) ? GET_IPH_PROTO(p) : 0;
}

static inline bool PacketHasFullPDU (const Packet* p)
{
    return ( (p->packet_flags & PKT_PDU_FULL) == PKT_PDU_FULL );
}

static inline bool PacketHasStartOfPDU (const Packet* p)
{
    return ( (p->packet_flags & PKT_PDU_HEAD) != 0 );
}

static inline bool PacketHasPAFPayload (const Packet* p)
{
    return ( (p->packet_flags & PKT_REBUILT_STREAM) || PacketHasFullPDU(p) );
}

static inline bool PacketIsRebuilt (const Packet* p)
{
    return ( (p->packet_flags & (PKT_REBUILT_STREAM|PKT_REBUILT_FRAG)) != 0 );
}

static inline void SetExtraData (Packet* p, uint32_t xid)
{
    p->xtradata_mask |= BIT(xid);
}

#endif  /* __DECODE_H__ */



typedef struct _UDPHdr
{
    u_int16_t uh_sport;
    u_int16_t uh_dport;
    u_int16_t uh_len;
    u_int16_t uh_chk;
}UDPHdr;

typedef struct _Packet
{
    const struct pcap_pkthdr *pkth;   /* BPF data */
    const u_int8_t *pkt;              /* base pointer to the raw packet data */

    Trh_hdr *trh;               /* Token Ring support headers */
    Trh_llc *trhllc;
    Trh_mr *trhmr;

    const EtherHdr *eh;               /* standard TCP/IP/Ethernet/ARP headers */
    VlanTagHdr *vh;

    IPHdr *iph, *orig_iph;   /* and orig. headers for ICMP_*_UNREACH family */
    u_int32_t ip_options_len;
    u_int8_t *ip_options_data;

    TCPHdr *tcph, *orig_tcph;
	IP6Hdr *ip6h, *orig_ip6h;
    u_int32_t tcp_options_len;
    u_int8_t *tcp_options_data;

    //UDPHdr *udph, *orig_udph;
    //ICMPHdr *icmph, *orig_icmph;

    //echoext *ext;           /* ICMP echo extension struct */

    const u_int8_t *data;         /* packet payload pointer */
    u_int16_t dsize;        /* packet payload size */
    u_int16_t alt_dsize;    /* the dsize of a packet before munging
                            (used for log)*/

    u_int8_t frag_flag;     /* flag to indicate a fragmented packet */
    u_int16_t frag_offset;  /* fragment offset number */
    u_int8_t mf;            /* more fragments flag */
    u_int8_t df;            /* don't fragment flag */
    u_int8_t rf;                  /* IP reserved bit */

    u_int16_t sp;           /* source port (TCP/UDP) */
    u_int16_t dp;           /* dest port (TCP/UDP) */
    u_int16_t orig_sp;      /* source port (TCP/UDP) of original datagram */
    u_int16_t orig_dp;      /* dest port (TCP/UDP) of original datagram */
    u_int32_t caplen;

    u_int8_t uri_count;     /* number of URIs in this packet */

	int family;
	int orig_family;
	int outer_family;
    void *ssnptr;           /* for tcp session tracking info... */
    void *flow;             /* for flow info */
    void *streamptr;        /* for tcp pkt dump */
    
    u_int32_t ip_option_count;  /* number of options in this packet */
    u_char ip_lastopt_bad;  /* flag to indicate that option decoding was
                               halted due to a bad option */
    u_int32_t tcp_option_count;
    u_char tcp_lastopt_bad;  /* flag to indicate that option decoding was
                                halted due to a bad option */

    u_int8_t csum_flags;        /* checksum flags */
    u_int32_t packet_flags;     /* special flags for the packet */
    int preprocessors;          /* flags for preprocessors to check */
}Packet;

#endif
