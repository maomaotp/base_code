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

//实现将字符串中的空格替换为'%20'
int trasf(char *str, char * dst)
{
	while( *str != 0) {
		if (*str == 32){
			*dst++ = '%';
			*dst++ = '2';
			*dst++ = '0';
		}
		else {
			*dst++ = *str;
		}
		str++;
	}
}

//将空格替换为 '%20' 在原字符串基础上
int trasf2(char str[], int length)
{
	int num=0;
	int i=0;
	while(str[i++] != '\0'){
		if (str[i] == ' '){
			num++;
		}
	}
	int add = num*2;
	int new = length+add;
	while(length) {
		if (str[length] != ' ') {
			str[new] = str[length]; 
			new--;
		}
		else {
			str[new--] = '0';
			str[new--] = '2';
			str[new--] = '%';
		}
		length--;
	}
}

int main()
{
    char str[24] = "hello world";

	char a[40] = "we are happy.";
	char dst[30];
	//trasf(a, dst);
	trasf2(a, strlen(a));
	printf("a=%s\n", a);
	
    return 0;
}
