
all: broker.c client.c worker.c 
	gcc -o broker broker.c /usr/local/lib/libzmq.so;
	gcc -o client client.c /usr/local/lib/libzmq.so;
	gcc -o worker worker.c /usr/local/lib/libzmq.so;
