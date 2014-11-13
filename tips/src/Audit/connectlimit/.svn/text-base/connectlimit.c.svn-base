#include "connectlimit.h"
#define PASS "201401161503_tips_connection_limit"
#define LIMIT_FILEPATH "/usr/local/tipschecker/etc/tipscheck/tipslimit"
#define BUF_LEN 32
char* encrypt(char* source, char* pass)
{
	int i = 0;
	int source_length = strlen(source);
	int pass_length = strlen(pass);
	char* tmp_str = (char*)malloc((source_length + 1) * sizeof(char));
	memset(tmp_str, 0, source_length + 1);
	for(i = 0; i < source_length; ++i)
	{
		tmp_str[i] = source[i] ^ pass[i%pass_length];
		if(tmp_str[i] == 0)
		{
			tmp_str[i] = source[i];
		}
	}
	tmp_str[source_length] = 0;
	return tmp_str;
}

int set_limit(long num)
{
	FILE* fp =	fopen(LIMIT_FILEPATH, "wb");
	if(NULL == fp)
	{
		printf("open tipslimit file failed\n");
		return -1;
	}
	char num_str[BUF_LEN] = {0};
	char* pass = PASS;
	snprintf(num_str, sizeof(num_str), "%ld", num);
//	printf("num_str:%s\n",num_str);
	char* encrypted_text = encrypt(num_str, pass);
	char buf[BUF_LEN] = {0};
	strcpy(buf, encrypted_text);
	fwrite(buf, BUF_LEN,1,fp);
	fclose(fp);
	printf("set connection limit : %ld\n", num);
//	printf("write buf is:%s\n", buf);
//	char* decrypted_text = encrypt(encrypted_text, pass);
//	printf("Decrypted text is:\n%s\n", decrypted_text);
	free(encrypted_text);
//	free(decrypted_text);
	return 0;
}

long get_limit()
{
	FILE* fp =	fopen(LIMIT_FILEPATH, "rb");
	if(NULL == fp)
	{
		printf("get limit : open tipslimit file failed\n");
		return -1;
	}
	char encrypted_text[BUF_LEN] = {0};
	fread(encrypted_text, BUF_LEN,1,fp);
	fclose(fp);
//	printf("Encrypted text is:%s\n", encrypted_text);
	char* pass = PASS;
	char* decrypted_text = encrypt(encrypted_text, pass);
//	printf("Decrypted text is:%s\n", decrypted_text);
	long num = atol(decrypted_text);
	free(decrypted_text);
	printf("get connection limit : %ld\n", num);
	return num;
}

