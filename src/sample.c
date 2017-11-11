#include<stdio.h>

int main()
{
	int x[10];
	int i, j;
	int temp;
	for (i = 0; i<10; i++) {
		x[i] = 10-i;
	}
	for (i = 0; i<10; i++) {
		printf("%d ", x[i]);
	}
	printf("\n");
	for (i = 0; i<10; i++){
		for (j = i; j<10; j++){
			if (x[i] > x[j]) {
				temp = x[i];
				x[i] = x[j];
				x[j] = temp;
			}
		}
	}
	for (i = 0; i<10; i++) {
		printf("%d ", x[i]);
	}
	printf("\n");
}
