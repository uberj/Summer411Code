default: omp_workshare1 omp_workshare2

omp_workshare2.o: omp_workshare2.c
	gcc -c omp_workshare2.c -fopenmp

omp_workshare2: omp_workshare2.o
	gcc -o omp_workshare2 omp_workshare2.o -fopenmp

omp_workshare1.o: omp_workshare1.c
	gcc -c omp_workshare1.c -fopenmp

omp_workshare1: omp_workshare1.o
	gcc -o omp_workshare1 omp_workshare1.o -fopenmp

clean:
	rm -f *.o omp_workshare1 omp_workshare2
