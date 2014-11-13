all: trojan.c
	gcc -shared trojan.c loadrule.c client.c geoip.c init.c -lzlog -lpcap -o libtrojan.so -Wall -Wextra
clean:
	rm -rf *.o test
