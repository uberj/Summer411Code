/*
 * Bounded prime numbers counter using the Sieve of Eratosthenes algorithm.
 * Uses MPI for parallel processing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define COMPOSITE 1
#define PRIME 0 
#define ARRAY_TYPE int

#define MASTER mpiRank == 0
#define SLAVE mpiRank != 0

int main(int argc, char *argv[])
{
        int i, index, prime, first, low_value, count, global_count, size, n = 0;
	int temp, namelen, mpiRank = 0, mpiSize = 1;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
        
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
	MPI_Get_processor_name(processor_name, &namelen);
        
	if (MASTER && argc < 2)
		printf("[Host %s] usage: %s bound\nExiting.", processor_name, argv[0]);
	else if (MASTER) {
		n = atoi(argv[1]);
	}

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
	
	if (!n) {
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
        size = n / mpiSize;
	low_value = mpiRank + 2;
        //create prime list
        ARRAY_TYPE *marked = (ARRAY_TYPE*)calloc(size,sizeof(ARRAY_TYPE));
        
	//fill out list with 0
	for (i = 0; i < size; i++)
		marked[i] = PRIME;
       
        if (MASTER)
		index = 0;
	prime = 2;
 
 	do {
		if (prime * prime > low_value)
			first = prime * prime - low_value;
		else {
			if (!(low_value % prime))
				first = 0;
			else
				first = prime - (low_value % prime);
		}
		fprintf(stderr, "[Host %s] sieving %d\n", processor_name, first);
		for (i = first; i < size; i+= prime)
			marked[i] = COMPOSITE; 
		if (MASTER) {
			while(marked[++index]);
			prime = index + 2;
		}
		MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
	} while (prime * prime <= n);
	count = 0;
	for (i = 0; i < size; i++)
		if (!marked[i])
			count++;
	MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


	//allow all processes to finish
        MPI_Barrier(MPI_COMM_WORLD);

        //print primes
	if(MASTER){
        	printf("[Host %s] Number of primes: %d\n", processor_name, global_count);
	}
	MPI_Finalize();
}
