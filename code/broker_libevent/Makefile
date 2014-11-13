all: broker.c
	gcc -o event broker.c -levent -lhiredis -lxml2

clean:
	rm -rf *.o
