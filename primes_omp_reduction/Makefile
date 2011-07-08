default: primes

primes.o: primes.c
	gcc -c primes.c -fopenmp -lm

primes: primes.o
	gcc -o primes primes.o -fopenmp -lm

clean: 
	rm -f *.o primes
