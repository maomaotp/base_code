CC=g++ -lssl -lcrypto  
HEADERS=*.h
SOURCES=*.cpp
LIB=-pthread -L. -lcrypto
tipschecker:$(SOURCES) $(HEADERS)
	@$(CC) ../../sqlite3.o $(SOURCES) $(LIB) $(CLIENT_OBJS) -o tipscache -g -I..
clean:
	@rm -f *.o *.gch tipschecker
