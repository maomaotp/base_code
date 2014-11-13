#include "soapH.h"
#include "DataEngineBinding.nsmap"

int http_get(struct soap *soap);

int main(int argc, char* argv[])
{
    int m, s; /* master and slave sockets */
    struct soap add_soap;
    soap_init(&add_soap);
    soap_set_namespaces(&add_soap, namespaces);
   
	//add_soap.fget = http_get;
    if (argc < 2) {
        printf("usage: %s <server_port> \n", argv[0]);
        exit(1);
    }

    m = soap_bind(&add_soap, NULL, atoi(argv[1]), 100);
    if (m < 0) {
        soap_print_fault(&add_soap, stderr);
        exit(-1);
    }
    
    fprintf(stderr, "Socket connection successful: master socket = %d\n", m);
    while(1) { 
        s = soap_accept(&add_soap); 
        if (s < 0) { 
            soap_print_fault(&add_soap, stderr);
            exit(-1);
        }
        fprintf(stderr, "Socket connection successful: slave socket = %d\n", s);
        
        soap_serve(&add_soap);//该句说明该server的服务
        soap_end(&add_soap);
    }

    return 0;
}


int __ns1__queryOrgan(struct ns2__queryOrgan* ns2__queryOrgan, struct ns2__queryOrganResponse *ns2__queryOrganResponse)
{
}


int ns2__add(struct soap *add_soap, int num1, int num2, int *sum)
{
	*sum = num1 + num2;
	return 0;
}


/*
int http_get(struct soap *soap) 
{ 
	FILE*fd = NULL;
	fd = fopen("CreateXml.xml", "rb"); //open WSDL file to copy
	if (!fd){
		printf("can not open add.wsdl\n");
		return 404; //return HTTP not found error
	}
	soap->http_content = "text/xml";  //HTTP header with text /xml content
	while(1) {
		size_t r = fread(soap->tmpbuf,1, sizeof(soap->tmpbuf), fd);
		if (!r){
			break;
		}
		if (soap_send_raw(soap, soap->tmpbuf, r)){
			break; //cannot send, but little we can do about that
		}
	}
	fclose(fd);
	//printf("test\n");
	soap_response(soap,SOAP_FILE);
	soap_end_send(soap);
	return SOAP_OK; 
}
*/
