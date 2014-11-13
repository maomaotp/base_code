// Reminder: Modify typemap.dat to customize the header file generated by wsdl2h
/* fm.h
   Generated by wsdl2h 2.8.18 from lab503_dataengine_service_1_0.wsdl and typemap.dat
   2014-10-16 10:02:00 GMT

   DO NOT INCLUDE THIS FILE DIRECTLY INTO YOUR PROJECT BUILDS
   USE THE soapcpp2-GENERATED SOURCE CODE FILES FOR YOUR PROJECT BUILDS

   gSOAP XML Web services tools.
   Copyright (C) 2000-2014 Robert van Engelen, Genivia Inc. All Rights Reserved.
   Part of this software is released under one of the following licenses:
   GPL or Genivia's license for commercial use.
*/

/** @page page_notes Notes

@note HINTS:
 - Run soapcpp2 on fm.h to generate the SOAP/XML processing logic.
   Use soapcpp2 -I to specify paths for #import
   To build with STL, 'stlvector.h' is imported from 'import' dir in package.
   Use soapcpp2 -j to generate improved proxy and server classes.
 - Use wsdl2h -c and -s to generate pure C code or C++ code without STL.
 - Use 'typemap.dat' to control namespace bindings and type mappings.
   It is strongly recommended to customize the names of the namespace prefixes
   generated by wsdl2h. To do so, modify the prefix bindings in the Namespaces
   section below and add the modified lines to 'typemap.dat' to rerun wsdl2h.
 - Use Doxygen (www.doxygen.org) on this file to generate documentation.
 - Use wsdl2h -R to generate REST operations.
 - Use wsdl2h -nname to use name as the base namespace prefix instead of 'ns'.
 - Use wsdl2h -Nname for service prefix and produce multiple service bindings
 - Use wsdl2h -d to enable DOM support for xsd:anyType.
 - Use wsdl2h -g to auto-generate readers and writers for root elements.
 - Use wsdl2h -b to auto-generate bi-directional operations (duplex ops).
 - Struct/class members serialized as XML attributes are annotated with a '@'.
 - Struct/class members that have a special role are annotated with a '$'.

@warning
   DO NOT INCLUDE THIS ANNOTATED FILE DIRECTLY IN YOUR PROJECT SOURCE CODE.
   USE THE FILES GENERATED BY soapcpp2 FOR YOUR PROJECT'S SOURCE CODE:
   THE soapStub.h FILE CONTAINS THIS CONTENT WITHOUT ANNOTATIONS.

@copyright LICENSE:
@verbatim
--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2000-2014, Robert van Engelen, Genivia Inc. All Rights Reserved.

This software is released under one of the following licenses:
GPL or Genivia's license for commercial use.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org

This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial-use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
@endverbatim

*/


//gsoapopt cw

/******************************************************************************\
 *                                                                            *
 * Definitions                                                                *
 *   http://www.lab503.cn/wsdl/dataengine/v1/service                          *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Import                                                                     *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Schema Namespaces                                                          *
 *                                                                            *
\******************************************************************************/


/* NOTE:

It is strongly recommended to customize the names of the namespace prefixes
generated by wsdl2h. To do so, modify the prefix bindings below and add the
modified lines to typemap.dat to rerun wsdl2h:

ns1 = "http://www.lab503.cn/wsdl/dataengine/v1/service"
ns2 = "http://www.lab503.cn/schema/dataengine/v1/local"
ns3 = "http://www.lab503.cn/schema/dataengine/v1/types"

*/

#define SOAP_NAMESPACE_OF_ns2	"http://www.lab503.cn/schema/dataengine/v1/local"
//gsoap ns2   schema namespace:	http://www.lab503.cn/schema/dataengine/v1/local
//gsoap ns2   schema elementForm:	qualified
//gsoap ns2   schema attributeForm:	unqualified

#define SOAP_NAMESPACE_OF_ns3	"http://www.lab503.cn/schema/dataengine/v1/types"
//gsoap ns3   schema namespace:	http://www.lab503.cn/schema/dataengine/v1/types
//gsoap ns3   schema form:	unqualified

/******************************************************************************\
 *                                                                            *
 * Built-in Schema Types and Top-Level Elements and Attributes                *
 *                                                                            *
\******************************************************************************/



/******************************************************************************\
 *                                                                            *
 * Schema Types and Top-Level Elements and Attributes                         *
 *   http://www.lab503.cn/schema/dataengine/v1/local                          *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Schema Types and Top-Level Elements and Attributes                         *
 *   http://www.lab503.cn/schema/dataengine/v1/types                          *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Schema Complex Types and Top-Level Elements                                *
 *   http://www.lab503.cn/schema/dataengine/v1/local                          *
 *                                                                            *
\******************************************************************************/

