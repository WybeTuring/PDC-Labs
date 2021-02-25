#include <stdio.h>
#include <stdlib.h>

int main(){
	int num;
	puts("Enter a number whose factorial you want to compute: ");
	scanf("%d", &num);
	if(num < 0){
		puts("Sorry, we cannot have factorials of negative numbers.");
	}
	else if(num == 1 || num == 0){
		printf("The factorial of %d is %d.", num, 1);
	}
	else{
		int temp = 1;
		for(int j = 0; j < num; j++){
			temp *= num - j;
		}
		printf("The factorial of %d is %d.", num, temp);
	}

	return 0;
}
