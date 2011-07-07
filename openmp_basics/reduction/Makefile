default: reduction

reduction.o: reduction.c
	gcc -c reduction.c -fopenmp

reduction: reduction.o
	gcc -o reduction reduction.o -fopenmp

clean:
	rm -f *.o reduction
