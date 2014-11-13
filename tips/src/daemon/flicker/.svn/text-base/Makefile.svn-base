#
# command:
# 	- make: to build the Driver program.
#	- make clean: to clean.
#
CC = gcc
all: n6601

n6601:example_main.o n6601.o libich8.a
	$(CC) -o n6601 n6601.o example_main.o libich8.a

n6601.o: n6601.c n6601.h
	$(CC) -c n6601.c

example_main.o: example_main.c
	$(CC) -c example_main.c 

.PHONY: clean
clean:
	@rm n6601 *.o

