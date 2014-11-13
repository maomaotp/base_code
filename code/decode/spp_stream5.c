/* $Id$ */

/*  I N C L U D E S  ************************************************/

#include <assert.h>
#include <stdio.h>
#include <sys/time.h>       /* struct timeval */
#include <sys/types.h>      /* u_int*_t */

#include "decode.h"
#include "spp_stream5.h"
#include "snort_stream5_session.h"
#include "stream5_common.h"
#include "stream_api.h"

extern Stream5SessionCache *tcp_lws_cache;
//extern Stream5SessionCache *udp_lws_cache;
//extern Stream5SessionCache *icmp_lws_cache;
//extern Stream5SessionCache *ip_lws_cache;

//extern FlushConfig ignore_flush_policy[MAX_PORTS];

/*  M A C R O S  **************************************************/

/* default limits */
#define S5_DEFAULT_PRUNE_QUANTA  30       /* seconds to timeout a session */
#define S5_DEFAULT_MEMCAP        8388608  /* 8MB */
#define S5_DEFAULT_PRUNE_LOG_MAX 1048576  /* 1MB */
#define S5_RIDICULOUS_HI_MEMCAP  1024*1024*1024 /* 1GB */
#define S5_RIDICULOUS_LOW_MEMCAP 32768    /* 32k*/
#define S5_RIDICULOUS_MAX_SESSIONS 1024*1024 /* 1 million sessions */
#define S5_DEFAULT_MAX_TCP_SESSIONS 262144 /* 256k TCP sessions by default */
#define S5_DEFAULT_MAX_UDP_SESSIONS 131072 /* 128k UDP sessions by default */
#define S5_DEFAULT_MAX_ICMP_SESSIONS 65536 /* 64k ICMP sessions by default */
#define S5_DEFAULT_MAX_IP_SESSIONS   16384 /* 16k IP sessions by default */
#define S5_DEFAULT_TCP_CACHE_PRUNING_TIMEOUT    30          /* 30 seconds */
#define S5_DEFAULT_TCP_CACHE_NOMINAL_TIMEOUT    (60 * 60)   /* 1 hour */
#define S5_DEFAULT_UDP_CACHE_PRUNING_TIMEOUT    30          /* 30 seconds */
#define S5_DEFAULT_UDP_CACHE_NOMINAL_TIMEOUT    (3 * 60)    /* 3 minutes */
#define S5_MAX_CACHE_TIMEOUT                    (12 * 60 * 60)  /* 12 hours */
#define S5_MIN_PRUNE_LOG_MAX     1024      /* 1k packet data stored */
#define S5_MAX_PRUNE_LOG_MAX     S5_RIDICULOUS_HI_MEMCAP  /* 1GB packet data stored */

uint32_t mem_in_use = 0;

uint32_t firstPacketTime = 0;
Stream5Stats s5stats;
MemPool s5FlowMempool;
static PoolCount s_tcp_sessions = 0, s_udp_sessions = 0;
static PoolCount s_icmp_sessions = 0, s_ip_sessions = 0;
static int s_proto_flags = 0;

uint32_t xtradata_func_count = 0;
LogFunction xtradata_map[LOG_FUNC_MAX];
LogExtraData extra_data_log = NULL;
void *extra_data_config = NULL;


static void Stream5GlobalInit(struct _SnortConfig *, char *);
static void Stream5ParseGlobalArgs(Stream5GlobalConfig *, char *);
static void Stream5PolicyInitTcp(struct _SnortConfig *, char *);
static void Stream5PolicyInitUdp(struct _SnortConfig *, char *);
static void Stream5PolicyInitIcmp(struct _SnortConfig *, char *);
static void Stream5PolicyInitIp(struct _SnortConfig *, char *);
static void Stream5CleanExit(int, void *);
static void Stream5Reset(int, void *);
static void Stream5ResetStats(int, void *);
static int Stream5VerifyConfig(struct _SnortConfig *);
static void Stream5PrintGlobalConfig(Stream5GlobalConfig *);
static void Stream5PrintStats(int);
static void Stream5Process(Packet *p, void *context);
static inline int IsEligible(Packet *p);

#if 0

tSfPolicyUserContextId s5_config = NULL;
Stream5GlobalConfig *s5_global_eval_config = NULL;
Stream5TcpConfig *s5_tcp_eval_config = NULL;
Stream5UdpConfig *s5_udp_eval_config = NULL;
Stream5IcmpConfig *s5_icmp_eval_config = NULL;
Stream5IpConfig *s5_ip_eval_config = NULL;

/*  S T R E A M  A P I **********************************************/
static int Stream5MidStreamDropAlert(void)
{
    Stream5Config *config = sfPolicyUserDataGet(s5_config, getRuntimePolicy());

    if (config == NULL)
        return 1;

    return (config->global_config->flags &
            STREAM5_CONFIG_MIDSTREAM_DROP_NOALERT) ? 0 : 1;
}

static void Stream5UpdateDirection(
                    void * ssnptr,
                    char dir,
                    snort_ip_p ip,
                    uint16_t port);
static uint32_t Stream5GetPacketDirection(
                    Packet *p);
static void Stream5StopInspection(
                    void * ssnptr,
                    Packet *p,
                    char dir,
                    int32_t bytes,
                    int response);
static int Stream5IgnoreChannel(
                    snort_ip_p srcIP,
                    uint16_t srcPort,
                    snort_ip_p dstIP,
                    uint16_t dstPort,
                    uint8_t protocol,
                    time_t now,
                    uint32_t preprocId,
                    char direction,
                    char flags);
static int Stream5GetIgnoreDirection(
                    void *ssnptr);
static int Stream5SetIgnoreDirection(
                    void *ssnptr, int);
static void Stream5ResumeInspection(
                    void *ssnptr,
                    char dir);
static void Stream5DropTraffic(
                    Packet*,
                    void *ssnptr,
                    char dir);
static void Stream5DropPacket(
                    Packet *p);
static int Stream5SetApplicationData(
                    void *ssnptr,
                    uint32_t protocol,
                    void *data,
                    StreamAppDataFree free_func);
static void *Stream5GetApplicationData(
                    void *ssnptr,
                    uint32_t protocol);
static StreamSessionKey * Stream5GetSessionKey(
                    Packet *p);
static void Stream5PopulateSessionKey(
                    Packet *p,
                    StreamSessionKey *key);
static void * Stream5GetApplicationDataFromSessionKey(
                    const StreamSessionKey *key,
                    uint32_t protocol);
static void *Stream5GetApplicationDataFromIpPort(
                    snort_ip_p srcIP,
                    uint16_t srcPort,
                    snort_ip_p dstIP,
                    uint16_t dstPort,
                    char ip_protocol,
                    uint16_t vlan,
                    uint32_t mplsId,
                    uint16_t addressSpaceId,
                    uint32_t protocol);
static uint32_t Stream5SetSessionFlags(
                    void *ssnptr,
                    uint32_t flags);
static uint32_t Stream5GetSessionFlags(void *ssnptr);
static int Stream5AlertFlushStream(Packet *p);
static int Stream5ResponseFlushStream(Packet *p);
static int Stream5AddSessionAlert(void *ssnptr,
                                  Packet *p,
                                  uint32_t gid,
                                  uint32_t sid);
