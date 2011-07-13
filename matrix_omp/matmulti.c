#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define TYPE short
#define PRINTMAX 10
#define CHUNK 1

void print_array(TYPE **A, int order, char label){
	int i, j;

	if(order > PRINTMAX)
		return;

	printf("printing matrix %c\n",label);

	for(i = 0; i < order; i++){
		for(j = 0; j < order; j++)
			printf("[%hd] ", A[i][j]);
		printf("\n");
	}
}

int main (int argc, char *argv[])
{
	TYPE **A, **B, **C;
	int i, j, k, order;
	int tid;

	if(argc < 2){
		fprintf(stderr, "USAGE: %s <order>", argv[0]);
		exit(EXIT_FAILURE);
	}

        order = atoi(argv[1]);

	A = (TYPE**)calloc(order, sizeof(TYPE*));
	B = (TYPE**)calloc(order, sizeof(TYPE*));
	C = (TYPE**)calloc(order, sizeof(TYPE*));
	if(!A || !B || !C){
		fprintf(stderr, "ERROR: Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < order; i++){
		A[i] = (TYPE*)calloc(order, sizeof(TYPE*));
		B[i] = (TYPE*)calloc(order, sizeof(TYPE*));
		C[i] = (TYPE*)calloc(order, sizeof(TYPE*));
		if(!A[i] || !B[i] || !C[i]){
			fprintf(stderr, "ERROR: Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
        
	#pragma omp parallel shared(A,B,C,order) private(i,j,k,tid)
	{
		tid = omp_get_thread_num();

		if(!tid)
			printf("Using %d threads on %d processors...\n", omp_get_num_threads(), omp_get_num_procs());
		#pragma omp for schedule(dynamic,CHUNK)
        	for(i = 0; i < order; i++){
			if(order <= PRINTMAX)
				printf("[thread %d] initializing row %d\n", tid, i); 
			for(j = 0; j < order; j++){
                		A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
				C[i][j] = 0;
			}
		}
	
		#pragma omp for schedule(dynamic,CHUNK)
                for(i = 0; i < order; i++){
                        if(order <= PRINTMAX)
				printf("[thread % d] calculating row %d\n", tid, i);
        		for(j = 0; j < order; j++)
				for(k = 0; k < order; k++)
					C[i][j] += A[i][k] * B[k][j];
		}
	}

        print_array(A, order, 'A');
	print_array(B, order, 'B');
	print_array(C, order, 'C');
}