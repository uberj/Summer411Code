#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
 
int main(int argc, char *argv[])
{
        int i = atoi(argv[1]), n;
  
        //create prime list
        int *prime = (int*)calloc(i,sizeof(int));
        int c1, c2, c3;
       
        //fill list with 0 - prime
        for(c1 = 2; c1 <= i; c1++){
                prime[c1] = 0;
        }
       
        //set 0 and 1 as not prime
        prime[0]=1;
        prime[1]=1;
       
        //find primes then eliminate their multiples (0 = prime, 1 = composite)
	#pragma omp parallel for private(c1,c2,c3) schedule(dynamic,1)
        for(c2 = 2;c2 <= (int)sqrt(i)+1;c2++){
                if(prime[c2] == 0){
                        c1=c2;
                        for(c3 = 2*c1;c3 <= i+1; c3 = c3+c1){
                                prime[c3] = 1;
                        }
                }
        }
       
        //print primes
	n = 0;
	#pragma omp parallel for private(c1) reduction(+:n) schedule(dynamic,1)
        for(c1 = 0; c1 < i+1; c1++){
                if(prime[c1] == 0) n++;
        }
        printf("Number of primes: %d\n", n);
       
return 0;
}
