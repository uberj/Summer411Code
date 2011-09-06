default: matmulti

matmulti.o: matmulti.c
	gcc -O3 -ftree-vectorizer-verbose=7 -std=c99 -c matmulti.c -fopenmp

matmulti: matmulti.o
	gcc -O3 -ftree-vectorizer-verbose=7 -std=c99 -o matmulti matmulti.o -fopenmp

clean:
	rm -f *.o matmulti
