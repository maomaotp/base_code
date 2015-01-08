
all: pull.c push.c 
	gcc -o push push.c /usr/local/lib/libzmq.so;
	gcc -o pull pull.c /usr/local/lib/libzmq.so;
clean:
	rm -rf  *.o server recv.data