/// @brief "http://www.lab503.cn/schema/dataengine/v1/local":ServiceException is a complexType.
///
struct ns2__ServiceException
{
/// Element "messageId" of XSD type xs:string.
    char*                                messageId                      1;	///< Required element.
/// Element "text" of XSD type xs:string.
    char*                                text                           1;	///< Required element.
};

/// @brief "http://www.lab503.cn/schema/dataengine/v1/local":queryFm is a complexType.
///
struct ns2__queryFm
{
/// Element "userId" of XSD type xs:string.
    char*                                userId                         1;	///< Required element.
};

/// @brief "http://www.lab503.cn/schema/dataengine/v1/local":queryFmResponse is a complexType.
///
struct ns2__queryFmResponse
{
/// Element "fmId" of XSD type xs:string.
    char*                                fmId                           1;	///< Required element.
};


/******************************************************************************\
 *                                                                            *
 * Schema Complex Types and Top-Level Elements                                *
 *   http://www.lab503.cn/schema/dataengine/v1/types                          *
 *                                                                            *
\******************************************************************************/

/// @brief "http://www.lab503.cn/schema/dataengine/v1/types":AuthInfo is a complexType.
///
struct ns3__AuthInfo
{
/// Element "clientId" of XSD type xs:string.
    char*                                clientId                       1;	///< Required element.
/// Element "timeStamp" of XSD type xs:string.
    char*                                timeStamp                      1;	///< Required element.
/// Element "passwd" of XSD type xs:string.
    char*                                passwd                         1;	///< Required element.
/// Element "opPerson" of XSD type xs:string.
    char*                                opPerson                       0;	///< Optional element.
};

/// @brief "http://www.lab503.cn/schema/dataengine/v1/types":OrganizationInfo is a complexType.
///
struct ns3__OrganizationInfo
{
/// Element "fmId" of XSD type xs:string.
    char*                                fmId                           1;	///< Required element.
/// Element "wbRid" of XSD type xs:int.
    int                                  wbRid                          1;	///< Required element.
/// Element "fmName" of XSD type xs:string.
    char*                                fmName                         1;	///< Required element.
/// Element "url" of XSD type xs:string.
    char*                                url                            1;	///< Required element.
/// Element "intro" of XSD type xs:string.
    char*                                intro                          1;	///< Required element.
/// Element "logoAddr" of XSD type xs:string.
    char*                                logoAddr                       1;	///< Required element.
/// Element "weiboAddr" of XSD type xs:string.
    char*                                weiboAddr                      1;	///< Required element.
/// Element "province" of XSD type xs:int.
    int                                  province                       1;	///< Required element.
/// Element "hdvAddr" of XSD type xs:string.
    char*                                hdvAddr                        1;	///< Required element.
/// Element "microAddr" of XSD type xs:string.
    char*                                microAddr                      1;	///< Required element.
/// Element "updateTime" of XSD type xs:string.
    char*                                updateTime                     1;	///< Required element.
};


/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Elements                                              *
 *   http://www.lab503.cn/schema/dataengine/v1/local                          *
 *                                                                            *
\******************************************************************************/

/// @brief Top-level root element "http://www.lab503.cn/schema/dataengine/v1/local":ServiceException of XSD type "http://www.lab503.cn/schema/dataengine/v1/local":ServiceException.
/// @note Use wsdl2h option -g to auto-generate a top-level root element declaration and processing code.

/// @brief Top-level root element "http://www.lab503.cn/schema/dataengine/v1/local":queryFm of XSD type "http://www.lab503.cn/schema/dataengine/v1/local":queryFm.
/// @note Use wsdl2h option -g to auto-generate a top-level root element declaration and processing code.

/// @brief Top-level root element "http://www.lab503.cn/schema/dataengine/v1/local":queryFmResponse of XSD type "http://www.lab503.cn/schema/dataengine/v1/local":queryFmResponse.
/// @note Use wsdl2h option -g to auto-generate a top-level root element declaration and processing code.


/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Attributes                                            *
 *   http://www.lab503.cn/schema/dataengine/v1/local                          *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Elements                                              *
 *   http://www.lab503.cn/schema/dataengine/v1/types                          *
 *                                                                            *
\******************************************************************************/

/// @brief Top-level root element "http://www.lab503.cn/schema/dataengine/v1/types":AuthInfo of XSD type "http://www.lab503.cn/schema/dataengine/v1/types":AuthInfo.
/// @note Use wsdl2h option -g to auto-generate a top-level root element declaration and processing code.

/// @brief Top-level root element "http://www.lab503.cn/schema/dataengine/v1/types":OrganizationInfo of XSD type "http://www.lab503.cn/schema/dataengine/v1/types":OrganizationInfo.
/// @note Use wsdl2h option -g to auto-generate a top-level root element declaration and processing code.


