/* $Id$ */

/*
** Copyright (C) 2014 Cisco and/or its affiliates. All rights reserved.
** Copyright (C) 2005-2013 Sourcefire, Inc.
** Author: Steven Sturges
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __SPP_STREAM5_H__
#define __SPP_STREAM5_H__

#include "decode.h"
//#include "stream5_common.h"


#define IS_IP4(x) ((x)->family == AF_INET)
#define IS_IP6(x) ((x)->family == AF_INET6)

#define COPY4(x, y)  x[0] = y[0]; x[1] = y[1]; x[2] = y[2]; x[3] = y[3];


#define IP_COPY_VALUE(x,y) \
        do {  \
                (x).bits = (y)->bits; \
                (x).family = (y)->family; \
                (x).ip32[0] = (y)->ip32[0]; \
                (x).ip32[1] = (y)->ip32[1]; \
                (x).ip32[2] = (y)->ip32[2]; \
                (x).ip32[3] = (y)->ip32[3]; \
        } while(0)

#define IS_OUTER_IP4(x) ((x)->outer_family == AF_INET)
#define IS_OUTER_IP6(x) ((x)->outer_family == AF_INET6)

#define GET_SRC_IP(p) ((p)->iph_api->iph_ret_src(p))
#define GET_DST_IP(p) ((p)->iph_api->iph_ret_dst(p))
#define GET_IPH_TOS(p)   (p)->iph_api->iph_ret_tos(p)
#define GET_IPH_LEN(p)   (p)->iph_api->iph_ret_len(p)
#define GET_IPH_TTL(p)   (p)->iph_api->iph_ret_ttl(p)
#define GET_IPH_ID(p)    (p)->iph_api->iph_ret_id(p)
#define GET_IPH_OFF(p)   (p)->iph_api->iph_ret_off(p)
#define GET_IPH_VER(p)   (p)->iph_api->iph_ret_ver(p)
#define GET_IPH_PROTO(p) ((uint8_t)(IS_IP6(p) ? ((p)->ip6h->next) : ((p)->iph_api->iph_ret_proto(p))))

#define GET_ORIG_IPH_PROTO(p)   (p)->orig_ip4h_api->orig_iph_ret_proto(p)
#define GET_ORIG_IPH_VER(p)     (p)->orig_ip4h_api->orig_iph_ret_ver(p)
#define GET_ORIG_IPH_LEN(p)     (p)->orig_ip4h_api->orig_iph_ret_len(p)
#define GET_ORIG_IPH_OFF(p)     (p)->orig_ip4h_api->orig_iph_ret_off(p)


/* list of function prototypes for this preprocessor */
void SetupStream5(void);

#endif  /* __SPP_STREAM5_H__ */
