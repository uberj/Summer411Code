#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>
#include "CDLList.h"
#define NUM_THREADS 2
#define INITIAL 1000

struct Data{
	int tid;
	long upperBound, sVal;
	pthread_mutex_t *lock;
	struct List *list;
};

long isPrime(struct List *list, long tested){
	long sqroot;
	struct Node *place;
	
	sqroot = (long)sqrt((double)tested);
	
	place = list->head->next;
	while(place->value < sqroot && !(tested % place->value == 0)){
	        place = place->next;
	}
	
	if(tested % place->value == 0) return place->value;
	return tested;
}

void print_results(struct List *list){
	struct Node *place;
	place = list->head;
	
	while(place->next != list->tail){ 
		place = place->next;
		printf("%ld\n", place->value);
	}
	
	printf("size is %d\n", list->size);
}

void *isPrimePara(void *datapass){
	int i, div;
	struct Data *data = (struct Data *)datapass;

	for(i = 0; data->sVal + i < data->upperBound; i += (2 * NUM_THREADS)){
		if((div = isPrime(data->list, data->sVal + i)) == data->sVal + i){
			struct Node *nPrimes = (struct Node *)malloc(sizeof(struct Node));
			nPrimes->value = data->sVal + i;
			pthread_mutex_lock(data->lock);
			addList(data->list, nPrimes);
			pthread_mutex_unlock(data->lock);
		}
	}
	pthread_exit((void *)data->tid);
}

int main(int argc, char *argv[]){
	int upperBound, i, rc;
	struct List *list = (struct List *)malloc(sizeof(struct List));
	void *status;
	upperBound = atoi(argv[1]);
	
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	
	initList(list);
	
	struct Node *node = (struct Node *)malloc(sizeof(struct Node));
	node->value = 2;
	addList(list, node);
	
	
	for(i = 3; i <= INITIAL; i+=2){
		rc = isPrime(list, i);
		if(rc == i){
			struct Node *nPrime = (struct Node *)malloc(sizeof(struct Node));
			nPrime->value = i;
			addList(list, nPrime);
		}
	}
	
	for(i = 0; i < (2 * NUM_THREADS); i+=2){
		struct Data *data = (struct Data *)malloc(sizeof(struct Data));
		data->list = list;
		data->lock = &lock;
		data->tid = i;
		data->sVal = INITIAL + 1 + i;
		data->upperBound = upperBound;
		printf("main: %ld startval\n", data->sVal);
		pthread_create(&threads[i/2], &attr, isPrimePara, (void *)data);
	}
	
	for(i = 0; i < NUM_THREADS; i++){
		rc = pthread_join(threads[i], &status);
		if(rc){
			printf("bad return from thread %d", i);
		}
	}    

	printf("size = %d\n", list->size);
	return 1;
}
