CC=g++ -lzlog -ltrojan -lcrypto -llssl -lpthread -pcap ,-rpath,./
HEADERS=*.h 
SOURCES=*.cpp
all:tipsalarm
tipsalarm:$(SOURCES) $(HEADERS)
	@$(CC) -g -Wall -Wextra -o tipsalarm -pthread 
clean:
	@rm -f tipsstat *.o
