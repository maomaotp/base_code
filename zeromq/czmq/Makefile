
all: worker.c client.c broker.c 
	gcc -o client client.c /usr/local/lib/libczmq.so;
	gcc -o server server.c /usr/local/lib/libczmq.so;
	gcc -o asyncsrv asyncsrv.c /usr/local/lib/libczmq.so;
