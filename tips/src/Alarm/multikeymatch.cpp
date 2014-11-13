#include "multikeymatch.h"
#include<iostream>
void MultiKeyMatch::GetMax()
{
		int k=0;
		list<char*>::iterator beg=keylist.begin();
		max=-1;
		while(beg!=keylist.end())
		{
				k=strlen(*beg);
				if(k<max||max==-1)max=k;
				beg++;
		}
		max-=1;
}
unsigned short MultiKeyMatch::GetPos(char ch1,char ch2)
{
		unsigned short posi=ch1;
		posi=posi<<8;
		posi+=ch2;
		return posi;
}
bool MultiKeyMatch::PositionExist(unsigned short position)
{
		map<unsigned short,struct match_struct>::iterator my_it;
		my_it=match_map.find(position);
		if(my_it==match_map.end())return true;
		return false;
}
void MultiKeyMatch::GetMatchKey(struct match_struct* match,char* key)
{
		for(int i=0;i<key_count;++i)
		{
				if(match->listarr[i])continue;
				else
				{
						match->listarr[i]=key;
						return;
				}
		}
}
void MultiKeyMatch::GetMatchArr()
{
		map<unsigned short,struct match_struct>::iterator beg=match_map.begin();
		list<char*>::iterator key_beg=keylist.begin();
		while(beg!=match_map.end())
		{
				if(beg->second.space!=0)
				{
						beg++;
						continue;
				}
				else
				{
						key_beg=keylist.begin();
						while(key_beg!=keylist.end())
						{
								char* pkey=(*key_beg)+(strlen(*key_beg)-2);
								if(!memcmp(pkey,beg->second.pos,2))
								{
										GetMatchKey(&(beg->second),(*key_beg));
								}
								key_beg++;
						}
						beg++;
				}
		}
}
void MultiKeyMatch::GetMatchMap()
{
	list<char*>::iterator beg=keylist.begin();
	while(beg!=keylist.end())
	{
		for(unsigned int j=0; j<(strlen(*beg)-1); ++j)
		{
			unsigned short position=(*beg)[j];
			position=position<<8;
			position+=(*beg)[j+1];
			int s=strlen(*beg)-j-2;
			if(PositionExist(position))
			{
				struct match_struct match;
				memset(&match,0,sizeof(match));
				strncpy(match.pos,(*beg)+j,2);
				match.space=max;
				if(s<match.space)match.space=s;
				if(match.space==0)
				{
						match.listarr=new char*[key_count];
						memset(match.listarr,0,key_count*4);
				}
				match_map.insert(make_pair(position,match));
			}
			else
			{
				map<unsigned short,struct match_struct>::iterator my_it;
				my_it=match_map.find(position);
				if(my_it->second.space>s)
				{
					my_it->second.space=s;
					if(my_it->second.space==0)
					{
						my_it->second.listarr=new char*[key_count];
						memset(my_it->second.listarr,0,key_count*4);
					} 
				}
			}
		}
		beg++;
	}
	return;
}
bool MultiKeyMatch::MatchByte(char* source,int site,char* p)
{
		int n=strlen(p);
		int i;
		for(i=n-1;i>0;--i)
		{
				if(source[site+1]!=p[i]||source[site]!=p[i-1])break;
				site--;
		}
		if(i==0)return true;
		else return false;
}
struct result_struct MultiKeyMatch::BackMatch(char** backlist,int site,char* source)
{
		struct result_struct result;
		memset(&result,0,sizeof(result));
		for(int i=0;i<key_count;++i)
		{
				if(backlist[i]==NULL)break;
				if(MatchByte(source,site,backlist[i]))
				{
						result.key=backlist[i];
						result.res=0;
						return result;
				}
		}
		return result;
}
struct result_struct MultiKeyMatch::AMatch(char* source,int start,int len)
{
		struct result_struct result;
		result.key=NULL;
		for(int i=start;i<len-1;)
		{
				unsigned short posit=GetPos(source[i],source[i+1]);
				map<unsigned short,struct match_struct>::iterator my_it;
				my_it=match_map.find(posit);
				if(my_it==match_map.end())
				{
						i+=max;
				}
				else if(my_it->second.space==0)
				{
						result=BackMatch(my_it->second.listarr,i,source);
						if(result.key)
						{
								result.res=i;
								return result;
						}
						else i+=1;
				}
				else
				{
						i+=my_it->second.space;
				}
		}
		return result;
}
MultiKeyMatch::MultiKeyMatch()
{
	match_flag=false;
		count=0;
		key_count=0;
}
MultiKeyMatch::~MultiKeyMatch()
{

		list<char*>::iterator beg=keylist.begin();
		while(beg!=keylist.end())
		{
				delete *beg;
				beg++;    
		}
		keylist.clear();
}
int MultiKeyMatch::AddKey(char* key)
{
		if(strlen(key)>1024||strlen(key)<2)return 0;
		char* p=new char[strlen(key)+1];
		strcpy(p,key);
		keylist.push_back(p);
		key_count++;
		match_flag=true;
		return 1;
}
void MultiKeyMatch::MatchInit()
{
		//match_map.clear();
		GetMax();
		list<char*>::iterator beg=keylist.begin();
		while(beg!=keylist.end())
		{
				count+=strlen(*beg)-1;
				beg++;
		}
		GetMatchMap();
		GetMatchArr();  
}
void MultiKeyMatch::MatchClear()
{
		count=0;
		key_count=0;
		list<char*>::iterator beg=keylist.begin();
		while(beg!=keylist.end())
		{
				delete *beg;
				beg++;
		}
		keylist.clear();
		match_map.clear();
}
void MultiKeyMatch::MatchMain(char* source,list<result_struct>* result,int len)
{
		if(!match_flag)
		{
			//cout<<"match_flag return !"<<endl;
			return ;
		}
		if(len<=max)return ;
		int start=0;
		struct result_struct ret;
		while(1)
		{
				ret=AMatch(source,start,len);
				if(!ret.key)break;
				start=ret.res+1;
				ret.res-=strlen(ret.key)-2;
				result->push_back(ret);
				//len=len-start;
		}
}
