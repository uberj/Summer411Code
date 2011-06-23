#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TAG_INIT 31337

int main(int argc, char *argv[])
{
	int i, j, ubound = 0, test, first, increment, kPrimeCount = 0, tPrimeCount = 0;
	int mpiRank = 0, mpiSize = 1;
	int namelen;
	double t;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
        MPI_Get_processor_name(processor_name, &namelen);
	
	fprintf(stderr, "Process %d started on host %s.\n", mpiRank, processor_name);

	if (!mpiRank){
		if (argc > 1){
        		ubound = atoi(argv[1]);
		}
                if (!ubound) {
			printf("Upper bound not supplied, terminating.\n");
		}
	}

	MPI_Bcast(&ubound, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (!ubound){
		MPI_Finalize();
		return 255;
	}

	first = (mpiRank * 2) + 3; // this will be the first number this particular process tests
        increment = mpiSize * 2; //  there is no need to test even numbers

        t = MPI_Wtime();

	for (i = first; i < ubound; i += increment){
		test = (int)sqrt((double)i);
		for(j = 2; j <= test; j++){
			if(i % j == 0){
				kPrimeCount--;
				break;
			}
		}
		if(i != ubound){
			kPrimeCount++;
		}
	}

	t = MPI_Wtime() - t;
	printf("[Host: %s] process %d completed computation in %f.\n", processor_name, mpiRank, t);

	if (!mpiRank){
		kPrimeCount++; //becaue we skip 2
		for(i=1;i<mpiSize;i++){
        		MPI_Recv(&tPrimeCount, 1, MPI_INT, i, TAG_INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//printf("Receive: %d\n",tPrimeCount);
			kPrimeCount += tPrimeCount;
		}
	}
	else{
		MPI_Send(&kPrimeCount, 1, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD);
		//printf("Send: %d\n", kPrimeCount);
	}
	
	if (!mpiRank){
		printf("All computation has finished.\n Found %d primes in the range 0 - %d.\nExiting.\n", kPrimeCount, ubound);
	}

	MPI_Finalize();
}

				