static int Stream5CheckSessionAlert(void *ssnptr,
                                    Packet *p,
                                    uint32_t gid,
                                    uint32_t sid);
static int Stream5UpdateSessionAlert(void *ssnptr,
                                    Packet *p,
                                    uint32_t gid,
                                    uint32_t sid,
                                    uint32_t event_id,
                                    uint32_t event_second);
static char Stream5SetReassembly(void *ssnptr,
                                    uint8_t flush_policy,
                                    char dir,
                                    char flags);
static char Stream5GetReassemblyDirection(void *ssnptr);
static char Stream5GetReassemblyFlushPolicy(void *ssnptr, char dir);
static char Stream5IsStreamSequenced(void *ssnptr, char dir);
static int Stream5MissingInReassembled(void *ssnptr, char dir);
static char Stream5PacketsMissing(void *ssnptr, char dir);

static int Stream5GetRebuiltPackets(
        Packet *p,
        PacketIterator callback,
        void *userdata);
static int Stream5GetStreamSegments(
        Packet *p,
        StreamSegmentIterator callback,
        void *userdata);

static StreamFlowData *Stream5GetFlowData(Packet *p);
static int Stream5SetApplicationProtocolIdExpected(
                    snort_ip_p srcIP,
                    uint16_t srcPort,
                    snort_ip_p dstIP,
                    uint16_t dstPort,
                    uint8_t protocol,
                    time_t now,
                    int16_t protoId,
                    uint32_t preprocId,
                    void* protoData,
                    void (*protoDataFreeFn)(void*));

static uint16_t s5GetPreprocessorStatusBit(void);

static void s5SetPortFilterStatus(
        struct _SnortConfig *sc,
        int protocol,
        uint16_t port,
        uint16_t status,
        tSfPolicyId policyId,
        int parsing
        );

static void s5UnsetPortFilterStatus(
        struct _SnortConfig *sc,
        int protocol,
        uint16_t port,
        uint16_t status,
        tSfPolicyId policyId,
        int parsing
        );

static void s5GetMaxSessions(struct _SnortConfig *sc, tSfPolicyId policyId, StreamSessionLimits* limits);

static void *Stream5GetSessionPtrFromIpPort(
                    snort_ip_p srcIP,
                    uint16_t srcPort,
                    snort_ip_p dstIP,
                    uint16_t dstPort,
                    char ip_protocol,
                    uint16_t vlan,
                    uint32_t mplsId,
                    uint16_t addressSpaceId);

static const StreamSessionKey *Stream5GetKeyFromSessionPtr(const void *ssnptr);

static uint8_t s5GetHopLimit (void* ssnptr, char dir, int outer);

static uint32_t Stream5GetFlushPoint(void *ssnptr, char dir);
static void Stream5SetFlushPoint(void *ssnptr, char dir, uint32_t flush_point);
static bool Stream5RegisterPAFPort(
    struct _SnortConfig *, tSfPolicyId,
    uint16_t server_port, bool toServer,
    PAF_Callback, bool autoEnable);
static bool Stream5RegisterPAFService(
    struct _SnortConfig *, tSfPolicyId,
    uint16_t service, bool toServer,
    PAF_Callback, bool autoEnable);
static void** Stream5GetPAFUserData(void* ssnptr, bool to_server);
static bool Stream5IsPafActive(void* ssnptr, bool to_server);
static bool Stream5ActivatePaf(void* ssnptr, bool to_server);


static uint32_t Stream5RegisterXtraData(LogFunction );
static uint32_t Stream5GetXtraDataMap(LogFunction **);
static void Stream5RegisterXtraDataLog(LogExtraData, void * );

static void Stream5CheckSessionClosed(Packet*);

static void Stream5SetExtraData(void* ssn, Packet*, uint32_t);
static void Stream5ClearExtraData(void* ssn, Packet*, uint32_t);

static void Stream5ForceSessionExpiration(void *ssnptr);
static unsigned Stream5RegisterHandler(Stream_Callback);
static bool Stream5SetHandler(void* ssnptr, unsigned id, Stream_Event);
#endif

#if 0
static void Stream5PolicyInitTcp(struct _SnortConfig *sc, char *args)
{
    tSfPolicyId policy_id = getParserPolicy(sc);
    Stream5Config *config = NULL;

    if (s5_config == NULL)
        FatalError("Tried to config stream5 TCP policy without global config!\n");

    sfPolicyUserPolicySet (s5_config, policy_id);
    config = (Stream5Config *)sfPolicyUserDataGetCurrent(s5_config);

    if ((config == NULL) || (config->global_config == NULL))
    {
        FatalError("Tried to config stream5 TCP policy without global config!\n");
    }

    if (!config->global_config->track_tcp_sessions)
    {
        return;
    }

    if (config->tcp_config == NULL)
    {
        config->tcp_config =
            (Stream5TcpConfig *)SnortAlloc(sizeof(Stream5TcpConfig));

        Stream5InitTcp(config->global_config);
        Stream5TcpInitFlushPoints();
        Stream5TcpRegisterRuleOptions(sc);
        AddFuncToPreprocPostConfigList(sc, Stream5PostConfigTcp, config->tcp_config);
    }

    /* Call the protocol specific initializer */
    Stream5TcpPolicyInit(sc, config->tcp_config, args);
}

static void Stream5PolicyInitUdp(struct _SnortConfig *sc, char *args)
{
    tSfPolicyId policy_id = getParserPolicy(sc);
    Stream5Config *config;

    if (s5_config == NULL)
        FatalError("Tried to config stream5 UDP policy without global config!\n");

    sfPolicyUserPolicySet (s5_config, policy_id);
    config = (Stream5Config *)sfPolicyUserDataGetCurrent(s5_config);

    if ((config == NULL) || (config->global_config == NULL))
    {
        FatalError("Tried to config stream5 UDP policy without global config!\n");
    }

    if (!config->global_config->track_udp_sessions)
    {
        return;
    }

    if (config->udp_config == NULL)
    {
        config->udp_config =
            (Stream5UdpConfig *)SnortAlloc(sizeof(Stream5UdpConfig));

        Stream5InitUdp(config->global_config);
    }

    /* Call the protocol specific initializer */
    Stream5UdpPolicyInit(config->udp_config, args);
}

static void Stream5PolicyInitIcmp(struct _SnortConfig *sc, char *args)
{
    tSfPolicyId policy_id = getParserPolicy(sc);
    Stream5Config *config;

    if (s5_config == NULL)
        FatalError("Tried to config stream5 ICMP policy without global config!\n");

    sfPolicyUserPolicySet (s5_config, policy_id);
    config = (Stream5Config *)sfPolicyUserDataGetCurrent(s5_config);

    if ((config == NULL) || (config->global_config == NULL))
    {
        FatalError("Tried to config stream5 ICMP policy without global config!\n");
    }

    if (!config->global_config->track_icmp_sessions)
    {
        return;
    }

    if (config->icmp_config == NULL)
    {
        config->icmp_config =
            (Stream5IcmpConfig *)SnortAlloc(sizeof(Stream5IcmpConfig));

        Stream5InitIcmp(config->global_config);
    }

    /* Call the protocol specific initializer */
    Stream5IcmpPolicyInit(config->icmp_config, args);
}

