default: hello

hello.o: hello.c
	gcc -c hello.c -fopenmp

hello: hello.o
	gcc -o hello hello.o -fopenmp

clean:
	rm -f *.o hello
