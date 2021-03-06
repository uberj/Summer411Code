/*
 * Bounded prime numbers counter using the Sieve of Eratosthenes algorithm.
 * Uses openMP for multi-threading.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define CHUNK 1
#define COMPOSITE 1
#define PRIME 0 
#define ARRAY_TYPE int
#define INDEX_TYPE int

#define MASTER mpiRank == 0
#define SLAVE mpiRank != 0
#define TAG_INIT 31336

int main(int argc, char *argv[])
{
        INDEX_TYPE i = 0, n, c1, c2, c3;
	int temp, namelen, mpiRank = 0, mpiSize = 1;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
        const int finished = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
	MPI_Get_processor_name(processor_name, &namelen);

	printf("[Host %s] mpiRank = %d\n", processor_name, mpiRank);
        
	if(MASTER && argc < 2){
		printf("[Host %s] usage: %s bound\nExiting.", processor_name, argv[0]);
	} else {
		i = atoi(argv[1]);
	}

	MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
	
	if(!i){
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
  
        //create prime list
        ARRAY_TYPE *prime = (ARRAY_TYPE*)calloc(i+1,sizeof(ARRAY_TYPE));
        //ARRAY_TYPE *prime_final = (ARRAY_TYPE*)calloc(i+1,sizeof(ARRAY_TYPE));
        
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
 
 	if(SLAVE){
        	//find primes then eliminate their multiples (0 = prime, 1 = composite)
        	for(c2 = ((mpiRank - 1) * 2) + 3;c2 <= (int)sqrt(i)+1;c2 += (mpiSize - 1) * 2){
                	if(prime[c2] == PRIME){
                        	c1=c2;
                        	for(c3 = 2*c1; c3 <= i; c3 = c3+c1){
                                	prime[c3] = COMPOSITE;
					MPI_Send(&c3, 1, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD);
                        	}
                	}
        	}
		MPI_Send(&finished, 1, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD);
	} else {
        	while(mpiSize > 1){
			MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, TAG_INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(temp)
				prime[temp] = COMPOSITE;
			else
				mpiSize--;
		}
	}

	//allow all processes to finish
        MPI_Barrier(MPI_COMM_WORLD);

        //print primes
	if(MASTER){
		n = 0;
        	for(c1 = 0; c1 < i+1; c1++)
                	if(prime[c1] == PRIME) n++;
        	printf("[Host %s] Number of primes: %d\n", processor_name, n);
	}
	MPI_Finalize();
}
