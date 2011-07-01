default: prime

prime.o: prime.c
	gcc -O3 -march=atom -ffast-math -c prime.c

prime: prime.o
	gcc -O3 -march=atom -ffast-math -o prime prime.o -pthread -lm

clean: 
	rm -f *.o prime