static void Stream5PolicyInitIp(struct _SnortConfig *sc, char *args)
{
    tSfPolicyId policy_id = getParserPolicy(sc);
    Stream5Config *config;

    if (s5_config == NULL)
        FatalError("Tried to config stream5 IP policy without global config!\n");

    sfPolicyUserPolicySet (s5_config, policy_id);
    config = (Stream5Config *)sfPolicyUserDataGetCurrent(s5_config);

    if ((config == NULL) || (config->global_config == NULL))
    {
        FatalError("Tried to config stream5 IP policy without global config!\n");
    }

    if (!config->global_config->track_ip_sessions)
    {
        return;
    }

    if (config->ip_config == NULL)
    {
        config->ip_config =
            (Stream5IpConfig*)SnortAlloc(sizeof(*config->ip_config));

        Stream5InitIp(config->global_config);
    }

    /* Call the protocol specific initializer */
    Stream5IpPolicyInit(config->ip_config, args);
}

static void Stream5Reset(int signal, void *foo)
{
    if (s5_config == NULL)
        return;

    Stream5ResetTcp();
    Stream5ResetUdp();
    Stream5ResetIcmp();
    Stream5ResetIp();

    mempool_clean(&s5FlowMempool);
}

static void Stream5ResetStats(int signal, void *foo)
{
    memset(&s5stats, 0, sizeof(s5stats));
    Stream5ResetTcpPrunes();
    Stream5ResetUdpPrunes();
    Stream5ResetIcmpPrunes();
    Stream5ResetIpPrunes();
}

static void Stream5CleanExit(int signal, void *foo)
{
    s5stats.tcp_prunes = Stream5GetTcpPrunes();
    s5stats.udp_prunes = Stream5GetUdpPrunes();
    s5stats.icmp_prunes = Stream5GetIcmpPrunes();
    s5stats.ip_prunes = Stream5GetIpPrunes();

    /* Clean up the hash tables for these */
    Stream5CleanTcp();
    Stream5CleanUdp();
    Stream5CleanIcmp();
    Stream5CleanIp();

    mempool_destroy(&s5FlowMempool);

    /* Free up the ignore data that was queued */
    StreamExpectCleanup();

    Stream5FreeConfigs(s5_config);
    s5_config = NULL;
}

static int Stream5VerifyConfigPolicy(
        struct _SnortConfig *sc,
        tSfPolicyUserContextId config,
        tSfPolicyId policyId,
        void* pData
        )
{
    Stream5Config *pPolicyConfig = (Stream5Config *)pData;
    tSfPolicyId tmp_policy_id = getParserPolicy(sc);

    int tcpNotConfigured = 0;
    int udpNotConfigured = 0;
    int icmpNotConfigured = 0;
    int ipNotConfigured = 0;
    int proto_flags = 0;

    //do any housekeeping before freeing Stream5Config
    if ( pPolicyConfig->global_config == NULL )
    {
        WarningMessage("%s(%d) Stream5 global config is NULL.\n",
                __FILE__, __LINE__);
        return -1;
    }

    if ( pPolicyConfig->global_config->disabled )
        return 0;

    if (pPolicyConfig->global_config->track_tcp_sessions)
    {
        tcpNotConfigured =
            !pPolicyConfig->global_config->max_tcp_sessions ||
            Stream5VerifyTcpConfig(sc, pPolicyConfig->tcp_config, policyId);

        if (tcpNotConfigured)
        {
            ErrorMessage(
                "WARNING: Stream5 TCP misconfigured (policy %u).\n", policyId);
        }
        else
        {
            if ( !(s_proto_flags & PROTO_BIT__TCP) )
                s_tcp_sessions += pPolicyConfig->global_config->max_tcp_sessions;

            proto_flags |= PROTO_BIT__TCP;
        }
    }

    if (pPolicyConfig->global_config->track_udp_sessions)
    {
        udpNotConfigured =
            !pPolicyConfig->global_config->max_udp_sessions ||
            Stream5VerifyUdpConfig(sc, pPolicyConfig->udp_config, policyId);

        if (udpNotConfigured)
        {
            ErrorMessage(
                "WARNING: Stream5 UDP misconfigured (policy %u).\n", policyId);
        }
        else
        {
            if ( !(s_proto_flags & PROTO_BIT__UDP) )
                s_udp_sessions += pPolicyConfig->global_config->max_udp_sessions;

            proto_flags |= PROTO_BIT__UDP;
        }
    }

    if (pPolicyConfig->global_config->track_icmp_sessions)
    {
        icmpNotConfigured =
            !pPolicyConfig->global_config->max_icmp_sessions ||
            Stream5VerifyIcmpConfig(pPolicyConfig->icmp_config, policyId);

        if (icmpNotConfigured)
        {
            ErrorMessage(
                "WARNING: Stream5 ICMP misconfigured (policy %u).\n", policyId);
        }
        else
        {
            if ( !(s_proto_flags & PROTO_BIT__ICMP) )
                s_icmp_sessions += pPolicyConfig->global_config->max_icmp_sessions;

            proto_flags |= PROTO_BIT__ICMP;
        }
    }

    if (pPolicyConfig->global_config->track_ip_sessions)
    {
        ipNotConfigured =
            !pPolicyConfig->global_config->max_ip_sessions ||
            Stream5VerifyIpConfig(pPolicyConfig->ip_config, policyId);

        if (ipNotConfigured)
        {
            ErrorMessage(
                "WARNING: Stream5 IP misconfigured (policy %u).\n", policyId);
        }
        else
        {
            if ( !(s_proto_flags & PROTO_BIT__IP) )
                s_ip_sessions += pPolicyConfig->global_config->max_ip_sessions;

            proto_flags |= PROTO_BIT__IP;
        }
    }

    if ( tcpNotConfigured || udpNotConfigured || icmpNotConfigured || ipNotConfigured
        )
    {
        ErrorMessage("ERROR: Stream5 not properly configured... exiting\n");
        return -1;
    }

    setParserPolicy(sc, policyId);
    AddFuncToPreprocList(sc, Stream5Process, PRIORITY_TRANSPORT, PP_STREAM5, proto_flags);
    setParserPolicy(sc, tmp_policy_id);

    s_proto_flags |= proto_flags;

    return 0;
}

