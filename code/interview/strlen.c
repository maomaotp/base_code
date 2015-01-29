#include <stdio.h>
#include <string.h>

int main()
{
    char buf[24] = "hello world";
    char get_buf[24];
    int buf_len = 0;

    fgets(get_buf, sizeof(get_buf), stdin);
    
    while(get_buf[++buf_len]) {
    }
    printf("buf_len == %d\n", buf_len);
    printf("strlen(buf) = %d\n", strlen(get_buf));
    return 0;   
}
