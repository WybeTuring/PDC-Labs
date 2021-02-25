#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 10
#define MAX_THREAD 2

int p = 0;
int final[2] = {0,0};
int arr[] = {0,1,2,3,4,5,6,7,8,9};

void* sumfunction(void* f){
    int t = p++;
    for(int i = t * (MAX/2); i < (t + 1) * (MAX/2); i++){
        final[t] += arr[i];
    }
}

int main(){
	pthread_t threads[MAX_THREAD];

    for(int i = 0; i < MAX_THREAD; i++){
        pthread_create(&threads[i], NULL, sumfunction, (void*)NULL);
    }

    for(int i = 0; i < MAX_THREAD; i++){
        pthread_join(threads[i], NULL);
    }

    int sum = 0;
    for(int i = 0; i < MAX_THREAD; i++){
        sum += final[i];
    }
	
    printf("The sum of the array is %d", sum);
	return 0;
}
