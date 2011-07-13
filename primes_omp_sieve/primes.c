#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define CHUNK 1
#define COMPOSITE 't'
#define PRIME 'f' 
#define ARRAY_TYPE char
#define INDEX_TYPE int

int main(int argc, char *argv[])
{
        INDEX_TYPE i, n, c1, c2, c3;
        
	if(argc < 2){
		printf("usage: %s bound\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	i = atoi(argv[1]); 
  
        //create prime list
        ARRAY_TYPE *prime = (ARRAY_TYPE*)calloc(i+1,sizeof(ARRAY_TYPE));
        
        //fill all evens in list with composite
        for(c1 = 2; c1 <= i; c1 += 2)
                prime[c1] = COMPOSITE;
        
	//fill all odds in list with prime
	for(c1 = 3; c1 <= i; c1 += 2)
		prime[c1] = PRIME;
       
        //set 0 and 1 as not prime
        prime[0]=COMPOSITE;
        prime[1]=COMPOSITE;
        prime[2]=PRIME;

        //find primes then eliminate their multiples (0 = prime, 1 = composite)
	#pragma omp parallel for private(c1,c2,c3) schedule(dynamic,CHUNK)
        for(c2 = 3;c2 <= (int)sqrt(i)+1;c2 += 2){
                if(prime[c2] == PRIME){
                        c1=c2;
                        for(c3 = 2*c1;c3 <= i+1; c3 = c3+c1){
                                prime[c3] = COMPOSITE;
                        }
                }
        }
        
        //print primes
	n = 0;
	#pragma omp parallel for private(c1) reduction(+:n) schedule(dynamic,CHUNK)
        for(c1 = 0; c1 < i+1; c1++){
                if(prime[c1] == PRIME) n++;
        }
        fprintf(stderr, "Number of primes: %d\n", n);
}
