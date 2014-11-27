/* soapClient.c
   Generated by gSOAP 2.8.18 from fm.h

Copyright(C) 2000-2014, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"
#ifdef __cplusplus
extern "C" {
#endif

SOAP_SOURCE_STAMP("@(#) soapClient.c ver 2.8.18 2014-10-16 10:02:24 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__queryFm(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct ns2__queryFm *ns2__queryFm, struct ns2__queryFmResponse *ns2__queryFmResponse)
{	struct __ns1__queryFm soap_tmp___ns1__queryFm;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://192.168.2.230:52101/lab503Service/dataengine";
	if (soap_action == NULL)
		soap_action = "";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___ns1__queryFm.ns2__queryFm = ns2__queryFm;
	soap_serializeheader(soap);
	soap_serialize___ns1__queryFm(soap, &soap_tmp___ns1__queryFm);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__queryFm(soap, &soap_tmp___ns1__queryFm, "-ns1:queryFm", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__queryFm(soap, &soap_tmp___ns1__queryFm, "-ns1:queryFm", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!ns2__queryFmResponse)
		return soap_closesock(soap);
	soap_default_ns2__queryFmResponse(soap, ns2__queryFmResponse);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_ns2__queryFmResponse(soap, ns2__queryFmResponse, "ns2:queryFmResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

#ifdef __cplusplus
}
#endif

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapClient.c */