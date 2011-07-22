CC = gcc
CFLAGS = -g -Wall -lm

objects = main.c read.c big_num.c tests.c

# This is the default rule
bn: $(objects)
	$(CC) $(CFLAGS) $(objects) -o run

clean:
	rm -f *.o
	rm -f run
