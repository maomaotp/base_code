GSOAP_ROOT=/root/gsoap/gsoap-2.8/gsoap
CC=g++ -DWITH_OPENSSL -DHAVE_CONFIG_H -DWITH_GZIP -lzlog -lssl -lcrypto ./net_guard_final/libgsoapssl++.a -I/usr/local/mysql/include -lmysqlclient -L/usr/local/mysql/lib -Wl,-rpath,/usr/local/mysql/lib
HEADERS=*.h 
SOURCES=*.cpp
CLIENT_OBJS=./net_guard_final/soapC.cpp ./net_guard_final/stdsoap2.cpp ./net_guard_final/soapClient.cpp ./net_guard_final/client.cpp
SHAREMEM=./sharemem/*.cpp
tipsmanager:$(SOURCES) $(HEADERS)
	@$(CC) -g -Wall -Wextra -I$(GSOAP_ROOT) $(SOURCES) $(CLIENT_OBJS) $(SHAREMEM) -lrt -otipsmanager -pthread -ldl -I.. 
clean:
	@rm -f *.gch tipsmanager
