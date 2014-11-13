#include "policy.h"

std::list<policy_t> policy;
#define  policy_path  "/usr/local/tipschecker/etc/policy.conf"

static void getopt_reset()
{
   optarg=NULL;
   optind=opterr=optopt=0;
}

bool juage_policy(const u_char *packet)
{
   if(policy.empty())
   return true;

   u_int16_t vlanid=0;
   struct iphdr *ip=NULL;
   struct ether_header *ether=(struct ether_header*)packet;
   u_int16_t type=ntohs(ether->ether_type);

   if(type == ETHERTYPE_IP)
   {
      ip=(struct iphdr *)(packet+sizeof(struct ether_header));
   }
   else if(type == ETHERTYPE_VLAN)
   {
      struct vlan_tag *vlan=NULL;
      vlan=(struct vlan_tag*)(packet+sizeof(ether_header));
      
      vlanid=ntohs(vlan->vlan_tpid);
      ip=(struct iphdr *)(packet+sizeof(struct ether_header)+sizeof(struct vlan_tag));
   }

   if(ip && find_policy(vlanid,ip))
   {
     printf("%d\n",vlanid);
     return true;
   }

   return false;
}

static bool find_policy(u_int16_t vlanid,const struct iphdr *ip)
{
   uint32_t sip=ntohl(ip->saddr);
   uint32_t dip=ntohl(ip->daddr);
   std::list<policy_t>::iterator it=policy.begin();

   for(;it != policy.end();it++)
   {
     if( juage_vlan(vlanid,it) && ((juage_ip(sip,it) || juage_ip(dip,it))))
    {
      return true;    
    }
   }

   return false;
}

static bool juage_ip(uint32_t ip,std::list<policy_t>::iterator &it)
{
   if(((*it).ip_t.st_ip == 0) || ((*it).ip_t.st_ip <= ip && (*it).ip_t.en_ip >= ip))
   return true;

   return false;
}

static bool juage_vlan(u_int16_t vlanid,std::list<policy_t>::iterator &it)
{
   uint16_t v=(*it).vlan_id;

   if(v == 0 || v == vlanid)
   return true;

   return false;
}

void init_policy()
{
     int len=0;
     FILE *file=NULL;
     char buffer[128]={0};

     file=fopen(policy_path,"r");
     if(!file)
     {
        perror("fopen"); 
	exit(-1);
     }

     while(fgets(buffer,128,file))
     {
       len=strlen(buffer);
       buffer[len]=0;
       buffer[len-1]='\0';
	puts(buffer);
       add_policy(buffer);
       memset(buffer,0,128);
     }

     fclose(file);
}

static void process_policy(const char *it)
{
    int size=0;
    policy_t policy_info;
    char *pointer[10]={0};

    spite_policy(&size,it,pointer);
	for(int i=0;i<10;i++)
	{
		if(NULL!= pointer[i])
			puts(pointer[i]);
		printf("i==%d\n",i);
	}
    memset(&policy_info,0,sizeof(policy_t));
 
    insert_policy(&size,pointer,&policy_info);
}

static void insert_policy(const int *counter,char * const pointer[],policy_t *ply)
{
    char ch=0;
    char mask[64]={0};
    char ip_range[64]={0};

    getopt_reset();

    while((ch=getopt(*counter,pointer,"v:s:p:m:")) != -1)
    {
       switch(ch)
       {
         case 'v':
	     ply->vlan_id=atoi(optarg);
	     break;
	 case 's':
	     strcpy(ip_range,optarg);
	     break;
	 case 'p':
	     covert_mask(optarg,mask);
	     break;
	 case 'm':
	     strcpy(mask,optarg);
	     break;
       }
    }

   if(0 != strlen(mask) && 0 != strlen(ip_range))
   {
      char *pos=NULL;   
      if(pos=strchr(ip_range,','))
      {
         *pos++='\0';
  
          ply->ip_t.st_ip=ntohl(inet_addr(ip_range));
	  ply->ip_t.en_ip=ntohl(inet_addr(pos));
         
	  if(ply->ip_t.st_ip > ply->ip_t.en_ip)
	  {
	    uint32_t temp=ply->ip_t.st_ip;
	    ply->ip_t.st_ip=ply->ip_t.en_ip;
	    ply->ip_t.en_ip=temp;
	  }
      }
      else
      {
        getnet_policy(ip_range,mask,ply);
      }
   }

   policy.push_back(*ply);  
}

static void getnet_policy(const char *iprange,const char *mask,policy_t *ply)
{
    in_addr_t endip=0;
    in_addr_t startip=0;

    startip=ntohl(inet_addr(iprange));
    endip=ntohl(inet_addr(mask));

    unsigned int temp=0;
    temp=((startip & endip) + 1);
    ply->ip_t.st_ip=temp;

    endip=~endip;
    temp=((endip | startip)-1);
    ply->ip_t.en_ip=temp;
}

static void covert_mask(const char *trains,char *mask)
{
    static const char BASE=8;
    
    int i=0;
    char PBASE=7;
    
    int localmask=atoi(trains);

    int total=0;
    char buffer[8]={0};
    unsigned char unkown=0;
    unsigned char kown=0;

    kown=localmask/BASE;
    unkown=localmask%BASE;

    memset(mask,0,sizeof(mask));

    for(i=0;i<kown;i++)
    {
      strcat(mask,"255.");
    }

    for(i=0;i<unkown;i++)
    {
      total+=pow(2,PBASE); 
      PBASE--;
    }

    sprintf(buffer,"%d.",total);

    if(4 - kown > 0)
    {
       strcat(mask,buffer);
       kown++;
    }
 
    for(i=0;i<(4-kown);i++)
    {
       strcat(mask,"0.");
    }

    mask[strlen(mask)-1]='\0';
}

static void spite_policy(int *temp,const char *str,char *p[])
{
    int counter=0;
    char *pos=NULL;

    char *inter=(char*)str;
    char *btr=(char*)str;
    char *postemp=NULL;

    while((pos=strchr(inter,' ')))
    {
        *pos='\0';
        p[counter]=btr;
	counter++;
	pos++;
	btr=pos;
	inter=pos;
	postemp=pos;
    }

    p[counter]=postemp;

    *temp=counter+1;
}

static void add_policy(const char *it)
{
   if(!check_policy(it))
   {
      return ;
   }

   process_policy(it);
}

static bool check_policy(const char *pcy)
{
    char *pr=NULL;
    char *sr=NULL;
    char *mr=NULL;
    const char *str=(const char *)pcy;
    
    pr=(char*)strstr(str,"p");
    sr=(char*)strstr(str,"s");
    mr=(char*)strstr(str,"m");

    if(sr)
    {
        return sr && pr || sr && mr;
    }
    else
    {
       return NULL == mr && pr == mr;
    }
}
