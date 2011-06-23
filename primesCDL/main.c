#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "CDLList.h"
#define NUM_THREADS 4
#define INITIAL 1000

struct Data{
	int lock;
	int sVal;
	int upperBound;
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

void isPrimePara(void *datapass){
	int i, div, start;
	struct Data *data = (struct Data *)datapass;
	start = data->sVal;
	data->lock = 0;
	for(i = 0; start + i < data->upperBound; i += (2 * NUM_THREADS)){
        	if((div = isPrime(data->list, start + i)) == start + i){
                	struct Node *nPrimes = (struct Node *)malloc(sizeof(struct Node));
			nPrimes->value = start + i;
			addList(data->list, nPrimes);
		}

	}
}
int main(int argc, char *argv[]){
	int upperBound, init, flag, r, counter;
	long i, rc;
	time_t startTime, endTime, totTime;
	struct Data *data = (struct Data *)malloc(sizeof(struct Data));
	void *status;
	pthread_t threads[NUM_THREADS];
	data->list = (struct List *)malloc(sizeof(struct List));
	initList(data->list);
	data->lock = 0;
	data->sVal = 0;
	upperBound = atoi(argv[1]);
	data->upperBound = upperBound;
	struct Node *node = (struct Node *)malloc(sizeof(struct Node));
	node->value = 2;
	addList(data->list, node);
	flag = 0;
	if(upperBound <= INITIAL){
		flag = 1;
		init = upperBound;
	}	
	for(i = 3; i <= INITIAL; i+=2){
		rc = isPrime(data->list, i);
		if(rc == i){
			struct Node *nPrime = (struct Node *)malloc(sizeof(struct Node));
			nPrime->value = i;
			addList(data->list, nPrime);
		}
	}
	
	for(i = 0; i < (2 * NUM_THREADS); i+=2){
		while(data->lock == 1) ;
		data->lock = 1;
		data->sVal = INITIAL + 1 + i;
		pthread_create(&threads[i/2], NULL, isPrimePara, (void *)data);
	}
	for(i = 0; i < NUM_THREADS; i++){
		r = pthread_join(threads[i], &status);
		if(r){
			printf("bad return from thread %ld", i);
		}
	}           
	/*counter = 0;
	struct Node *place = data->list->head->next;
	while(place != data->list->tail){
        	if((i = isPrime(data->list, place->value)) != place->value || place->value == place->prev->value){
			counter++;
		}
		place = place->next;
        }                 
	printf("size = %d\n, exceptions = %d\n", data->list->size, counter);
	*/
	printf("size = %d\n", data->list->size);
	return 1;
}
