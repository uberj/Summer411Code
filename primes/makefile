default: prog
dynArray.o: dynArray.c dynArray.h
	gcc -O3 -march=atom -Wall -ansi -c dynArray.c
primes.o: primes.c  primes.h
	gcc -O3 -march=atom -Wall -ansi -c primes.c
prog: dynArray.o primes.o main.c
	gcc -O3 -march=atom -Wall -ansi -o prog dynArray.o primes.o main.c -lm
clean: 
	rm dynArray.o
	rm primes.o

cleanall: clean
	rm prog
