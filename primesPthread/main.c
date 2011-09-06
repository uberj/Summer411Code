#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "primes.h"
#define NUM_THREADS 2

void print_results(struct Data *data){
	int i, size;
	size = data->primes->size;
	for(i = 0; i < size; i++) printf("%d\n", data->primes->data[i]);
}

int main (int argc, const char *argv[]) {
	int i, upperBound;
	pthread_t threads[NUM_THREADS];
	time_t startTime, endTime, totTime;
	struct Data *data = (struct Data *)malloc(sizeof(struct Data)); 
	data->primes = (struct DynArr *)malloc(sizeof(struct DynArr));
	initDynArr(data->primes, 10); 
	upperBound = atoi(argv[1]);
	printf("%d is bound\n", upperBound);
	startTime = time(NULL);
	data->lock = 0;
	for(i = 2; i <= upperBound; i++){
		printf("%d\n", i);
		while(data->lock == 1) ;
		data->lock++;
		data->tested = i;
		/* isPrimeDAPara((void *)data); */
        	pthread_create(&threads[NUM_THREADS % i], NULL, isPrimeDAPara,(void *)data);
		if(i != 2) i++;
	}
	pthread_exit(NULL);
	endTime = time(NULL);
	totTime = endTime - startTime;
	printf("%d\n", data->primes->size);
	printf("TIME: %ld:%ld\n", totTime/60, totTime - ((totTime/60)*60));
	free(data->primes->data);
	free(data->primes);
	free(data);
	return 0;
}
