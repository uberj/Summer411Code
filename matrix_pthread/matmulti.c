#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

/* Matt Atwood 6/24/11
 * This program takes 3 command line arguments N C P
 * The program generates 2 NxN matrices spawns C number
 * of threads to achieve the task and if P is not 0 it
 * prints the 2 randomly generated matrices, the initial
 * product matrix and the final product matrix.
 */

struct Data{
	int lock;
	short *a;
	short *b;
	short *ab;
	int sRow;
	int nRow;
	int dim;
	int tid;
};

/* Matt Atwood 6/24/11
 * This function is to be called by a pthread
 * Pre: datapass not null, pointers in datapass
 * not null,
 * Post: the values from sRow to sRow+nRow are filled
 * into the product matrix
 */
void matMulti(void *datapass){
	int i, j, k, t, start, dim, rows;
	assert(datapass != NULL);
	assert(data->a != NULL);
	assert(data->b != NULL);
	assert(data->ab != NULL);
	struct Data *data = (struct Data *)datapass;
	i = data->sRow;
	rows = data->nRow; 
        dim = data->dim;
	t = data->tid;
	data->lock = 0;
	start = i;
	for(; i < start + rows; i++){
                for(j = 0; j < dim; j++){
                        for(k = 0; k < dim; k++){
                               	data->ab[(i * dim) + j] += (data->a[(i * dim) + k] * data->b[(k * dim) + j]);
			}
                }
        }
	pthread_exit((void *)t);
}

/* Matt Atwood 6/24/11
 * This function prints out an array
 * Pre: a is not null
 * Post: user is displayed the array as a 2D array
 * corresponding to number of rows and columns passed
 */
void print_array(int *a, int row, int columns){	
        int i, j;
	assert(a != null);
	printf("Printing Matrix: \n");
        for(i = 0; i < row; i++){
                for(j = 0; j < columns; j++){
                        printf("[%d] ", a[(i * columns)+ j]);
                }
                printf("\n");
        }
}

int main(int argc, char *argv[]){
	int i, j, k, dim, print, nthreads;
	if(argc != 3){ 
		printf("bad use pMult N C P\n");
		return -1; 
	}
	dim = atoi(argv[1]);
	nthreads = atoi(argv[2]);
	print = atoi(argv[3]);
	pthread_t threads[nthreads];
	struct Data *data = (struct Data *)malloc(sizeof(struct Data));
	void *status;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if(dim < 0 || nthreads < 0){
		printf("bad use pMult N & C > 0\n");	
		return -1;	
        }
	/* Create Matrices */
	short a[dim * dim], b[dim * dim], ab[dim * dim];
	for(i = 0; i < dim * dim; i++){
		a[i] = rand() % 2;
	}
	if(print) print_array(a, dim, dim);
	for(i = 0; i < dim * dim; i++){
                b[i] = rand() % 2;
        }
	if(print) print_array(b, dim, dim);            
        for(i = 0; i < dim * dim; i++){
        	ab[i] = 0;
	}
	if(print) print_array(ab, dim, dim);

	/* Setup to create threads */
	data->a = a;
	data->b = b;
	data->ab = ab;
	data->lock = 0;
        data->dim = dim;
	j = 0;
	for(i = 0; i < nthreads; i++){
		while(data->lock > 0) ;
		data->lock = 1;
		data->sRow = j;
		data->tid = i;
		data->nRow = (dim/nthreads) + (dim % nthreads > i);
        	pthread_create(&threads[i], &attr, matMulti, (void *)data);
		j = j + data->nRow; 	
        }
	
	/* Catch threads before executing */
	for(i = 0; i < nthreads; i++){
        	k = pthread_join(threads[i], &status);
		if(k) printf("Error on pthread_join %ld");
	}
	if(print) print_array(data->ab, dim, dim);
	
	return 1;
}
