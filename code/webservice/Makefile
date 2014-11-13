all: fmserver.c
	gcc -o fm_server fmserver.c sqlhandler.c sql.c  -levent -lhiredis -lxml2 -lmysqlclient

clean:
	rm -rf *.o
