default: primes

primes.o: primes.c
	gcc -O3 -march=atom -c primes.c -fopenmp -lm

primes: primes.o
	gcc -O3 -march=atom -o primes primes.o -fopenmp -lm

clean: 
	rm -f *.o primes
