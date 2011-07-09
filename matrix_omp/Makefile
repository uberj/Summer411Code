default: matmulti

matmulti.o: matmulti.c
	gcc -O3 -c matmulti.c -fopenmp

matmulti: matmulti.o
	gcc -O3 -o matmulti matmulti.o -fopenmp

clean:
	rm -f *.o matmulti
