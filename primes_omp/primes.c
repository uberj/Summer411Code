#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

int main (int argc, char *argv[])
{
	int i, j, test, ubound, runningcount = 0, isprime = TRUE;
        int nthreads, tid, chunk = 1;
	if(argc < 2){
		printf("USAGE: %s <upper bound>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

        ubound = atoi(argv[1]);

	#pragma omp parallel shared(runningcount, nthreads, chunk) private(isprime, i, j, test, tid)
	{
		tid = omp_get_thread_num();
		if(tid == 0){
			nthreads = omp_get_num_threads();
			printf("Number of threads = %d\n", nthreads);
		}
		printf("Thread %d starting...\n",tid);

		#pragma omp for schedule(dynamic, chunk)
		for(i = 3; i < ubound; i++){
			test = (int)sqrt((double)i);
			for(j = 2; j <= test; j++){
				if(i % j == 0){
					isprime = FALSE;
					break;
				}
			}
			if(isprime)
				runningcount++;
			isprime = TRUE;
		}
	}
	runningcount++;
	printf("Found %d primes in the range 1 - %d\n", runningcount, ubound);
}


