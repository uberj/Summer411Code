#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct Data{
	int lock;
	int *a;
	int *b;
	int *ab;
	int sRow;
	int nRow;
	int dim;
	int tid;
};

void matMulti(void *datapass){
	int i, j, k, t, s, dim, rows;
	struct Data *data = (struct Data *)datapass;
	i = data->sRow;
	//printf("thread started, sRow = %d, nRow = %d\n", data->sRow, data->nRow);
	rows = data->nRow; 
        dim = data->dim;
	t = data->tid;
	data->lock = 0;
	s = i;
	for(; i < s + rows; i++){
                for(j = 0; j < dim; j++){
                        for(k = 0; k < dim; k++){
                               	data->ab[(i * dim) + j] += (data->a[(i * dim) + k] * data->b[(k * dim) + j]);
				//printf("[%d] altering position %d\n", t, i*dim + j);
			}
                }
        }
	//printf("thread done\n");
	pthread_exit((void *)t);
}


void print_array(int *a, int row, int columns){	
        int i, j;
	printf("Printing Matrix: \n");
        for(i = 0; i < row; i++){
                for(j = 0; j < columns; j++){
                        printf("[%d] ", a[(i * columns)+ j]);
                }
                printf("\n");
        }
}

int main(int argc, char *argv[]){
	int i, j, k, dim, nthreads; 
	dim = atoi(argv[1]);
	nthreads = atoi(argv[2]);
	pthread_t threads[nthreads];
	struct Data *data = (struct Data *)malloc(sizeof(struct Data));
	void *status;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if(dim < 0 || nthreads < 0) return -1;	
        
	int a[dim * dim], b[dim * dim], ab[dim * dim];
	for(i = 0; i < dim * dim; i++){
		a[i] = rand() % 10;
	}
	//print_array(a, dim, dim);
	for(i = 0; i < dim * dim; i++){
                b[i] = rand() % 10;
        }
	//print_array(b, dim, dim);            
        for(i = 0; i < dim * dim; i++){
        	ab[i] = 0;
	}
	//print_array(ab, dim, dim);

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
	for(i = 0; i < nthreads; i++){
        	k = pthread_join(threads[i], &status);
		if(k) printf("derp\n");
		//printf("%ld\n", (long)status);
	}
	//print_array(data->ab, dim, dim);
	return 1;
}

/*
void matMult(void *datapass){
	int i, j, k, dim, row, counter;
	struct Data data = (struct Data *)datapass;
	row = data->sRow;
	counter = data->nRow;
	dim = data->dim
	data->lock = 0;
	
	for(i = 0; i < counter; i++){
        	for(j = 0; j < dim; j++){
                	for(k = 0; k < dim; k++){
                        	data->ab[row + i][j] += (data->a[row + i][k] * data->b[k][j]);
			}
		}
	} 

} */
