CC = gcc
CFLAGS = -g -Wall

objects = model.o main.o

# This is the default rule
arith.a: $(objects)
	ar -r arith.a $(objects)
	$(CC) $(CFLAGS) -o run arith.a


test: heaplib.a heaptest.c
	$(CC) $(CFLAGS) -o run arith.a
	./run

clean:
	rm -f *.o
	rm -f arith.a
	rm -f run
