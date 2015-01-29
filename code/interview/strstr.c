#include <stdio.h>

int main()
{
    char str[24] = "hello ";
    char dst[24] = "world";
    int str_len=0;
    int dst_len=0;

    while(str[++str_len]){
    }
    while(dst[dst_len]) {
        str[str_len++] = dst[dst_len++];
    }
    str[str_len+1] = '\0';
    printf("str = %s strlen = %d dst_len = %d\n", str, str_len, dst_len);

    return 0;

}
