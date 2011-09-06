#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

int NUM_THREADS;
int UBOUND; 

int kPrimeCount;
pthread_mutex_t mutexsum;

void *findPrime(void *param)
{
	int i, j, test, tid, isprime = TRUE;
	int runningCount = 0;
	tid = (int)param;

     	for(i = (2 * tid) + 3; i < UBOUND; i += (2 * NUM_THREADS)){
		test = (int)sqrt((double)i);
		for(j = 2; j <= test; j++){
			if(i % j == 0){
				isprime = FALSE;
				break;
			}
		}
		if(isprime)
			runningCount++;
		isprime = TRUE;
	}
	pthread_mutex_lock(&mutexsum);
	kPrimeCount += runningCount;
	pthread_mutex_unlock(&mutexsum);
	pthread_exit((void*) tid);
}

int main(int argc, char *argv[])
{
    	pthread_t thread[NUM_THREADS = sysconf(_SC_NPROCESSORS_ONLN)];
	pthread_attr_t attr;
	int rc, t;
	void *status;

	if(argc == 2) {
        	UBOUND = atoi(argv[1]);
	} else {
		printf("USAGE: %s [upper bound]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("Detected %d processors.\n", NUM_THREADS);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_mutex_init(&mutexsum, NULL);

	kPrimeCount = 1; // skipping the first prime

	for(t = 0; t < NUM_THREADS; t++){
		printf("Main: creating thread %d\n", t);
		rc = pthread_create(&thread[t], &attr, findPrime, (void *)t);
		if (rc) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(EXIT_FAILURE);
		}
	}

	/* Free attribute and wait for the worker threads */
	pthread_attr_destroy(&attr);
	for(t = 0; t < NUM_THREADS; t++){
		rc = pthread_join(thread[t], &status);
		if (rc) {
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
		printf("Main: completed join with thread %d having a status of %ld\n", t, (long)status);
	}
	
	printf("Main: program completed.\nFound %d primes in the range 0 - %d.\n Exiting.\n", kPrimeCount, UBOUND);
	pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
}

				





