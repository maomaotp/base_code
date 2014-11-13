/* Greenbone Security Assistant
 * $Id$
 * Description: Headers/structs used generally in GSA
 *
 * Authors:
 * Matthew Mundell <matthew.mundell@greenbone.net>
 * Jan-Oliver Wagner <jan-oliver.wagner@greenbone.net>
 *
 * Copyright:
 * Copyright (C) 2009 Greenbone Networks GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2,
 * or, at your option, any later version as published by the Free
 * Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * @file gsad_base.h
 * @brief Headers/structs used generally in GSA.
 */

#ifndef _GSAD_BASE_H
#define _GSAD_BASE_H

#include <glib.h>

/**
 * @brief Initial filtered results per page on the report summary.
 */
#define RESULTS_PER_PAGE 100

/** @brief Answer for invalid input. */
#define GSAD_MESSAGE_INVALID                                                      \
  "<gsad_msg status_text=\"%s\" operation=\"%s\">"                                \
  "At least one entered value contains invalid characters or exceeds"             \
  " a size limit.  You may use the Back button of your browser to adjust"         \
  " the entered values.  If in doubt, the online help of the respective section"  \
  " will lead you to the appropriate help page."                                  \
  "</gsad_msg>"

/** @brief Answer for invalid input. */
#define GSAD_MESSAGE_INVALID_PARAM(op)                                            \
  "<gsad_msg status_text=\"Invalid parameter\" operation=\"" op "\">"             \
  "At least one entered value contains invalid characters or exceeds"             \
  " a size limit.  You may use the Back button of your browser to adjust"         \
  " the entered values.  If in doubt, the online help of the respective section"  \
  " will lead you to the appropriate help page."                                  \
  "</gsad_msg>"

/**
 *  @brief Structure of credential related information.
 */
#include <iostream>
using namespace std;
typedef struct
{
  char *username;  ///< Name of user.
  char *password;  ///< User's password.
  char *role;      ///< User's role.
  char *timezone;  ///< User's timezone.
  char *token;     ///< Session token.
  char *caller;    ///< Caller URL, for POST relogin.
} credentials_t;

struct user
{
  char *cookie;       ///< Cookie token.
  char *token;        ///< Request session token.
  gchar *username;    ///< Login name.
  gchar *password;    ///< Password.
  gchar *role;        ///< Role.
  gchar *timezone;    ///< Timezone.
  gchar *state;    ///< state.
  gchar *admit_time;    ///< admit_time.
  time_t time;        ///< Login time.
  int login_flag;
};
typedef struct user user_t;

typedef struct sqlite_user
{
	const char *username;
	const char *password;
	gchar *role;
	char *createtime;
	char *admit_time;//有效期
	char *client_name;//姓名
	char *unit_name;//部门
	char *telephone;//电话
	char *last_time;//上次登陆时间
	char *state;//登录状态
	int login_flag;

}sqlite_user_t;

typedef struct config_manage
{
	char manage_IP[50];		/*管理口IP*/
	char manage_IP_MAC[50];		/*管理口IP的MAC地址*/
	char manage_mask[50];		/*管理口IP的子网掩码*/
	char manage_gateway[50];	/*管理口网关*/
	char manage_PORT[50];		/*管理中心PORT*/
	char manage_PORT1[50];		/*管理中心PORT*/
	char net_IP[50];			/*通信网口IP*/
	char gateway[50];			/*通信口网关*/
	char mac[50];				/*通信口MAC地址*/
	char mask[50];				/*通信口子网掩码*/
	char static_route[50];		/*静态路由*/
	char supervisor[50];		/*管理机IP*/
	char serve_addr[50];		/*代理服务器地址*/
	char serve_user[50];		/*代理服务器地址*/
	char serve_password[50];		/*代理服务器口令*/
	char adminstration_IP[50];		/*管理中心IP*/
	char adminstration_IP1[50];		/*管理中心IP*/
	char unit_name[256];		/*部属单位名称*/
	//char unit_addr[256];		/*部属单位地址*/
	int modify_flag;

}config_manage_t;

typedef struct system_config
{
	char cpu[50];
	char mem[50];
	char disk[50];
	char runtime[50];
	char alarmtime[50];
	char syntime[50];
	char netstatetime[50];
	int modify_flag;
}system_config_t;

typedef struct status_struct{
char device_id[12];
char stime[21];
int mem;
int cpu;
double disk;
char mirror;//镜像口状态1是开启，0是关闭
int flow;//镜像口流量
int active_peer;
char module_state[9];
}status_struct_t;//设备状态信息

/**
 * @brief Config preference.
 */
typedef struct
{
  gchar *name;     ///< Name of preference.
  gchar *nvt;      ///< ID of NVT.
  void *value;     ///< Value of preference.
  int value_size;  ///< Size of value.
} preference_t;

/**
 * @brief Method data parameters.
 */
struct method_data_param
{
  gchar *key;             /* Key. */
  gchar *value;           /* Binary value. */
  gsize value_size;       /* Size of value. */
};

/**
 * @brief Method data parameter type.
 */
typedef struct method_data_param method_data_param_t;

int gsad_base_init ();
char *ctime_r_strip_newline (time_t *, char *);
char * xsl_transform (const char *);
char * gsad_message (credentials_t *, const char *, const char *, int,
                     const char *, const char *);

/**
 * @brief Content types.
 */
enum content_type
{
  GSAD_CONTENT_TYPE_APP_DEB,
  GSAD_CONTENT_TYPE_APP_EXE,
  GSAD_CONTENT_TYPE_APP_HTML,
  GSAD_CONTENT_TYPE_APP_KEY,
  GSAD_CONTENT_TYPE_APP_NBE,
  GSAD_CONTENT_TYPE_APP_PDF,
  GSAD_CONTENT_TYPE_APP_RPM,
  GSAD_CONTENT_TYPE_APP_XML,
  GSAD_CONTENT_TYPE_DONE,         ///< Special marker.
  GSAD_CONTENT_TYPE_IMAGE_PNG,
  GSAD_CONTENT_TYPE_TEXT_CSS,
  GSAD_CONTENT_TYPE_TEXT_HTML,
  GSAD_CONTENT_TYPE_TEXT_PLAIN,
  GSAD_CONTENT_TYPE_OCTET_STREAM
} ;


/* Params. */

#define params_t GHashTable

/**
 * @brief Request parameter.
 */
struct param
{
  int valid;             /* Validation flag. */
  gchar *value;          /* Value. */
  gchar *original_value; /* Original value, before validation. */
  gchar *filename;       /* Filename. */
  int value_size;        /* Size of value, excluding trailing NULL. */
  params_t *values;      /* Multiple binary values. */
  int filelen;
};

/**
 * @brief Request parameter.
 */
typedef struct param param_t;

params_t *params_new ();

void params_free (params_t *);

int params_given (params_t *, const char *);

const char *params_value (params_t *, const char *);

int params_value_size (params_t *, const char *);

const char *params_original_value (params_t *, const char *);

const char *params_filename (params_t *, const char *);

params_t *params_values (params_t *, const char *);

param_t *params_get (params_t *, const char *);

int params_valid (params_t *, const char *);

param_t *params_add (params_t *, const char *, const char *);

param_t *params_append_bin (params_t *, const char *, const char *, int, int);

#define params_iterator_t GHashTableIter

#define params_iterator_init g_hash_table_iter_init

gboolean params_iterator_next (params_iterator_t *, char **, param_t **);

#endif /* not _GSAD_BASE_H */
