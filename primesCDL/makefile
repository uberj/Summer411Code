default: prog

CDLList.o: CDLList.c CDLList.h
	gcc -Wall -ansi -c CDLList.c
prog: CDLList.o main.c
	gcc -Wall -ansi -o prog CDLList.o main.c -lm -lpthread

clean: 
	rm CDLList.o

cleanall: clean
	rm prog
