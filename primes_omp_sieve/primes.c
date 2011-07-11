/* This code is in public domain. Use for whatever purpose at your own risk. */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
 
uint32_t *sieve;
int MAXN;	/* maximum value of N */
int P1;		/* = ceil(MAXN/64) */
int P2;		/* = ceil(MAXN/2) */
int P3;		/* = ceil(ceil(sqrt(MAXN))/2) */
 
#define GET(b) ((sieve[(b)>>5]>>((b)&31))&1)
#define chunk 1
 
int isprime(int p) { return p==2 || (p>2 && (p&1)==1 && (GET((p-1)>>1)==0)); }
 
int main(int argc, char *argv[])
{
	uint32_t i, j, k; 
	int p, n;
	
	if(argc < 2){
		printf("USAGE: %s <upperbound>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	MAXN = atoi(argv[1]);
	P1 = ceil(MAXN/64);
	P2 = ceil(MAXN/2);
	P3 = ceil(ceil(sqrt(MAXN))/2);

	sieve = (int *)calloc(MAXN, sizeof(int));
	memset(sieve, 0, sizeof(sieve));

	#pragma omp parallel shared(sieve) private(i,j,k,p)
	{
		#pragma omp for schedule(dynamic,chunk)
    		for (k = 1; k <= P3; k++)
        		if (GET(k)==0) 
				for(j=2*k+1,i=2*k*(k+1);i<P2;i+=j) 
					sieve[i>>5]|=1<<(i&31);
		n = 0;
		#pragma omp for schedule(dynamic,chunk) reduction(+:n)
		for (p = 0; p <= MAXN; p++)
        		if (isprime(p)) n++;
	
	}
	printf("The number of primes below %d is %d.\n", MAXN, n);
    	return 0;
}
