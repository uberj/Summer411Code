#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TAG_INIT 31337

struct pParam {
	int tid, tsize;
	int bound;
	int mrank, msize;
	int *primecount;
	pthread_mutex_t *lock;
};

void *findPrime(void *param)
{
	int i, j, test, start, increment, runningcount = 0;
	struct pParam *p = (struct pParam *)param;

	start = (((p->tid * p->msize) + p->mrank) * 2) + 3;
	increment = (p->tsize * p->msize) * 2;

	printf("|| pThread %d starting at %d incrementing by %d.\n", p->tid, start, increment);
	for(i = start; i < p->bound; i += increment){
        	test = (int)sqrt((double)i);
		for(j = 2; j <= test; j++){
			if(i % j == 0){
				runningcount--;
				break;
			}
		}
		if(i != p->bound){
			runningcount++;
		}
	}
	pthread_mutex_lock(p->lock);
	*(p->primecount) += runningcount;
	pthread_mutex_unlock(p->lock);
	pthread_exit((void *) p->tid);
}


int main(int argc, char *argv[])
{
	int i, ubound = 0, tPrimeCount = 0, FinalPrimeCount = 0, tSize = atoi(argv[1]);

	// pthread data
	pthread_t thread[tSize];
	struct pParam *param[tSize];
	pthread_attr_t attr;
	pthread_mutex_t lock;
	long rc;
	void *status;

	// mpi data
	int mpiRank = 0, mpiSize = 1;
	int namelen;
	double time;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
        MPI_Get_processor_name(processor_name, &namelen);
	
	fprintf(stderr, "Process %d started on host %s.\n", mpiRank, processor_name);

	if (!mpiRank){
		if (argc > 2){
        		ubound = atoi(argv[2]);
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

	//first = (mpiRank * 2) + 3; // this will be the first number this particular process tests
        //increment = mpiSize * 2; //  there is no need to test even numbers

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&lock, NULL);


        time = MPI_Wtime();

        for (i = 0; i < tSize; i++) {
		printf("[%s] creating pThread %d.\n", processor_name, i);
		param[i] = (struct pParam *)malloc(sizeof(struct pParam));
		param[i]->tid = i;
		param[i]->tsize = tSize;
		param[i]->bound = ubound;
                param[i]->mrank = mpiRank;
		param[i]->msize = mpiSize;
		param[i]->lock = &lock;
		param[i]->primecount = &tPrimeCount;
		rc = pthread_create(&thread[i], &attr, findPrime, (void *)(param[i]));
		//TODO check for errors
	}

	pthread_attr_destroy(&attr);
	for (i = 0; i < tSize; i++) {
        	rc = pthread_join(thread[i], &status);
		//TODO check for errors
		printf("[%s] completed join with pThread %d.\n", processor_name, i);
	}
	pthread_mutex_destroy(&lock);

	time = MPI_Wtime() - time;
	printf("[Host: %s] process %d completed computation in %f.\n", processor_name, mpiRank, time);


	if (!mpiRank){
		tPrimeCount++; //because we skip 2
		for(i=1;i<mpiSize;i++){
        		MPI_Recv(&tPrimeCount, 1, MPI_INT, i, TAG_INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			FinalPrimeCount += tPrimeCount;
		}
	}
	else{
		MPI_Send(&tPrimeCount, 1, MPI_INT, 0, TAG_INIT, MPI_COMM_WORLD);
	}
	
	if (!mpiRank){
		printf("All computation has finished.\n Found %d primes in the range 0 - %d.\nExiting.\n", FinalPrimeCount, ubound);
	}

	MPI_Finalize();
	pthread_exit(NULL);
}

				





