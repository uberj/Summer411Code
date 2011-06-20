#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "dynArray.h"

struct Data {
	int lock;
	int tested;
	struct DynArr *primes;
};

int isPrime(int tested);
int isPrimeDA(int tested, struct DynArr *primes);
void isPrimeDAPara(void *data);