/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Attributes                                            *
 *   http://www.lab503.cn/schema/dataengine/v1/types                          *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Services                                                                   *
 *                                                                            *
\******************************************************************************/


//gsoap ns1  service name:	DataEngineBinding 
//gsoap ns1  service type:	DataEngine 
//gsoap ns1  service port:	http://192.168.2.230:52101/lab503Service/dataengine 
//gsoap ns1  service namespace:	http://www.lab503.cn/wsdl/dataengine/v1/service 
//gsoap ns1  service transport:	http://schemas.xmlsoap.org/soap/http 

/** @mainpage dataengine_USCOREservice Definitions

@section dataengine_USCOREservice_bindings Service Bindings

  - @ref DataEngineBinding

@section dataengine_USCOREservice_more More Information

  - @ref page_notes "Notes"

  - @ref page_XMLDataBinding "XML Data Binding"

  - @ref SOAP_ENV__Header "SOAP Header Content" (when applicable)

  - @ref SOAP_ENV__Detail "SOAP Fault Detail Content" (when applicable)


*/

/**

@page DataEngineBinding Binding "DataEngineBinding"

@section DataEngineBinding_operations Operations of Binding "DataEngineBinding"

  - @ref __ns1__queryFm

@section DataEngineBinding_ports Endpoints of Binding "DataEngineBinding"

  - http://192.168.2.230:52101/lab503Service/dataengine

@note Use wsdl2h option -Nname to change the service binding prefix name


*/

/******************************************************************************\
 *                                                                            *
 * SOAP Fault Detail                                                          *
 *                                                                            *
\******************************************************************************/

/**

The SOAP Fault is part of the gSOAP context and its content is accessed
through the soap.fault->detail variable (SOAP 1.1) or the
soap.fault->SOAP_ENV__Detail variable (SOAP 1.2).
Use wsdl2h option -j to omit these declarations.

*/
struct SOAP_ENV__Detail
{
/// xsd:anyAttribute omitted: to parse attribute content of the Detail element into DOM anyAttribute, use wsdl2h option -d.
/// SOAP Fault message "DataEngine_ServiceException" part "ServiceException" element ""http://www.lab503.cn/schema/dataengine/v1/local":ServiceException".
    struct ns2__ServiceException*        ns2__ServiceException         ;
    int                                  __type                        ;	///< set to SOAP_TYPE_X for a serializable type X
    void                                *fault                         ;	///< points to serializable object of type X or NULL
    _XML                                 __any                         ;	///< Catch any other content in XML string.
};


/******************************************************************************\
 *                                                                            *
 * Service Binding                                                            *
 *   DataEngineBinding                                                        *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Service Operation                                                          *
 *   __ns1__queryFm                                                           *
 *                                                                            *
\******************************************************************************/


/** Operation "__ns1__queryFm" of service binding "DataEngineBinding".

  - SOAP document/literal style messaging

  - Addressing input action: ""

  - Addressing output action: "Response"

  - SOAP Output Fault: "http://www.lab503.cn/schema/dataengine/v1/local":ServiceException (literal)

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns1__queryFm(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // input parameters:
    struct ns2__queryFm*                ns2__queryFm,
    // output parameters:
    struct ns2__queryFmResponse        *ns2__queryFmResponse
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns1__queryFm(
    struct soap *soap,
    // input parameters:
    struct ns2__queryFm*                ns2__queryFm,
    // output parameters:
    struct ns2__queryFmResponse        *ns2__queryFmResponse
  );
@endcode

*/

//gsoap ns1  service method-protocol:	queryFm SOAP
//gsoap ns1  service method-style:	queryFm document
//gsoap ns1  service method-encoding:	queryFm literal
//gsoap ns1  service method-action:	queryFm ""
//gsoap ns1  service method-output-action:	queryFm Response
//gsoap ns1  service method-fault:	queryFm ns2__ServiceException
int __ns1__queryFm(
    struct ns2__queryFm*                ns2__queryFm,	///< Input parameter
    struct ns2__queryFmResponse        *ns2__queryFmResponse	///< Output parameter
);

