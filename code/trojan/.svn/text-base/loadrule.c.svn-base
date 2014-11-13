#include "loadrule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"


#define RULEFILE "./rule"
#define MODE "r"
#define RULESIZE 800

int datalengthtemp;
struct filerule *rule[RULESIZE];

void release()
{
   int i=0;
   for(;i<RULESIZE;i++)
   {
     free(rule[i]->content);
	 rule[i]->content=NULL;
	 free(rule[i]);
	 rule[i]=NULL;
   }
}

void readrule()
{
	char buffer[256] = { 0 };
	FILE *file=fopen(RULEFILE,MODE);
	if(!file){
		dzlog_info("open rule file failed!!!");
		abort();
		return;
	}

	while(fgets(buffer,256,file)){
		processrule(buffer);
		fflush(file);
	}
	fclose(file);
	file=NULL;
	printf("parse rule compile\n");
	//rea();
}


void rea()
{
	int i;
	for(i=0;i<RULESIZE;i++){
		if(rule[i]){
			if(rule[i]->next){
				struct filerule *temp=rule[i];
				while(temp){
					printf("i:%d,iden:%s,name:%s,type:%s,content:%s\n",i,temp->identifier,temp->name,temp->type,temp->content);
					temp=temp->next;
				}
			}
			else{
				printf("i:%d,iden:%s,name:%s,type:%s,content:%s\n",i,rule[i]->identifier,rule[i]->name,rule[i]->type,rule[i]->content); 
			}
			printf("\n\n\n");
		}
	}
}

void insertRule(unsigned int pos,struct filerule *rul)
{
    if(rul)
	{
	   if(!rule[pos])
          rule[pos]=rul;
	   else
	   {
           struct filerule *temp=NULL;
		   temp=rule[pos];
	       while(temp->next)
		   {
		      temp=temp->next;
		   }

           temp->next=rul;   
	   }
	}
}

void processrule(const char *ruled)
{
	char *ordination=(char*)((void*)ruled);
    char *temp=NULL;
	unsigned int i=1;
	int counter=0;
	int rulepos=0;

	struct filerule *horse=addrule();
	if(!horse){   
        dzlog_info("the horse is NULL!!!");
		return;
	}

	while(ordination){
		while((temp = strsep(&ordination,";\n"))){
			if(*temp == 0) break;
			if(i==1){ 
				strcpy(horse->identifier,temp);
			}
			else if(i==2){  
				strcpy(horse->name,temp);
			}
			else if(i==3){
				char *vector=temp;
				char *str=NULL;
				int data=0;
				int j=0;

				while((str = (strsep(&vector,","))))
				{
					data=atoi(str);
					horse->vect[j]=data;
					j++;
				}
			}	
			else if(i==4){
				horse->begin=atoi(temp);
			}
			else if(i==5){
				horse->length=strlen(temp);
				char vecchar[128]={0};
				strcpy(vecchar,temp);
				horse->content=(char*)calloc(1,128);
                if(NULL == horse->content)
                {
                    dzlog_info("calloc error!!!");
                    return;
                }

				memset(horse->content,0,128);
				unsigned char *crt= (unsigned char *)horse->content;
				convert(vecchar,crt,horse->length);
				datalengthtemp=horse->length;
			}
			else if(i == 6){
				strcpy(horse->type,temp);
			}
			i++;
		}
	}
    for(i=0; i<(sizeof(horse->vect)/sizeof(unsigned int)); i+=3)
    {
		if(horse->vect[i] ==0)  break;
	    counter++;
	}

	horse->totallength=counter;
    horse->current=0;

	rulepos=horse->vect[0];
	horse->length=datalengthtemp;
	horse->next=NULL;
	insertRule(rulepos,horse);
}

int temp(char t)
{
	if(t >= 48 && t <=57)
	{
	   return t-48;
	}
    else
	{
	  return t-87;
	}
}

void convert(const char *src,unsigned char *dest,int length)
{
	int i=0;
	int number;
	int second;
	unsigned char te;

    for(;i<length;i+=2)
	{
       number=0;
	   te=0;
	   second=0;
 
	   number=temp(src[i]);
	   second=temp(src[i+1]);
 
	   te=number;
	   te=(te<<4)|second;
	   
	   *dest=te;
	   dest++;
	}
}

struct filerule* addrule()
{
   struct filerule *Tro=(struct filerule *)calloc(1,sizeof(struct filerule));
   if(!Tro)
   {
      dzlog_info(" calloc fail!!!");
	  return NULL;
   }

  return Tro;
}
