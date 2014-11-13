#include <stdio.h>
#include <string.h>

int my_strcmp(const char* str, const char* dst)
{
    int len=0;
    
    while(str[len]) {
        if(str[len] == dst[len]) {
            len++;
            continue;
        }
        else break;
    }

    return (str[len] - dst[len]);

}


int my_strlen(const char* buf)
{
    int buf_len = 0;
    
    while(buf[++buf_len]);
    
    return buf_len;   
}

int reverstr1(char* buf)
{
    int len = 0;
    int i;
    char tmp[64];
    
    while(buf[len]) {
       tmp[len] = buf[len++];
    }
    tmp[len] = '\0';
    printf("tmp == %s\n", tmp);

    for(i=0; i<len; i++){
        buf[i] = tmp[len-1-i];
    }
    buf[len] = '\0';
    printf("buf = %s\n", buf);

    return 0;

}

void reverstr2(char* buf)
{
    int len=0;
    int i;
    char tmp;

    while(buf[++len]);

    printf("len = %d\n", len);

    for(i=0; i<=len/2; i++) {
        tmp = buf[i];
        buf[i] = buf[len-1-i];
        buf[len-1-i] = tmp;
    }
    buf[len] = '\0';

}

void reverstr3(char* buf)
{
    char str1[20];
    char str2[20];

    sscanf(buf, "%s %s", str1, str2);
    printf("%s %s\n", str1, str2);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, 20, "%s %s", str2, str1);
}


int main()
{
    char str[24] = "hello world";
    char dst[24] = "hello";

    //printf("str_len ==%d\n", my_strlen(str));
    //printf("my_strcmp = %d\n", my_strcmp(str, dst));

    //reverstr1(str);
    reverstr3(str);
    printf("str==%s\n", str);

    return 0;
}
