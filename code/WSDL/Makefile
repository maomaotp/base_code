GSOAP_ROOT = /root/gsoap-2.7/gsoap
WSNAME = add
CC = g++ -g -DWITH_NONAMESPACES
INCLUDE = -I$(GSOAP_ROOT)
SERVER_OBJS = soapC.o stdsoap2.o soapServer.o $(WSNAME)server.o 
CLIENT_OBJS = soapC.o stdsoap2.o soapClient.o $(WSNAME)client.o $(WSNAME)test.o
	
all: server
server: $(SERVER_OBJS) 
	$(CC) $(INCLUDE) -o $(WSNAME)server $(SERVER_OBJS) 

client: $(CLIENT_OBJS) 
	    $(CC) $(INCLUDE) -o $(WSNAME)test $(CLIENT_OBJS)
	
clean:
	rm -f *.o *.xml *.a *.wsdl *.nsmap soapH.h $(WSNAME)Stub.* $(WSNAME)server ns.xsd $(WSNAME)test
