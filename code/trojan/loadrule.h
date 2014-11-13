#ifndef LOADRULE_H
#define LOADRULE_H
 
struct filerule
{
   char identifier[8];
   char name[64];
   char type[3];
   int length;
   char *content;
   int begin;
   int current;
   int totallength;
   unsigned int vect[9];
   struct filerule *next;
};

void rea();
void processrule(int i, const char *ruled);
struct filerule * addrule();
void convert(const char *src,unsigned char *dest,int length);

#endif
