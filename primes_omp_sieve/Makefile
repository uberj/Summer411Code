default: primes

primes.o: primes.c
	gcc -O3 -c primes.c -fopenmp -lm

primes: primes.o
	gcc -O3 -o primes primes.o -fopenmp -lm

clean:
	rm -f *.o primes
