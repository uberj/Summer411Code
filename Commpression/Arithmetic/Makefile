CC = gcc
CFLAGS = -g -Wall

objects = main.o model.o  

# This is the default rule
arith.a: $(objects)
	ar -r arith.a $(objects)
	$(CC) $(CFLAGS) -o run arith.a


comp: arith.a
	./run -c data.txt
	@echo
	cat data.txt.acp

test: arith.a
	./run -c data.txt
	cat data.txt.acpi?
	./run -x data.txt.acpi

clean:
	rm -f *.o
	rm -f arith.a
	rm -f run
	rm data.txt.*
