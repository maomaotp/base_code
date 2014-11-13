all:main main1
main:main.c sharemem.o
	gcc -o main main.c sharemem.o -lrt
main1:main1.c sharemem.o
	gcc -o main1 main1.c sharemem.o -lrt

sharemem.o:sharemem.c sharemem.h shm_struct_t.h
	gcc -c -o sharemem.o sharemem.c -lrt

clean:
	rm -f *.o main main1
