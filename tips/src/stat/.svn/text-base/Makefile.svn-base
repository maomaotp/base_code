GSOAP_ROOT=/root/gsoap/gsoap-2.8/gsoap
CC=g++ -DWITH_OPENSSL -DHAVE_CONFIG_H -DWITH_GZIP -lzlog -lssl -lcrypto ./net_guard_final/libgsoapssl++.a  -Wl,-rpath
HEADERS=*.h 
SOURCES=*.cpp ./sharemem/*.cpp
OBJECT=communiteNetwork.o
CLIENT_OBJS=./net_guard_final/soapC.cpp ./net_guard_final/stdsoap2.cpp ./net_guard_final/soapClient.cpp ./net_guard_final/client.cpp
all:tipsstat
tipsstat:$(SOURCES) $(HEADERS) $(OBJECT)
	@$(CC) -g -Wall -Wextra -I$(GSOAP_ROOT) $(SOURCES) $(CLIENT_OBJS) $(OBJECT) -lrt -o tipsstat -pthread -ldl -lpcap -I.. 
communiteNetwork.o:communiteNetwork.cpp communiteNetwork.h
	g++ -Wall -Wextra -o communiteNetwork.o -c communiteNetwork.cpp

install:
	cp tipsstat TIPS_INSTALL_DIR
clean:
	@rm -f *.gch tipsstat *.o