static int Stream5VerifyConfig(struct _SnortConfig *sc)
{
    int rval;
    int obj_size = 0;
    PoolCount total_sessions = 0;
    Stream5Config* defConfig;

    if (s5_config == NULL)
        return 0;

    s_tcp_sessions = s_udp_sessions = 0;
    s_icmp_sessions = s_ip_sessions = 0;

    if ((rval = sfPolicyUserDataIterate (sc, s5_config, Stream5VerifyConfigPolicy)))
        return rval;

    defConfig = sfPolicyUserDataGet(s5_config, getDefaultPolicy());

    total_sessions = s_tcp_sessions + s_udp_sessions
                   + s_icmp_sessions + s_ip_sessions;

    if ( !total_sessions )
        return 0;

    if ( (defConfig->global_config->max_tcp_sessions > 0)
        && (s_tcp_sessions == 0) )
    {
        LogMessage("TCP tracking disabled, no TCP sessions allocated\n");
    }

    if ( (defConfig->global_config->max_udp_sessions > 0)
        && (s_udp_sessions == 0) )
    {
        LogMessage("UDP tracking disabled, no UDP sessions allocated\n");
    }

    if ( (defConfig->global_config->max_icmp_sessions > 0)
        && (s_icmp_sessions == 0) )
    {
        LogMessage("ICMP tracking disabled, no ICMP sessions allocated\n");
    }

    if ( (defConfig->global_config->max_ip_sessions > 0)
        && (s_ip_sessions == 0) )
    {
        LogMessage("IP tracking disabled, no IP sessions allocated\n");
    }

    /* Initialize the memory pool for Flowbits Data */
    /* use giFlowbitSize - 1, since there is already 1 byte in the
     * StreamFlowData structure */
    obj_size = sizeof(StreamFlowData) + getFlowbitSizeInBytes() - 1;

    if (obj_size % sizeof(long) != 0)
    {
        /* Increase obj_size by sizeof(long) to force sizeof(long) byte
         * alignment for each object in the mempool.  Without this,
         * the mempool data buffer was not aligned. Overlaying the
         * StreamFlowData structure caused problems on some Solaris
         * platforms. */
        obj_size += ( sizeof(long) - (obj_size % sizeof(long)));
    }

    if (mempool_init(&s5FlowMempool, total_sessions, obj_size) != 0)
    {
        FatalError("%s(%d) Could not initialize flow bits memory pool.\n",
                   __FILE__, __LINE__);
    }

    return 0;
}

static void Stream5PrintStats(int exiting)
{
    printf("Stream5 statistics:\n");
    printf("            Total sessions: %u\n",
            s5stats.total_tcp_sessions +
            s5stats.total_udp_sessions +
            s5stats.total_icmp_sessions +
            s5stats.total_ip_sessions);
    printf("              TCP sessions: %u\n", s5stats.total_tcp_sessions);
    printf("              UDP sessions: %u\n", s5stats.total_udp_sessions);
    printf("             ICMP sessions: %u\n", s5stats.total_icmp_sessions);
    printf("               IP sessions: %u\n", s5stats.total_ip_sessions);

    printf("                TCP Prunes: %u\n", Stream5GetTcpPrunes());
    printf("                UDP Prunes: %u\n", Stream5GetUdpPrunes());
    printf("               ICMP Prunes: %u\n", Stream5GetIcmpPrunes());
    printf("                 IP Prunes: %u\n", Stream5GetIpPrunes());
    ("TCP StreamTrackers Created: %u\n",
            s5stats.tcp_streamtrackers_created);
    printfe("TCP StreamTrackers Deleted: %u\n",
            s5stats.tcp_streamtrackers_released);
    printf("              TCP Timeouts: %u\n", s5stats.tcp_timeouts);
    printf("              TCP Overlaps: %u\n", s5stats.tcp_overlaps);
    printf("       TCP Segments Queued: %u\n", s5stats.tcp_streamsegs_created);
    printf("     TCP Segments Released: %u\n", s5stats.tcp_streamsegs_released);
    printf("       TCP Rebuilt Packets: %u\n", s5stats.tcp_rebuilt_packets);
    printf("         TCP Segments Used: %u\n", s5stats.tcp_rebuilt_seqs_used);
    printf("              TCP Discards: %u\n", s5stats.tcp_discards);
    printf("                  TCP Gaps: %u\n", s5stats.tcp_gaps);
    printf("      UDP Sessions Created: %u\n",
            s5stats.udp_sessions_created);
    printf("      UDP Sessions Deleted: %u\n",
            s5stats.udp_sessions_released);
    printf("              UDP Timeouts: %u\n", s5stats.udp_timeouts);
    printf("              UDP Discards: %u\n", s5stats.udp_discards);
    printf("                    Events: %u\n", s5stats.events);
    printf("           Internal Events: %u\n", s5stats.internalEvents);
    printf("           TCP Port Filter\n");
    printf("                  Filtered: %u\n", s5stats.tcp_port_filter.filtered);
    printf("                 Inspected: %u\n", s5stats.tcp_port_filter.inspected);
    printf("                   Tracked: %u\n", s5stats.tcp_port_filter.session_tracked);
    printf("           UDP Port Filter\n");
    printf("                  Filtered: %u\n", s5stats.udp_port_filter.filtered);
    printf("                 Inspected: %u\n", s5stats.udp_port_filter.inspected);
    printf("                   Tracked: %u\n", s5stats.udp_port_filter.session_tracked);
}
#endif

/*
 * MAIN ENTRY POINT
 */
void Stream5Process(Packet *p, void *context)
{
    SessionKey key;
    Stream5LWSession *lwssn = NULL;

    if (!firstPacketTime)
        firstPacketTime = p->pkth->ts.tv_sec;

    if(!IsEligible(p))
    {
        printf("Is not eligible!\n");
        return;
    }

    printf("*************In Stream5!***************\n");

    /* Call individual TCP/UDP/ICMP/IP processing, per GET_IPH_PROTO(p) */
    switch(GET_IPH_PROTO(p)) {
        case IPPROTO_TCP:
            {
                Stream5TcpPolicy *policy = NULL;

                lwssn = GetLWSession(tcp_lws_cache, p, &key);
                if (lwssn != NULL)
                {
                    policy = (Stream5TcpPolicy *)lwssn->policy;
                }

                Stream5ProcessTcp(p, lwssn, policy, &key);
            }
            break;

        case IPPROTO_UDP:
            {
                Stream5UdpPolicy *policy = NULL;

                lwssn = GetLWSession(udp_lws_cache, p, &key);
                if (lwssn != NULL)
                {
                    policy = (Stream5UdpPolicy *)lwssn->policy;
                }

                if (Stream5SetRuntimeConfiguration(lwssn, IPPROTO_UDP) == -1)
                    return;

                Stream5ProcessUdp(p, lwssn, policy, &key);
            }
            break;

        case IPPROTO_ICMP:
            if (Stream5SetRuntimeConfiguration(NULL, IPPROTO_ICMP) != -1)
            {
                if ( s5_global_eval_config->track_icmp_sessions )
                {
                    lwssn = NULL;
                    Stream5ProcessIcmp(p);
                }
                else
                {
                    // note - this block same as below
                    lwssn = GetLWSession(ip_lws_cache, p, &key);
                    Stream5ProcessIp(p, lwssn, &key);
                }
            }
            break;

        default:
            if (Stream5SetRuntimeConfiguration(NULL, IPPROTO_IP) == -1)
                return;

            // note - this block same as above
            lwssn = GetLWSession(ip_lws_cache, p, &key);
            Stream5ProcessIp(p, lwssn, &key);
            break;
    }
}


