all: main main1

main:main.cpp sharemem.o
	g++ -o main main.cpp sharemem.o -lrt
main1:main1.cpp sharemem.o
	g++ -o main1 main1.cpp sharemem.o -lrt

sharemem.o:sharemem.cpp sharemem.h shm_struct_t.h
	g++ -c -o sharemem.o sharemem.cpp -lrt

clean:
	rm -f *.o main main1
