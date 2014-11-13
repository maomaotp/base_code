
GSOAP_ROOT=/root/gsoap/gsoap-2.8/

CC=g++ -g -DWITH_OPENSSL -DWITH_NONAMESPACES -DHAVE_CONFIG_H -DWITH_GZIP -lssl -lcrypto ./net_guard_final/libgsoapssl++.a

HEADERS=*.h 
SOURCES=*.cpp
CLIENT_OBJS=./net_guard_final/soapC.cpp ./net_guard_final/stdsoap2.cpp ./net_guard_final/soapClient.cpp ./net_guard_final/client.cpp
tipsmanager:$(SOURCES) $(HEADERS)
	$(CC) -I$(GSOAP_ROOT) $(SOURCES) $(CLIENT_OBJS) -otipsmanager -DWITH_NONAMESPACES

stdsoap2:./net_guard_final/stdsoap2.cpp
	g++ -DWITH_OPENSSL -DWITH_NONAMESPACES -DHAVE_CONFIG_H -DWITH_GZIP $(INCLUDE) -o stdsoap2.o -c ./net_guard_final/stdsoap2.cpp

clean:
	@rm -f *.o *.gch tipsmanager
