#include <stdio.h>


//二维数组中查找一个数字
int find_number(int a[4][4], const int number)
{
	int i=0;
	int j=3;
	while(i>=0 && j>=0){
		int tmp = a[i][j];
		printf("a[%d][%d] = %d\n", i, j, tmp);
		if (tmp == number) {
			return 1; 
		}
		else if (tmp < number){
			i++;
		}
		else if (tmp > number){
			j--;
		}
		if (j>3 || i>3 || j<0 || i<0){
			break;
		}
	}
	return 0;
}

//合并数组
void array_merg(int a[14], int b[13])
{
	char c[30];
	int i=0,j=0,k=0;
	while(1){
		if ((i == 14) && (j == 13)){
			break;
		}
		else if (i == 14){
			c[k++] = b[j++];
		}
		else if (j == 13){
			c[k++] = a[i++];
		}
		else {
			c[k++] = (a[i] < b[j]) ? a[i++]:b[j++];
		}
		printf("c[%d] = %d:  i=%d   j=%d\n", k-1, c[k-1], i, j);
	}
}

int main()
{
//	int a[4][4] = {
//		{1,2,8,9},
//		{2,4,9,12},
//		{4,7,10,13},
//		{6,8,11,15},
//	};
	int c[14] = {1,3,5,7,9,11,13,15,17,19,36,93,102,123};
	int d[13] = {2,4,6,8,10,12,14,16,18,20,23,24,25};
	array_merg(c,d);

	return 0;

//	int b = find_number(a, 6);
//	printf("%d\n", b);
}
