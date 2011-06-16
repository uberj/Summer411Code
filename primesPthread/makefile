default: prog
dynArray.o: dynArray.c dynArray.h
	gcc -Wall -ansi -c dynArray.c
primes.o: primes.c  primes.h
	gcc -Wall -ansi -c primes.c
prog: dynArray.o primes.o main.c
	gcc -Wall -ansi -o prog dynArray.o primes.o main.c -lm
clean: 
	rm dynArray.o
	rm primes.o

cleanall: clean
	rm prog