static inline int IsEligible(Packet *p)
{
    if ((p->frag_flag) || (p->error_flags & PKT_ERR_CKSUM_IP))
        return 0;

    if (p->packet_flags & PKT_REBUILT_STREAM)
        return 0;

    if (p->family == 0)
        return 0;

    switch(GET_IPH_PROTO(p))
    {
        case IPPROTO_TCP:
        {
             if(p->tcph == NULL)
                 return 0;

             if (p->error_flags & PKT_ERR_CKSUM_TCP)
                 return 0;
        }
        break;
        case IPPROTO_UDP:
        {
             if(p->udph == NULL)
                 return 0;

             if (p->error_flags & PKT_ERR_CKSUM_UDP)
                 return 0;
        }
        break;
        case IPPROTO_ICMP:
        case IPPROTO_ICMPV6:
        {
             if(p->icmph == NULL)
                 return 0;

             if (p->error_flags & PKT_ERR_CKSUM_ICMP)
                 return 0;
        }
        break;
        default:
            if(p->iph == NULL)
                return 0;
            break;
    }

    return 1;
}

#if 0

/*************************** API Implementations *******************/
static int Stream5SetApplicationData(
                    void *ssnptr,
                    uint32_t protocol,
                    void *data,
                    StreamAppDataFree free_func)
{
    Stream5LWSession *ssn;
    Stream5AppData *appData = NULL;
    if (ssnptr)
    {
        ssn = (Stream5LWSession*)ssnptr;
        appData = ssn->appDataList;
        while (appData)
        {
            if (appData->protocol == protocol)
            {
                /* If changing the pointer to the data, free old one */
                if ((appData->freeFunc) && (appData->dataPointer != data))
                {
                    if ( appData->dataPointer )
                        appData->freeFunc(appData->dataPointer);
                }
                else
                {
                    /* Same pointer, same protocol.  Go away */
                    break;
                }

                appData->dataPointer = NULL;
                break;
            }

            appData = appData->next;
        }

        /* If there isn't one for this protocol, allocate */
        if (!appData)
        {
            appData = SnortAlloc(sizeof(Stream5AppData));

            /* And add it to the list */
            if (ssn->appDataList)
            {
                ssn->appDataList->prev = appData;
            }
            appData->next = ssn->appDataList;
            ssn->appDataList = appData;
        }

        /* This will reset free_func if it already exists */
        appData->protocol = protocol;
        appData->freeFunc = free_func;
        appData->dataPointer = data;

        return 0;
    }
    return -1;
}

static void *Stream5GetApplicationData(void *ssnptr, uint32_t protocol)
{
    Stream5LWSession *ssn;
    Stream5AppData *appData = NULL;
    void *data = NULL;
    if (ssnptr)
    {
        ssn = (Stream5LWSession*)ssnptr;
        appData = ssn->appDataList;
        while (appData)
        {
            if (appData->protocol == protocol)
            {
                data = appData->dataPointer;
                break;
            }
            appData = appData->next;
        }
    }
    return data;
}

static inline void * Stream5GetSessionPtr(const SessionKey *key)
{
    Stream5LWSession *ssn;

    switch(key->protocol)
    {
        case IPPROTO_TCP:
            ssn = GetLWSessionFromKey(tcp_lws_cache, key);
            break;
        case IPPROTO_UDP:
            ssn = GetLWSessionFromKey(udp_lws_cache, key);
            break;
        case IPPROTO_ICMP:
            ssn = GetLWSessionFromKey(icmp_lws_cache, key);
            if (ssn) break;
            /* fall through */
        default:
            ssn = GetLWSessionFromKey(ip_lws_cache, key);
            break;
    }

    return (void *)ssn;
}

static void * Stream5GetSessionPtrFromIpPort(
                    snort_ip_p srcIP,
                    uint16_t srcPort,
                    snort_ip_p dstIP,
                    uint16_t dstPort,
                    char ip_protocol,
                    uint16_t vlan,
                    uint32_t mplsId,
                    uint16_t addressSpaceId)
{
    SessionKey key;

    GetLWSessionKeyFromIpPort(srcIP, srcPort, dstIP, dstPort, ip_protocol, vlan, mplsId, addressSpaceId, &key);

    return (void*)Stream5GetSessionPtr(&key);
}

static const StreamSessionKey *Stream5GetKeyFromSessionPtr(const void *ssnptr)
{
    const Stream5LWSession *ssn = (const Stream5LWSession*)ssnptr;
    return ssn->key;
}

static void Stream5PopulateSessionKey(Packet *p, StreamSessionKey *key)
{
    uint16_t addressSpaceId = 0;

    if (!key || !p)
        return;

    GetLWSessionKeyFromIpPort(
        GET_SRC_IP(p), p->sp,
        GET_DST_IP(p), p->dp,
        GET_IPH_PROTO(p),
        p->vh ? VTH_VLAN(p->vh) : 0,
        p->mpls ? p->mplsHdr.label : 0,
        addressSpaceId, key);
}

static StreamSessionKey * Stream5GetSessionKey(Packet *p)
{
    SessionKey *key = calloc(1, sizeof(*key));

    if (!key)
        return NULL;

    Stream5PopulateSessionKey(p, key);

    return key;
}

static void * Stream5GetApplicationDataFromSessionKey(const StreamSessionKey *key, uint32_t protocol)
{
    Stream5LWSession *ssn = Stream5GetSessionPtr(key);
    return Stream5GetApplicationData(ssn, protocol);
}

static void * Stream5GetApplicationDataFromIpPort(
                    snort_ip_p srcIP,
                    uint16_t srcPort,
                    snort_ip_p dstIP,
                    uint16_t dstPort,
                    char ip_protocol,
                    uint16_t vlan,
                    uint32_t mplsId,
                    uint16_t addressSpaceID,
                    uint32_t protocol)
{
    Stream5LWSession *ssn;

    ssn = (Stream5LWSession *) Stream5GetSessionPtrFromIpPort(srcIP,srcPort,dstIP,dstPort,
            ip_protocol,vlan,mplsId, addressSpaceID);

    return Stream5GetApplicationData(ssn, protocol);
}

static void Stream5CheckSessionClosed(Packet* p)
{
    Stream5LWSession* ssn;

    if (!p || !p->ssnptr)
        return;

    ssn = (Stream5LWSession*)p->ssnptr;

    if (ssn->session_state & STREAM5_STATE_CLOSED)
    {
        switch (ssn->protocol)
        {
        case IPPROTO_TCP:
            DeleteLWSession(tcp_lws_cache, ssn, "closed normally");
            p->ssnptr = NULL;
            break;
        case IPPROTO_UDP:
            DeleteLWSession(udp_lws_cache, ssn, "closed normally");
            p->ssnptr = NULL;
            break;
        case IPPROTO_IP:
            DeleteLWSession(ip_lws_cache, ssn, "closed normally");
            p->ssnptr = NULL;
            break;
        case IPPROTO_ICMP:
            DeleteLWSession(icmp_lws_cache, ssn, "closed normally");
            p->ssnptr = NULL;
            break;
        default:
            break;
        }
    }
}

