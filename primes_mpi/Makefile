default: prime

prime.o:
	gcc -c prime.c

prime: prime.o
	gcc -o prime prime.o -pthread -lm

clean: 
	rm -f *.o prime
