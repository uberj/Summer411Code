/*
 * Bounded prime numbers counter using the Sieve of Eratosthenes algorithm.
 * Uses openMP for multi-threading.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define CHUNK 1
#define COMPOSITE 't'
#define PRIME 'f' 
#define ARRAY_TYPE char
#define INDEX_TYPE int

int main(int argc, char *argv[])
{
        INDEX_TYPE i = 0, n, c1, c2, c3;
	int mpiRank = 0, mpiSize = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
        
	if(!mpiRank && argc < 2){
		printf("usage: %s bound\nExiting.", argv[0]);
	} else {
		i = atoi(argv[1]);
	}

	MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(!i){
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
  
        //create prime list
        ARRAY_TYPE *prime = (ARRAY_TYPE*)calloc(i+1,sizeof(ARRAY_TYPE));
        
        //fill all evens in list with composite
        for(c1 = 2; c1 <= i; c1 += 2)
                prime[c1] = COMPOSITE;
        
	//fill all odds in list with prime
	for(c1 = 3; c1 <= i; c1 += 2)
		prime[c1] = PRIME;
       
        //set 0 and 1 as not prime; 2 as prime
        prime[0]=COMPOSITE;
        prime[1]=COMPOSITE;
        prime[2]=PRIME;

        //find primes then eliminate their multiples (0 = prime, 1 = composite)
        for(c2 = (mpiRank * 2) + 3;c2 <= (int)sqrt(i)+1;c2 += mpiSize * 2){
                if(prime[c2] == PRIME){
                        c1=c2;
                        for(c3 = 2*c1;c3 <= i+1; c3 = c3+c1){
                                prime[c3] = COMPOSITE;
				MPI_Bcast(&(sieve[c3]), 1, MPI_INT, mpiRank, MPI_COMM_WORLD);
                        }
                }
        }
        
        //print primes
	if(!mpiRank){
		n = 0;
        	for(c1 = 0; c1 < i+1; c1++)
                	if(prime[c1] == PRIME) n++;
        	printf("Number of primes: %d\n", n);
	}
	MPI_Finalize();
}
