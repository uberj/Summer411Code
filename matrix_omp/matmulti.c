#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define TYPE short
#define PRINTMAX 10
#define CHUNK 1

void print_array(TYPE *A, int order, char label){
	int i, j;

	if(order > PRINTMAX)
		return;

	printf("printing matrix %c\n",label);

	for(i = 0; i < order; i++){
		for(j = 0; j < order; j++)
			printf("[%hd] ", A[i*order+j]);
		printf("\n");
	}
}

int main (int argc, char *argv[])
{
	TYPE * restrict A, * restrict B, * restrict C;
	int i, j, k, order;
	int tid;

	if(argc < 2){
		fprintf(stderr, "USAGE: %s <order>", argv[0]);
		exit(EXIT_FAILURE);
	}

        order = atoi(argv[1]);

	A = (TYPE*)calloc(order * order, sizeof(TYPE));
	B = (TYPE*)calloc(order * order, sizeof(TYPE));
	C = (TYPE*)calloc(order * order, sizeof(TYPE));
	if(!A || !B || !C){
		fprintf(stderr, "ERROR: Out of memory\n");
		exit(EXIT_FAILURE);
	}
        

	#pragma omp parallel shared(A,B,C,order) private(i,j,k,tid)
	{
		#pragma omp for
		for(i = 0; i < order * order; i++){
			A[i] = 7;
			B[i] = 7;
			C[i] = 0;
		}

	
		#pragma omp for
                for(i = 0; i < order; i++)
        		for(k = 0; k < order; k++)
				for(j = 0; j < order; j++)
					C[(i*order)+j] += A[(i*order)+k] * B[(k*order)+j];			
	}

        print_array(A, order, 'A');
	print_array(B, order, 'B');
	print_array(C, order, 'C');
}
