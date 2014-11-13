GSOAP_ROOT=/root/gsoap/gsoap-2.8/gsoap
WSNAME=file
CC=g++ -g -DWITH_NONAMESPACES -DWITH_OPENSSL
INCLUDE=-I$(GSOAP_ROOT)
SERVER_OBJS=soapC.o stdsoap2.o soapServer.o server.o
CLIENT_OBJS=soapC.o stdsoap2.o soapClient.o client.o main.o

all:server
server: $(SERVER_OBJS)
	$(CC) $(INCLUDE) -o $(WSNAME)server $(SERVER_OBJS)  
  
client: $(CLIENT_OBJS)
	$(CC) $(INCLUDE) -o $(WSNAME)client $(CLIENT_OBJS) 

#clean: 
#	rm -f *.o *.xsd *.xml *.wsdl *.nsmap soapC.cpp soapH.h soapStub.h soapClient.cpp soapServer.cpp soapServerLib.cpp soapClientLib.cpp $(WSNAME)Stub.* $(WSNAME)server ns.xsd $(WSNAME)test 
