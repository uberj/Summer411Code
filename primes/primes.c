#include "primes.h"

/*Matt Atwood 6/15/11
**returns number if it is a prime else returns first integer that
**divides the number in question into an integer.
*/
int isPrime (int tested){
	int i;
	for(i = 2; i < (int)sqrt((double)tested); i++){
		if(tested % i == 0) return i;
	}
	return tested;
}
               
/*Matt Atwood 6/15/2011
**returns number if prime, else returns first integer that
**divides the number, uses a dynArr of primes to speed up
**0 on fail.
*/
int isPrimeDA (int tested, struct DynArr *primes){
	int i, prime, div, size;
	double sqroot;
	size = primes->size;
	sqroot = (int)sqrt((double)tested);
	if(size > 0){
		for(i = 0; i < size; i++){
			if((prime = getDynArr(primes, i)) <= sqroot){
				if(tested % prime == 0) return prime;
			}
			else {
                       		addDynArr(primes, tested);
				return tested;
			}
		}		
	}
	else if((div = isPrime(tested)) == tested) addDynArr(primes, tested);
	return div;
}
