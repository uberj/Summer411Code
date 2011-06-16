#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "primes.h"

int main (int argc, const char *argv[]) {
	int i, upperBound;
	time_t startTime, endTime, totTime;
	struct DynArr *primes = (struct DynArr *)malloc(sizeof(struct DynArr));
	initDynArr(primes, 10); 
	upperBound = atoi(argv[1]);
	printf("%d is bound\n", upperBound);
	startTime = time(NULL);
	for(i = 2; i <= upperBound; i++){
        	isPrimeDA(i, primes);
		if(i != 2) i++;
	}
	endTime = time(NULL);
	totTime = endTime - startTime;
	printf("%d\n", primes->size);
	printf("TIME: %ld:%ld\n", totTime/60, totTime - ((totTime/60)*60));
	free(primes->data);
	free(primes);
	return 0;
}
