#ifndef MULTIKEYMATCH_H
#define MULTIKEYMATCH_H
#include<string.h>
#include<list>
#include<map>
//#include"data.h"
using namespace std;
struct result_struct{
  int res;
  int codeflag;
  char* key;
};
struct match_struct{
  char pos[3];
  int space;
  char** listarr;
};
class MultiKeyMatch{
  private:
  int max;
  int count;
  int key_count;
  map<unsigned short,struct match_struct> match_map;
	bool match_flag;
  list<char*> keylist;
  private:
  void GetMax();
  void GetMatchKey(struct match_struct* match,char* key);
  void GetMatchArr();
  void GetMatchMap();
  bool PositionExist(unsigned short position);
  unsigned short GetPos(char ch1,char ch2);
  bool MatchByte(char* source,int site,char* p);
  struct result_struct BackMatch(char** backlist,int size,char* source);
  struct result_struct AMatch(char* source,int start,int len);
  void GetShift();
  public:
  MultiKeyMatch();
  ~MultiKeyMatch();
  int AddKey(char* key);
  void MatchInit();
  void MatchClear();
  void MatchMain(char* sourc,list<result_struct>* result,int len);
};
#endif
