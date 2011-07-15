/*
 * Bounded prime numbers counter using the Sieve of Eratosthenes algorithm.
 * Uses MPI for parallel processing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define MIN(a,b) ((a)<(b)?(a):(b))

#define COMPOSITE 1
#define PRIME 0 
#define ARRAY_TYPE char

#define MASTER(id) (id == 0)
#define SLAVE(id) (id != 0)

#define BLOCK_LOW(id,p,n) ((id) * (n) / (p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id) + 1, p, n) - 1)
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n) - BLOCK_LOW(id,p,n))
#define BLOCK_OWNER(index,p,n) (((p)*((index)+1)-1)/(n))

int main(int argc, char *argv[])
{
        int i, index, prime, first, low_value, high_value, proc0_size, count, global_count, size, n = 0;
	int temp, namelen, id, p;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
        
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Get_processor_name(processor_name, &namelen);
        
	if (MASTER(id) && argc < 2)
		printf("[Host %s] usage: %s bound\nExiting.", processor_name, argv[0]);
	else if (MASTER(id)) {
		n = atoi(argv[1]);
	}

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
	
	if (!n) {
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}

        low_value = 2 + BLOCK_LOW(id,p,n-1);
	high_value = 2 + BLOCK_HIGH(id,p,n-1);
	size = BLOCK_SIZE(id,p,n-1);

	proc0_size = (n-1)/p;

	if ((2 + proc0_size) < (int)sqrt((double) n)){
		 if (MASTER(id)) printf("[Host %s] Too many processes.\n", processor_name);
		 MPI_Finalize();
		 exit(1);
	}
        fprintf(stderr, "[Host %s] Allocating a %d element char array...", processor_name, size);
        //create prime list
        ARRAY_TYPE *marked = (ARRAY_TYPE*)calloc(size,sizeof(ARRAY_TYPE));
        if (marked == NULL) {
		printf("[Host %s] Cannot allocate enough memory.\n", processor_name);
		MPI_Finalize();
		exit(1);
	}
	fprintf(stderr, "finished\n"); 

	//fill out list with 0
	for (i = 0; i < size; i++)
		marked[i] = PRIME;
       
        if (MASTER(id))
		index = 0;
	prime = 2;
        fprintf(stderr, "[Host %s] starting.\n", processor_name);
 	do {
		if (prime * prime > low_value)
			first = prime * prime - low_value;
		else {
			if (!(low_value % prime))
				first = 0;
			else
				first = prime - (low_value % prime);
		}
		for (i = first; i < size; i+= prime)
			marked[i] = COMPOSITE; 
		if (MASTER(id)) {
			while(marked[++index]);
			prime = index + 2;
		}
		MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
	} while (prime * prime <= n);
	count = 0;
	for (i = 0; i < size; i++)
		if (!marked[i])
			count++;
	fprintf(stderr, "[Host %s] finished.\n", processor_name);
	MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


	//allow all processes to finish
        MPI_Barrier(MPI_COMM_WORLD);

        //print primes
	if(MASTER(id)){
        	printf("[Host %s] Number of primes: %d\n", processor_name, global_count);
	}
	MPI_Finalize();
}