/**

@page DataEngineBinding Binding "DataEngineBinding"

@section DataEngineBinding_policy_enablers Policy Enablers of Binding "DataEngineBinding"

Based on policies, this service imports

  - WS-Policy reminders and enablers:
    - WS-Addressing 1.0 (2005/08, accepts 2004/08):
	@code
	#import "wsa5.h" // to be added to this header file for the soapcpp2 build step
	@endcode
	@code
	#include "plugin/wsaapi.h"
	soap_register_plugin(soap, soap_wsa); // register the wsa plugin in your code
	// See the user guide gsoap/doc/wsa/html/index.html
	@endcode
    - WS-Addressing (2004/08):
	@code
	#import "wsa.h" // to be added to this header file for the soapcpp2 build step
	@endcode
	@code
	#include "plugin/wsaapi.h"
	soap_register_plugin(soap, soap_wsa); // register the wsa plugin in your code
	// See the user guide gsoap/doc/wsa/html/index.html
	@endcode
    - WS-ReliableMessaging 1.0:
	@code
	#import "wsrm5.h" // to be added to this header file for the soapcpp2 build step
	@endcode
	@code
	#include "plugin/wsrmapi.h"
	soap_register_plugin(soap, soap_wsa); // register the wsa plugin in your code
	soap_register_plugin(soap, soap_wsrm); // register the wsrm plugin in your code
	// See the user guide gsoap/doc/wsrm/html/index.html
	@endcode
    - WS-ReliableMessaging 1.1:
	@code
	#import "wsrm.h" // to be added to this header file for the soapcpp2 build step
	@endcode
	@code
	#include "plugin/wsrmapi.h"
	soap_register_plugin(soap, soap_wsa); // register the wsa plugin in your code
	soap_register_plugin(soap, soap_wsrm); // register the wsrm plugin in your code
	// See the user guide gsoap/doc/wsrm/html/index.html
	@endcode
    - WS-Security (SOAP Message Security) 1.0 (accepts 1.1):
	@code
	#import "wsse.h" // to be added to this header file for the soapcpp2 build step
	@endcode
	@code
	#include "plugin/wsseapi.h"
	soap_register_plugin(soap, soap_wsse); // register the wsse plugin in your code
	// See the user guide gsoap/doc/wsse/html/index.html
	@endcode
    - WS-Security (SOAP Message Security) 1.1 (accepts 1.0):
	@code
	#import "wsse11.h" // to be added to this header file for the soapcpp2 build step
	@endcode
	@code
	#include "plugin/wsseapi.h"
	soap_register_plugin(soap, soap_wsse); // register the wsse plugin in your code
	// See the user guide gsoap/doc/wsse/html/index.html
	@endcode
    - HTTP Digest Authentication:
	@code
	#include "plugin/httpda.h"
	soap_register_plugin(soap, soap_http_da); // register the HTTP DA plugin in your code
	// See the user guide gsoap/doc/httpda/html/index.html
	@endcode
*/


/******************************************************************************\
 *                                                                            *
 * XML Data Binding                                                           *
 *                                                                            *
\******************************************************************************/


/**

@page page_XMLDataBinding XML Data Binding

SOAP/XML services use data bindings contractually bound by WSDL and auto-
generated by wsdl2h and soapcpp2 (see Service Bindings). Plain data bindings
are adopted from XML schemas as part of the WSDL types section or when running
wsdl2h on a set of schemas to produce non-SOAP-based XML data bindings.

The following readers and writers are C/C++ data type (de)serializers auto-
generated by wsdl2h and soapcpp2. Run soapcpp2 on this file to generate the
(de)serialization code, which is stored in soapC.c[pp]. Include "soapH.h" in
your code to import these data type and function declarations. Only use the
soapcpp2-generated files in your project build. Do not include the wsdl2h-
generated .h file in your code.

Data can be read in XML and deserialized from:
  - a file descriptor, using soap->recvfd = fd
  - a socket, using soap->socket = ...
  - a C++ stream, using soap->is = ...
  - a buffer, using the soap->frecv() callback

Data can be serialized in XML and written to:
  - a file descriptor, using soap->sendfd = fd
  - a socket, using soap->socket = ...
  - a C++ stream, using soap->os = ...
  - a buffer, using the soap->fsend() callback

The following options are available for (de)serialization control:
  - soap->encodingStyle = NULL; to remove SOAP 1.1/1.2 encodingStyle
  - soap_mode(soap, SOAP_XML_TREE); XML without id-ref (no cycles!)
  - soap_mode(soap, SOAP_XML_GRAPH); XML with id-ref (including cycles)
  - soap_set_namespaces(soap, struct Namespace *nsmap); to set xmlns bindings


@section ns2 Top-level root elements of schema "http://www.lab503.cn/schema/dataengine/v1/local"

  - <ServiceException> (use wsdl2h option -g to auto-generate)

  - <queryFm> (use wsdl2h option -g to auto-generate)

  - <queryFmResponse> (use wsdl2h option -g to auto-generate)

@section ns3 Top-level root elements of schema "http://www.lab503.cn/schema/dataengine/v1/types"

  - <AuthInfo> (use wsdl2h option -g to auto-generate)

  - <OrganizationInfo> (use wsdl2h option -g to auto-generate)

*/

/* End of fm.h */
