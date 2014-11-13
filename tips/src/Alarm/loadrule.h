#ifndef LOADRULE_H
#define LOADRULE_H
 
struct filerule
{
   char identifier[8];
   char name[32];
   char type[3];
   int length;
   unsigned char *content;
   int begin;
   int current;
   int totallength;
   unsigned int vect[9];
   struct filerule *next;
};

void rea();
void processrule(const char *ruled);
struct filerule * addrule();
void convert();
#endif
