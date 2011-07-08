default: matmulti

matmulti.o: matmulti.c
	gcc -O3 -c matmulti.c -fopenmp -lm

matmulti: matmulti.o
	gcc -O3 -o matmulti matmulti.o -fopenmp -lm

clean:
	rm -f *.o matmulti
