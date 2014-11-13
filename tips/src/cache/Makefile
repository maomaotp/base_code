CC=g++ -lssl -lcrypto
HEADERS=*.h
SOURCES=*.cpp
LIB=-pthread -L.  -lcrypto
all:tipscache
tipscache:$(SOURCES) $(HEADERS)
	@$(CC)  $(SOURCES) $(LIB) $(CLIENT_OBJS)  -o tipscache -g -Wall -Wextra -I.. -I/usr/local/mysql/include -lzlog -lmysqlclient -L/usr/local/mysql/lib -Wl,-rpath,/usr/local/mysql/lib

install:
	cp tipscache TIPS_INSTALL_DIR
clean:
	@rm -f *.o *.gch tipscache
