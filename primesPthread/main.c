#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "primes.h"
#define NUM_THREADS 4

int main (int argc, const char *argv[]) {
	int i, upperBound;
	time_t startTime, endTime, totTime;
	struct Data data = (struct Data *)malloc(sizeof(struct Data)); 
	data->primes = (struct DynArr *)malloc(sizeof(struct DynArr));
	initDynArr(data->primes, 10); 
	upperBound = atoi(argv[1]);
	printf("%d is bound\n", upperBound);
	startTime = time(NULL);
	for(i = 2; i <= upperBound; i++){
		data->tested = i;
        	isPrimeDAPara(data);
		if(i != 2) i++;
	}
	endTime = time(NULL);
	totTime = endTime - startTime;
	printf("%d\n", primes->size);
	printf("TIME: %ld:%ld\n", totTime/60, totTime - ((totTime/60)*60));
	free(data->primes->data);
	free(primes);
	free(data);
	return 0;
}
