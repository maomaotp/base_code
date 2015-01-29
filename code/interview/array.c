#include <stdio.h>


//二维数组中查找一个数字
int find_number(int a[4][4], const int number)
{
	int i=0;
	int j=3;
	while(i>=0 && 3>=j>=0){
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

int main()
{
	int a[4][4] = {
		{1,2,8,9},
		{2,4,9,12},
		{4,7,10,13},
		{6,8,11,15},
	};

	int b = find_number(a, 6);
	printf("%d\n", b);
}
