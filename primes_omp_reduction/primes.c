#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

int main (int argc, char *argv[])
{
	int i, j, test, ubound, runningcount = 0, isprime = TRUE;

	if(argc < 2){
		printf("USAGE: %s <upper bound>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

        ubound = atoi(argv[1]);

	#pragma omp parallel for         \
		default(none)		 \
		private(i,j,test,isprime)\
		shared(ubound)		 \
		schedule(dynamic,1)	 \
		reduction(+:runningcount)
		for(i = 3; i < ubound; i += 2 ){
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
	runningcount++;
	printf("Found %d primes in the range 1 - %d\n", runningcount, ubound);
}