static int Stream5AlertFlushStream(Packet *p)
{
    Stream5LWSession *ssn;

    if (!p || !p->ssnptr)
    {
        DEBUG_WRAP(DebugMessage(DEBUG_STREAM_STATE,
                    "Don't flush NULL packet or session\n"););
        return 0;
    }

    ssn = p->ssnptr;
    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    if (!(s5_global_eval_config->flags & STREAM5_CONFIG_FLUSH_ON_ALERT))
    {
        DEBUG_WRAP(DebugMessage(DEBUG_STREAM_STATE,
                    "Don't flush on alert from individual packet\n"););
        return 0;
    }

    if ((ssn->protocol != IPPROTO_TCP) ||
        (p->packet_flags & PKT_REBUILT_STREAM))
    {
        DEBUG_WRAP(DebugMessage(DEBUG_STREAM_STATE,
                    "Don't flush on rebuilt packets\n"););
        return 0;
    }

    /* Flush the listener queue -- this is the same side that
     * the packet gets inserted into */
    Stream5FlushListener(p, ssn);

    return 0;
}

static int Stream5ResponseFlushStream(Packet *p)
{
    Stream5LWSession *ssn;

    if ((p == NULL) || (p->ssnptr == NULL))
    {
        DEBUG_WRAP(DebugMessage(DEBUG_STREAM_STATE,
                    "Don't flush NULL packet or session\n"););
        return 0;
    }

    ssn = p->ssnptr;
    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    if ((ssn->protocol != IPPROTO_TCP) ||
        (p->packet_flags & PKT_REBUILT_STREAM))
    {
        DEBUG_WRAP(DebugMessage(DEBUG_STREAM_STATE,
                    "Don't flush on rebuilt packets\n"););
        return 0;
    }

    /* Flush the talker queue -- this is the opposite side that
     * the packet gets inserted into */
    Stream5FlushTalker(p, ssn);

    return 0;
}

static uint32_t Stream5SetSessionFlags(
                    void *ssnptr,
                    uint32_t flags)
{
    Stream5LWSession *ssn;
    if (ssnptr)
    {
        ssn = (Stream5LWSession *)ssnptr;
        if ((ssn->ha_state.session_flags & flags) != flags)
        {
            ssn->ha_state.session_flags |= flags;
        }
        return ssn->ha_state.session_flags;
    }

    return 0;
}

static uint32_t Stream5GetSessionFlags(void *ssnptr)
{
    Stream5LWSession *ssn;
    if (ssnptr)
    {
        ssn = (Stream5LWSession *)ssnptr;
        return ssn->ha_state.session_flags;
    }

    return 0;
}

static int Stream5AddSessionAlert(
    void *ssnptr,
    Packet *p,
    uint32_t gid,
    uint32_t sid)
{
    Stream5LWSession *ssn;

    if ( !ssnptr )
        return 0;

    ssn = (Stream5LWSession *)ssnptr;
    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    /* Don't need to do this for other protos because they don't
       do any reassembly. */
    if ( GET_IPH_PROTO(p) != IPPROTO_TCP )
        return 0;

    return Stream5AddSessionAlertTcp(ssn, p, gid, sid);
}

/* return non-zero if gid/sid have already been seen */
static int Stream5CheckSessionAlert(
    void *ssnptr,
    Packet *p,
    uint32_t gid,
    uint32_t sid)
{
    Stream5LWSession *ssn;

    if ( !ssnptr )
        return 0;

    ssn = (Stream5LWSession *)ssnptr;
    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    /* Don't need to do this for other protos because they don't
       do any reassembly. */
    if ( GET_IPH_PROTO(p) != IPPROTO_TCP )
        return 0;

    return Stream5CheckSessionAlertTcp(ssn, p, gid, sid);
}

static int Stream5UpdateSessionAlert(
    void *ssnptr,
    Packet *p,
    uint32_t gid,
    uint32_t sid,
    uint32_t event_id,
    uint32_t event_second)
{
    Stream5LWSession *ssn;

    if ( !ssnptr )
        return 0;

    ssn = (Stream5LWSession *)ssnptr;
    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    /* Don't need to do this for other protos because they don't
       do any reassembly. */
    if ( GET_IPH_PROTO(p) != IPPROTO_TCP )
        return 0;

    return Stream5UpdateSessionAlertTcp(ssn, p, gid, sid, event_id, event_second);
}

static void Stream5SetExtraData (void* pv, Packet* p, uint32_t flag)
{
    Stream5LWSession* ssn = pv;

    if ( !ssn )
        return;

    Stream5SetExtraDataTcp(ssn, p, flag);
}

// FIXTHIS get pv/ssn from packet directly?
static void Stream5ClearExtraData (void* pv, Packet* p, uint32_t flag)
{
    Stream5LWSession* ssn = pv;

    if ( !ssn )
        return;

    Stream5ClearExtraDataTcp(ssn, p, flag);
}

static int Stream5IgnoreChannel(
                    snort_ip_p      srcIP,
                    uint16_t srcPort,
                    snort_ip_p      dstIP,
                    uint16_t dstPort,
                    uint8_t protocol,
                    time_t now,
                    uint32_t preprocId,
                    char direction,
                    char flags)
{
    return StreamExpectAddChannel(srcIP, srcPort, dstIP, dstPort,
                                  protocol, now, direction, flags, S5_EXPECTED_CHANNEL_TIMEOUT,
                                  0, preprocId, NULL, NULL);
}

static int Stream5GetIgnoreDirection(void *ssnptr)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;
    if (!ssn)
        return SSN_DIR_NONE;

    return ssn->ha_state.ignore_direction;
}

static int Stream5SetIgnoreDirection(void *ssnptr, int ignore_direction)
{
    Stream5LWSession *lwssn = (Stream5LWSession *)ssnptr;

    if (!lwssn)
        return 0;

    if (lwssn->ha_state.ignore_direction != ignore_direction)
    {
        lwssn->ha_state.ignore_direction = ignore_direction;
    }

    return lwssn->ha_state.ignore_direction;
}

void Stream5DisableInspection(Stream5LWSession *lwssn, Packet *p)
{
    /*
     * Don't want to mess up PortScan by "dropping"
     * this packet.
     *
     * Also still want the perfmon to collect the stats.
     *
     * And don't want to do any detection with rules
     */
    DisableAllDetect(p);
    SetPreprocBit(p, PP_SFPORTSCAN);
    SetPreprocBit(p, PP_PERFMONITOR);
    otn_tmp = NULL;
}

static void Stream5StopInspection(
                    void * ssnptr,
                    Packet *p,
                    char dir,
                    int32_t bytes,
                    int response)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn)
        return;

    switch (dir)
    {
        case SSN_DIR_BOTH:
        case SSN_DIR_FROM_CLIENT:
        case SSN_DIR_FROM_SERVER:
            if (ssn->ha_state.ignore_direction != dir)
            {
                ssn->ha_state.ignore_direction = dir;
            }
            break;
    }

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return;

    /* Flush any queued data on the client and/or server */
    if (ssn->protocol == IPPROTO_TCP)
    {
        if (ssn->ha_state.ignore_direction & SSN_DIR_FROM_CLIENT)
        {
            Stream5FlushClient(p, ssn);
        }

        if (ssn->ha_state.ignore_direction & SSN_DIR_FROM_SERVER)
        {
            Stream5FlushServer(p, ssn);
        }
    }

    /* TODO: Handle bytes/response parameters */

    Stream5DisableInspection(ssn, p);
}

