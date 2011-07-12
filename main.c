#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

struct Data{
	long tid;
	long start;
	long prime;
	long rangeCount;
	char *array;
};

void print_array(long *array, long rangeCount){
	long i;
	printf("element is 2\n");
	for(i = 1; i < rangeCount; i++){
		if(array[i] != '0') printf("element is %ld\n", i * 2 + 1);
	}
}

void *removeWithFactor(void *datapass){
	long i;
	struct Data *data = (struct Data *)datapass;
	i = data->start + data->prime;
	for(; i < data->rangeCount; i+= data->prime){
		//printf("removing %ld has factor %ld\n", i * 2 + 1, data->prime);
		data->array[i] = '0';
	}
	return (void *)data->tid;
}

int main(int argc, char *argv[]){
	int ret;
	long test, sqroot, i, j, size, rangeCount, uBound, place, tid;
	void *status;
	
	if((uBound = atoi(argv[1])) % 2 == 0) rangeCount = uBound / 2;
	else rangeCount = (uBound / 2) + 1;
	char *range = (char *)malloc(sizeof(char) * rangeCount);
	
	pthread_t threads[(long)(sqrt(uBound) / 2)];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	
	/* fill the arrays with odds that could be primes */
	/*for(i = 0; i < rangeCount; i++){
		range[i] = '1';
	}
        */
	memset(range, '1', rangeCount);
	/* look for primes */
	tid = 0;
	for(i = 1; i < rangeCount; i++){
		j = 1;
		test = i * 2 + 1;
		//printf("test is %ld sqrt is %ld\n", test, (long)sqrt(uBound));
		if(test > sqrt(uBound)) break;
		if(range[i] == '0') continue;
		
		sqroot = sqrt(test);
		
		place = j * 2 + 1;
		   
		while(place <= sqroot && !(test % place == 0)) {
			j++;
			place = j * 2 + 1;
		}
		//printf("%ld mod %ld = %ld\n", test, place, test % place);
		if(test % place != 0 || test == place){
			//printf("found prime %ld\n", test);
			struct Data *data = (struct Data *)malloc(sizeof(struct Data));
			data->tid = tid;
			data->start = i;
			data->prime = test;
			data->rangeCount = rangeCount;
			data->array = range;
			pthread_create(&threads[data->tid], &attr, removeWithFactor, (void *)data); 
			tid++;
		}

	}
	
	for(i = 0; i < tid; i++){
		ret = pthread_join(threads[i], &status);
		if(ret){
			printf("bad return on thread %ld\n", i);
		}
	}

	

	size = 1;
	for(i = 1; i < rangeCount; i++){
		if(range[i] != '0'){
			size++;
			//printf("element is %ld\n", i * 2 + 1);
		}
	}
 
	printf("uBound is %ld, Size is %ld\n", uBound, size);
	return 1;
}
