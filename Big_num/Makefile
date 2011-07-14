CC = gcc
CFLAGS = -g -Wall -lm

objects = main.c 

# This is the default rule
bn: $(objects)
	$(CC) $(CFLAGS) $(objects) -o run 

clean:
	rm -f *.o
	rm -f run