static void Stream5ResumeInspection(
                    void *ssnptr,
                    char dir)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn)
        return;

    switch (dir)
    {
        case SSN_DIR_BOTH:
        case SSN_DIR_FROM_CLIENT:
        case SSN_DIR_FROM_SERVER:
            if (ssn->ha_state.ignore_direction & dir)
            {
                ssn->ha_state.ignore_direction &= ~dir;
            }
            break;
    }

}

static void Stream5UpdateDirection(
                    void * ssnptr,
                    char dir,
                    snort_ip_p ip,
                    uint16_t port)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn)
        return;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return;

    switch (ssn->protocol)
    {
        case IPPROTO_TCP:
            TcpUpdateDirection(ssn, dir, ip, port);
            break;
        case IPPROTO_UDP:
            UdpUpdateDirection(ssn, dir, ip, port);
            break;
        case IPPROTO_ICMP:
            //IcmpUpdateDirection(ssn, dir, ip, port);
            break;
    }
}

static uint32_t Stream5GetPacketDirection(Packet *p)
{
    Stream5LWSession *lwssn;

    if (!p || !(p->ssnptr))
        return 0;

    lwssn = (Stream5LWSession *)p->ssnptr;
    if (Stream5SetRuntimeConfiguration(lwssn, lwssn->protocol) == -1)
        return 0;

    GetLWPacketDirection(p, lwssn);

    return (p->packet_flags & (PKT_FROM_SERVER|PKT_FROM_CLIENT));
}

static void Stream5DropTraffic(
                    Packet* p,
                    void *ssnptr,
                    char dir)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn)
        return;

    if ((dir & SSN_DIR_FROM_CLIENT) && !(ssn->ha_state.session_flags & SSNFLAG_DROP_CLIENT))
    {
        ssn->ha_state.session_flags |= SSNFLAG_DROP_CLIENT;
        if ( Active_PacketForceDropped() )
            ssn->ha_state.session_flags |= SSNFLAG_FORCE_BLOCK;
    }

    if ((dir & SSN_DIR_FROM_SERVER) && !(ssn->ha_state.session_flags & SSNFLAG_DROP_SERVER))
    {
        ssn->ha_state.session_flags |= SSNFLAG_DROP_SERVER;
        if ( Active_PacketForceDropped() )
            ssn->ha_state.session_flags |= SSNFLAG_FORCE_BLOCK;
    }
}

static void Stream5DropPacket(Packet *p)
{
    Stream5LWSession* ssn = (Stream5LWSession *) p->ssnptr;

    if (!ssn)
        return;

    switch (ssn->protocol)
    {
        case IPPROTO_TCP:
            Stream5TcpSessionClear(p);
            break;
        case IPPROTO_UDP:
            UdpSessionCleanup(ssn);
            break;
        case IPPROTO_IP:
            IpSessionCleanup(ssn);
            break;
        case IPPROTO_ICMP:
            IcmpSessionCleanup(ssn);
            break;
        default:
            break;
    }


    if (!(p->packet_flags & PKT_STATELESS))
        Stream5DropTraffic(p, p->ssnptr, SSN_DIR_BOTH);
}

static int Stream5GetRebuiltPackets(
                            Packet *p,
                            PacketIterator callback,
                            void *userdata)
{
    Stream5LWSession *ssn = (Stream5LWSession*)p->ssnptr;

    if (!ssn || ssn->protocol != IPPROTO_TCP)
        return 0;

    /* Only if this is a rebuilt packet */
    if (!(p->packet_flags & PKT_REBUILT_STREAM))
        return 0;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    return GetTcpRebuiltPackets(p, ssn, callback, userdata);
}

static int Stream5GetStreamSegments(
        Packet *p,
        StreamSegmentIterator callback,
        void *userdata)
{
    Stream5LWSession *ssn = (Stream5LWSession*)p->ssnptr;

    if ((ssn == NULL) || (ssn->protocol != IPPROTO_TCP))
        return -1;

    /* Only if this is a rebuilt packet */
    if (!(p->packet_flags & PKT_REBUILT_STREAM))
        return -1;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return -1;

    return GetTcpStreamSegments(p, ssn, callback, userdata);
}

static StreamFlowData *Stream5GetFlowData(Packet *p)
{
    Stream5LWSession *ssn = (Stream5LWSession*)p->ssnptr;

    if (!ssn)
        return NULL;

    return (StreamFlowData *)ssn->flowdata->data;
}

static char Stream5GetReassemblyDirection(void *ssnptr)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn || ssn->protocol != IPPROTO_TCP)
        return SSN_DIR_NONE;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return SSN_DIR_NONE;

    return Stream5GetReassemblyDirectionTcp(ssn);
}

static uint32_t Stream5GetFlushPoint(void *ssnptr, char dir)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if ((ssn == NULL) || (ssn->protocol != IPPROTO_TCP))
        return 0;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    return Stream5GetFlushPointTcp(ssn, dir);
}

static void Stream5SetFlushPoint(void *ssnptr, char dir, uint32_t flush_point)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if ((ssn == NULL) || (ssn->protocol != IPPROTO_TCP))
        return;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return;

    Stream5SetFlushPointTcp(ssn, dir, flush_point);
}

static char Stream5SetReassembly(void *ssnptr,
                                   uint8_t flush_policy,
                                   char dir,
                                   char flags)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn || ssn->protocol != IPPROTO_TCP)
        return 0;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 0;

    return Stream5SetReassemblyTcp(ssn, flush_policy, dir, flags);
}

static char Stream5GetReassemblyFlushPolicy(void *ssnptr, char dir)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn || ssn->protocol != IPPROTO_TCP)
        return STREAM_FLPOLICY_NONE;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return STREAM_FLPOLICY_NONE;

    return Stream5GetReassemblyFlushPolicyTcp(ssn, dir);
}

static char Stream5IsStreamSequenced(void *ssnptr, char dir)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn || ssn->protocol != IPPROTO_TCP)
        return 1;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 1;

    return Stream5IsStreamSequencedTcp(ssn, dir);
}

static int Stream5MissingInReassembled(void *ssnptr, char dir)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn || ssn->protocol != IPPROTO_TCP)
        return SSN_MISSING_NONE;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return SSN_MISSING_NONE;

    return Stream5MissingInReassembledTcp(ssn, dir);
}

static char Stream5PacketsMissing(void *ssnptr, char dir)
{
    Stream5LWSession *ssn = (Stream5LWSession *)ssnptr;

    if (!ssn || ssn->protocol != IPPROTO_TCP)
        return 1;

    if (Stream5SetRuntimeConfiguration(ssn, ssn->protocol) == -1)
        return 1;

    return Stream5PacketsMissingTcp(ssn, dir);
}

