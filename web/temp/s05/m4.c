#include <stdio.h>
int func(int n) {
	int sum=0,i;
	for(i=0; i<n; i++)
	{
		sum+=i;
	}
	return sum;
}


int main()
{
	int n, i;
	scanf("%d", &n);
	long result = 0;
	
	for(i=1; i<=n; i++)
	{
		result += i;
	}
result += 10;
	printf("result[1-100] = %ld \n", result );
	printf("hello\n");
result += 10;
result += 10;
result += 10;
	
	printf("result[1-250] = %d \n", func(250) );
}
