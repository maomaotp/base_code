all:tipsauditor

tipsauditor: Audit.o client.o connectlimit.o sharemem.o
	gcc -g  -Wall -Wextra -O2 -o tipsauditor Audit.o client.o connectlimit.o sharemem.o -L./ -lopendpi -lpcap -lzlog -Wl,-rpath -Wl,/usr/local/lib -pthread -lrt

Audit.o:Audit.c  
#	gcc -g -Wall -Wextra -DPACKAGE_NAME=\"OpenDPI\" -DPACKAGE_TARNAME=\"opendpi\" -DPACKAGE_VERSION=\"1.3.0\" -DPACKAGE_STRING=\"OpenDPI\ 1.3.0\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DPACKAGE=\"opendpi\" -DVERSION=\"1.3.0\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\"./\" -DHAVE_NETINET_IN_H=1 -DHAVE_STDINT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_UNISTD_H=1 -I.  -I../../../src/include/   -g -O2 -MT OpenDPI_demo.o -MD -MP -MF .deps/OpenDPI_demo.Tpo -c -o OpenDPI_demo.o OpenDPI_demo.c
	gcc -g -lzlog -Wall -Wextra -I./include -I ./ -I./glib  -g -O2 -MT Audit.o -MD -MP -MF .deps/Audit.Tpo -c -o Audit.o Audit.c 
client.o:client.c client.h
	gcc -c  -g -Wall -Wextra -o client.o client.c
connectlimit.o:./connectlimit/connectlimit.h ./connectlimit/connectlimit.c
	gcc -c -o connectlimit.o ./connectlimit/connectlimit.c
sharemem.o: ./sharemem/sharemem.h ./sharemem/sharemem.c
	gcc -c -g -Wall -Wextra -o sharemem.o ./sharemem/sharemem.c -lrt
clean:
	rm -rf tipsauditor   *.o

install:
	cp tipsauditor TIPS_INSTALL_DIR
#install:
#	cp -af libopendpi.so.1.0.3 /usr/local/lib
#	cp -af libopendpi.so /usr/local/lib
#	cp -af libopendpi.so.1 /usr/local/lib
#	cp -af libopendpi.a /usr/local/lib
#	cp -af libopendpi.la /usr/local/lib