static uint16_t s5GetPreprocessorStatusBit(void)
{
    static uint16_t preproc_filter_status_bit = PORT_MONITOR_SESSION;

    preproc_filter_status_bit <<= 1;

    return preproc_filter_status_bit;
}

static void s5SetPortFilterStatus(
        struct _SnortConfig *sc,
        int protocol,
        uint16_t port,
        uint16_t status,
        tSfPolicyId policyId,
        int parsing
        )
{
    switch (protocol)
    {
        case IPPROTO_TCP:
            s5TcpSetPortFilterStatus(sc, port, status, policyId, parsing);
            break;
        case IPPROTO_UDP:
            s5UdpSetPortFilterStatus(sc, port, status, policyId, parsing);
            break;
        case IPPROTO_ICMP:
            break;
        default:
            break;
    }
}

static void s5UnsetPortFilterStatus(
        struct _SnortConfig *sc,
        int protocol,
        uint16_t port,
        uint16_t status,
        tSfPolicyId policyId,
        int parsing
        )
{
    if (status <= PORT_MONITOR_SESSION)
        return;

    switch (protocol)
    {
        case IPPROTO_TCP:
            s5TcpUnsetPortFilterStatus(sc, port, status, policyId, parsing);
            break;
        case IPPROTO_UDP:
            s5UdpUnsetPortFilterStatus(sc, port, status, policyId, parsing);
            break;
        case IPPROTO_ICMP:
            break;
        default:
            break;
    }
}
static uint8_t s5GetHopLimit (void* pv, char dir, int outer)
{
    Stream5LWSession *ssn = (Stream5LWSession*)pv;

    if ( !ssn )
        return 255;

    if ( SSN_DIR_FROM_CLIENT == dir )
        return outer ? ssn->outer_client_ttl : ssn->inner_client_ttl;

    return outer ? ssn->outer_server_ttl : ssn->inner_server_ttl;
}

static int Stream5SetApplicationProtocolIdExpected(
                    snort_ip_p      srcIP,
                    uint16_t srcPort,
                    snort_ip_p      dstIP,
                    uint16_t dstPort,
                    uint8_t protocol,
                    time_t now,
                    int16_t protoId,
                    uint32_t preprocId,
                    void *protoData,
                    void (*protoDataFreeFn)(void*))
{
    return StreamExpectAddChannel(srcIP, srcPort, dstIP, dstPort,
                                  protocol, now, SSN_DIR_BOTH, 0, S5_EXPECTED_CHANNEL_TIMEOUT,
                                  protoId, preprocId, protoData, protoDataFreeFn);
}

int isPacketFilterDiscard(
        Packet *p,
        int ignore_any_rules
        )
{
    uint8_t  action = 0;
    tPortFilterStats   *pPortFilterStats = NULL;
    tSfPolicyId policy_id = getRuntimePolicy();

    switch(GET_IPH_PROTO(p))
    {
        case IPPROTO_TCP:
            if ((s5_global_eval_config != NULL) &&
                s5_global_eval_config->track_tcp_sessions)
            {
                action = s5TcpGetPortFilterStatus(NULL, p->sp, policy_id, 0) |
                    s5TcpGetPortFilterStatus(NULL, p->dp, policy_id, 0);
            }

            pPortFilterStats = &s5stats.tcp_port_filter;
            break;

        case IPPROTO_UDP:
            if ((s5_global_eval_config != NULL) &&
                s5_global_eval_config->track_udp_sessions)
            {
                action = s5UdpGetPortFilterStatus(NULL, p->sp, policy_id, 0) |
                    s5UdpGetPortFilterStatus(NULL, p->dp, policy_id, 0);
            }

            pPortFilterStats = &s5stats.udp_port_filter;
            break;
        default:
            return PORT_MONITOR_PACKET_PROCESS;
    }

    if (!(action & PORT_MONITOR_SESSION_BITS))
    {
        if (!(action & PORT_MONITOR_INSPECT) && ignore_any_rules)
        {
            /* Ignore this TCP packet entirely */
            DisableDetect(p);
            SetPreprocBit(p, PP_SFPORTSCAN);
            SetPreprocBit(p, PP_PERFMONITOR);
            //otn_tmp = NULL;
            pPortFilterStats->filtered++;
        }
        else
        {
            pPortFilterStats->inspected++;
        }

        return PORT_MONITOR_PACKET_DISCARD;
    }

    pPortFilterStats->session_tracked++;
    return PORT_MONITOR_PACKET_PROCESS;
}

static bool Stream5RegisterPAFPort (
    struct _SnortConfig *sc, tSfPolicyId id,
    uint16_t server_port, bool to_server,
    PAF_Callback cb, bool autoEnable)
{
    return s5_paf_register_port(sc, id, server_port, to_server, cb, autoEnable);
}

static bool Stream5RegisterPAFService (
    struct _SnortConfig *sc, tSfPolicyId id,
    uint16_t service, bool to_server,
    PAF_Callback cb, bool autoEnable)
{
    return s5_paf_register_service(sc, id, service, to_server, cb, autoEnable);
}

static uint32_t Stream5RegisterXtraData(LogFunction f)
{
    uint32_t i = 0;
    while(i < xtradata_func_count)
    {
        if(xtradata_map[i++] == f)
        {
            return i;
        }
    }
    if ( xtradata_func_count == LOG_FUNC_MAX)
        return 0;
    xtradata_map[xtradata_func_count++] = f;
    return xtradata_func_count;
}

static uint32_t Stream5GetXtraDataMap(LogFunction **f)
{
    if(f)
    {
        *f = xtradata_map;
        return xtradata_func_count;
    }
    else
        return 0;
}

static void Stream5RegisterXtraDataLog(LogExtraData f, void *config)
{
    extra_data_log = f;
    extra_data_config = config;
}

void** Stream5GetPAFUserData(void* ssnptr, bool to_server)
{
    return Stream5GetPAFUserDataTcp((Stream5LWSession*)ssnptr, to_server);
}

static bool Stream5IsPafActive (void* ssnptr, bool to_server)
{
    return Stream5IsPafActiveTcp((Stream5LWSession*)ssnptr, to_server);
}

static bool Stream5ActivatePaf (void* ssnptr, bool to_server)
{
    return Stream5ActivatePafTcp((Stream5LWSession*)ssnptr, to_server);
}

static void s5GetMaxSessions(struct _SnortConfig *sc, tSfPolicyId policyId, StreamSessionLimits* limits)
{
    tSfPolicyUserContextId context;
    Stream5Config* config;

    context = s5_config;
    config = sfPolicyUserDataGet(context, policyId);

    if (config && config->global_config)
    {
        limits->tcp_session_limit = config->global_config->track_tcp_sessions ? config->global_config->max_tcp_sessions : 0;
        limits->udp_session_limit = config->global_config->track_udp_sessions ? config->global_config->max_udp_sessions : 0;
        limits->icmp_session_limit = config->global_config->track_icmp_sessions ? config->global_config->max_icmp_sessions : 0;
        limits->ip_session_limit = config->global_config->track_ip_sessions ? config->global_config->max_ip_sessions : 0;
    }
    else
        memset(limits, 0, sizeof(*limits));
}

#endif
