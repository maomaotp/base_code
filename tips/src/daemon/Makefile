#update by yong , 2013.10.14
CC=g++
tips:
	@$(CC) -g daemon.cpp AdaptiveData.cpp sharemem.cpp ReadConfig.cpp GetConfigInfo.cpp Sigsuspend.cpp -o tipsdaemon libn6601.so -lpthread -lrt -lzlog -Wl,-rpath,/root/daemon 
clean:
	@rm main.o tipsdaemon *.o -rf
