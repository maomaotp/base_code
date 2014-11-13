#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <gcrypt.h>
#include <glib.h>
//#include <glib.h>
//#include <gnutls/gnutls.h>
#include <locale.h>
#include <netinet/in.h>
#include <openvas_logging.h>
#include <cstring>
#include <pidfile.h>
#include <openvas_uuid.h>
#include <pthread.h>
#include <pwd.h> /* for getpwnam */
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "gsad_base.h"
#include "tips_sqlite.h"
#include "sqlhandler.h"
#include "tips_block.h"
#include "tips_event.h"
#include "tips_netlog.h"
#include "tips_opt.h"
#include "tips_xml_part.h"
#include "admin_opt.h"
#include "arp.h"
/* This must follow the system includes. */
#include <microhttpd.h>
#include "openvas_uuid.h"
#include "managecp_ip.h"
#include <fcntl.h>
#include "connection_https.h"
#include "sharemem.h"//add by lipeiting 2014-01-15
char ETH_USER[10] = {0};

struct gsad_connection_info
{
	int connectiontype;			/*连接类型，1是post2是get*/
	struct MHD_PostProcessor *postprocessor;/*post处理器*/
	char *response;				/*响应生成的html的内容*/
	int answercode;				/*请求参赛的获取情况*/
	params_t *params;			/*请求参数*/
	char *cookie;				/*用户信息等*/
};


char *SERVER_ERROR =(char *)"<html><body>An internal server error has occured.</body></html>";

ShareMem obj;//add by lipeiting 2014-01-15
struct sockaddr_in tips_address;
GPtrArray *users = NULL;
static GMutex *mutex = NULL;
GMutex *filemutex = NULL;
int session_timeout=180;
int use_secure_cookie = 1;
CUser_sqlite_handle tips_sqlite_handle;
CManageCP_IP managecp_ip;
const char *ERROR_PAGE = "<html><body>HTTP Method 不支持</body></html>";

#define EXPIRES_LENGTH 100
#define CONFIG_MAXLINE 4098


static void
gsad_add_content_type_header (struct MHD_Response *response,
                              enum content_type* ct)
{
  if (!response)
    return;

  switch (*ct)
    {
      case GSAD_CONTENT_TYPE_APP_DEB:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/deb");
        break;
      case GSAD_CONTENT_TYPE_APP_EXE:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/exe");
        break;
      case GSAD_CONTENT_TYPE_APP_HTML:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/html");
        break;
      case GSAD_CONTENT_TYPE_APP_KEY:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/key");
        break;
      case GSAD_CONTENT_TYPE_APP_NBE:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/nbe");
        break;
      case GSAD_CONTENT_TYPE_APP_PDF:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/pdf");
        break;
      case GSAD_CONTENT_TYPE_APP_RPM:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/rpm");
        break;
      case GSAD_CONTENT_TYPE_APP_XML:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/xml; charset=utf-8");
        break;
      case GSAD_CONTENT_TYPE_IMAGE_PNG:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "image/png");
        break;
      case GSAD_CONTENT_TYPE_OCTET_STREAM:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "application/octet-stream");
        break;
      case GSAD_CONTENT_TYPE_TEXT_CSS:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "text/css");
        break;
      case GSAD_CONTENT_TYPE_TEXT_HTML:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "text/html; charset=utf-8");
        break;
      case GSAD_CONTENT_TYPE_TEXT_PLAIN:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "text/plain; charset=utf-8");
        break;
      case GSAD_CONTENT_TYPE_DONE:
        break;
      default:
        MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                 "text/plain; charset=utf-8");
        break;
    }
}


static int attach_sid(struct MHD_Response *response, const char *sid)
{
	printf("This is in the function attach_sid\n");
	gchar* value;
	char expires[101];
	struct tm expire_time_broken;
	time_t now,expire_time;
//hyj	gchar *tz;
	int ret;
	
	now = time(NULL);
	expire_time = now + (session_timeout * 60) + 30;
	if(localtime_r(&expire_time, &expire_time_broken) == NULL)
		abort();
	ret = strftime(expires, 100, "%a, %d-%b-%Y %T GMT", 
			&expire_time_broken);
	if(ret == 0)
		abort();
	value = g_strdup_printf("SID=%s; expires=%s; path=/; %sHTTPonly",
				sid, expires,
				(1 ? "secure; ":""));
	ret = MHD_add_response_header(response, "Set-Cookie",value);
	g_free(value);
	return ret;

}

static int
remove_sid (struct MHD_Response *response)
{
  int ret;
  gchar *value;
//hyj  char *locale;
  char expires[EXPIRES_LENGTH + 1];
  struct tm expire_time_broken;
  time_t expire_time;

  /* Set up the expires param. */

  expire_time = time (NULL);
  if (localtime_r (&expire_time, &expire_time_broken) == NULL)
    abort ();
  ret = strftime (expires, EXPIRES_LENGTH, "%a, %d-%b-%Y %T GMT",
                  &expire_time_broken);
  if (ret == 0)
    abort ();

  value = g_strdup_printf ("SID=0; expires=%s; path=/; %sHTTPonly",
                           expires,
                           (1 ? "secure; " : ""));
  ret = MHD_add_response_header (response, "Set-Cookie", value);
  g_free (value);
  return ret;
}

user_t *
user_add (const gchar *username, const gchar *password,
	  const gchar *timezone, const gchar *role)
{
/*保留参数?*/if(timezone){};
  user_t *user = NULL;
  int index;
  if(role == NULL){
	printf("role == NULL\n");
	exit(0);
  }
  g_mutex_lock (mutex);
  for (index = 0; (unsigned int)index < users->len; index++)			//update by hyj
    {
      user_t *item;
      item = (user_t*) g_ptr_array_index (users, index);
      if (strcmp (item->username, username) == 0)
        {
          user = item;
          break;
        }
    }
  if (user)//验证是否已经用此username登陆的用户，如果有则会一直登陆超时的会话中显示
    {
      free (user->token);
      user->token = openvas_uuid_make ();
      free (user->cookie);
      user->cookie = openvas_uuid_make ();
      g_free (user->password);
      user->password = g_strdup (password);
      g_free (user->role);
      user->role = g_strdup (role);
 //   g_free (user->timezone);
 //   user->timezone = g_strdup (timezone);
    }
  else//如果没有此username登陆的用户，则可以登陆
    {
      user = (user_t*)g_malloc (sizeof (user_t));
      user->cookie = openvas_uuid_make ();
      user->token = openvas_uuid_make ();
      user->username = g_strdup (username);
      user->password = g_strdup (password);
      user->role = g_strdup (role);
   //   user->timezone = g_strdup (timezone);
      g_ptr_array_add (users, (gpointer) user);
    }
  user->time = time (NULL);
  g_mutex_unlock(mutex);
  return user;
}

void
user_remove (user_t *user)
{
  g_ptr_array_remove (users, (gpointer) user);
  g_mutex_unlock (mutex);
}

int
user_find (const gchar *cookie, const gchar *token, user_t **user_return)
{
  int ret;
  user_t *user = NULL;
  int index;
  g_mutex_lock (mutex);
  if(token == NULL){
 	g_mutex_unlock (mutex);
	return 3;
   }
  for (index = 0; (unsigned int)index < users->len; index++)			//update by hyj
    {
      user_t *item;
      item = (user_t*) g_ptr_array_index (users, index);
      if (strcmp (item->token, token) == 0)
        {
          if ((cookie == NULL) || strcmp (item->cookie, cookie))//cookie丢失或者为空
            {
              /* Check if the session has expired. */
              if (time (NULL) - item->time > (session_timeout * 60))//判断是否超时
                /* Probably the browser removed the cookie. */
                ret = 2;
              else
                ret = 3;
              break;
            }
          user = item;
          break;
        }
    }
  if (user)
    {
      if (time (NULL) - user->time > (session_timeout * 60))
        ret = 2;
      else
        {
          *user_return = user;
          ret = 0;
          user->time = time (NULL);
  	  g_mutex_unlock (mutex);
          return ret;
        }
    }
  else
    ret = 2;
  g_mutex_unlock (mutex);
  return ret;
}

void
user_release (user_t *user)
{
  /*保留参数？*/user = user;
  g_mutex_unlock (mutex);
}


static int
append_param (void *string, enum MHD_ValueKind kind, const char *key,
              const char *value)
{
/*保留参数?*/kind = kind;
  if (strcmp (key, "token") && strcmp (key, "r"))
    {
      g_string_append ((GString*) string, key);
      g_string_append ((GString*) string, "=");
      g_string_append ((GString*) string, value);
      g_string_append ((GString*) string, "&");
    }
  return MHD_YES;
}


static gchar *
reconstruct_url (struct MHD_Connection *connection, const char *url)
{
  GString *full_url;

  full_url = g_string_new (url);
  /* To simplify appending the token later, ensure there is at least
   * one param. */
  g_string_append (full_url, "?r=1&");

  MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND,
                             append_param, full_url);

  if (full_url->str[strlen (full_url->str) - 1] == '&')
    full_url->str[strlen (full_url->str) - 1] = '\0';

  return g_string_free (full_url, FALSE);
}


static int handler_send_response (struct MHD_Connection *connection, struct MHD_Response *response, enum content_type *content_type, char *content_disposition, int http_response_code, int remove_cookie)
{
  if (remove_cookie)
    if (remove_sid (response) == MHD_NO)
      {
        MHD_destroy_response (response);
        return MHD_NO;
      }
	gsad_add_content_type_header (response, content_type);
	if (content_disposition != NULL){
		MHD_add_response_header (response, "Content-Disposition", content_disposition);
		free (content_disposition);
	}
	MHD_queue_response (connection, http_response_code, response);
	MHD_destroy_response (response);
	return MHD_YES;
}


static int file_reader (void *cls, uint64_t pos, char *buf, int max)
{
  FILE *file = (FILE *)cls;

  fseek (file, pos, SEEK_SET);
  return fread (buf, 1, max, file);
}


/*记得改成认证信息结构体*/
static struct MHD_Response* file_content_response(void *credentials, struct MHD_Connection *connection, const char *url, int *http_response_code, enum content_type *content_type, char **content_disposition)
{
	/*保留参数?*/credentials = credentials;
	/*保留参数?*/connection = connection;
	content_disposition	= content_disposition;
	FILE* file;
	gchar* path;
	char default_file[] = "login/login.html";
	struct MHD_Response *response;
	char date_2822[100];
	struct tm *mtime;
	time_t next_week;
	printf("url:%s\n",url);
	if(strstr(url, "..")){
		path = g_strconcat(default_file, NULL);
	}
	else{
		const char* relative_url = url;
		if(*url == '/'){
			relative_url = url + 1;
		}
	
		path = g_strconcat(relative_url, NULL);	
		printf("path========================%s\n",(char *)path);
	}
	if(!strcmp(path, default_file)){
		time_t now;
		gchar *xml;
		char *res;
		char ctime_now[200];

		now = time(NULL);
		strcpy(ctime_now,ctime(&now));
		xml = g_strdup_printf("<login_page><token></token><time>%s</time></login_page>", ctime_now);
		res = xsl_transform (xml);
		printf("xml:%s\n",res);
		response = MHD_create_response_from_data(strlen(res), res, MHD_NO, MHD_YES);
		g_free(path);
		g_free(xml);
		return response;
	}
	printf("path:%s\n",path);
	file = fopen(path, "r");
	if(file == NULL){
		printf("not find the file\n");
		g_free(path);
		//return MHD_create_response_from_data(strlen(FILE_NOT_FOUND), )
		return NULL;//需要该
	}
	if(strstr(path, ".png"))
		*content_type = GSAD_CONTENT_TYPE_IMAGE_PNG;
	else if(strstr(path, ".html"))
		*content_type = GSAD_CONTENT_TYPE_TEXT_HTML;
	else if(strstr(path, ".css"))
		*content_type = GSAD_CONTENT_TYPE_TEXT_CSS;

	struct stat buf;
	if(stat(path, &buf)){
		printf("stat error\n");
		g_free(path);
		fclose(file);
		return NULL;
	}

	if((buf.st_mode & S_IFMT) != S_IFREG){//需要改
		printf("no a file\n");
		g_free(path);
		fclose(file);
		return NULL;
	}

	response = MHD_create_response_from_callback(buf.st_size, 32 * 1024, (MHD_ContentReaderCallback) &file_reader, file, (MHD_ContentReaderFreeCallback)&fclose);
	mtime = localtime(&buf.st_mtime);
	if(mtime && strftime(date_2822, 100, "%a, %d %b %Y %H:%M:%S %Z", mtime)){
		MHD_add_response_header (response, "Last-Modified", date_2822);
	}
	
	next_week = time(NULL) + 7*24*60*60;
	mtime = localtime(&next_week);
	if(mtime && strftime(date_2822, 100, "%a, %d %b %Y %H:%M:%S %Z", mtime)){
 	     MHD_add_response_header (response, "Expires", date_2822);
    	}

	  g_free (path);
 	 *http_response_code = MHD_HTTP_OK;
	  return response;
}




int send_redirect_header(struct MHD_Connection *connection, const char* location, user_t *user/*记得改成user的结构体*/)
{
	int ret;
	struct MHD_Response *response;

	response = MHD_create_response_from_data(0, NULL, MHD_NO, MHD_NO);
	if(!response)
		return MHD_NO;

	ret = MHD_add_response_header(response, MHD_HTTP_HEADER_LOCATION, location);
	if(!ret){
		MHD_destroy_response(response);
		return MHD_NO;
	}
	if(user){
		if(attach_sid(response, user->cookie) == MHD_NO){
			MHD_destroy_response(response);
			return MHD_NO;
		}
	}
	MHD_add_response_header(response, MHD_HTTP_HEADER_EXPIRES, "-1");
	MHD_add_response_header(response, MHD_HTTP_HEADER_CACHE_CONTROL, "no-cache");
	ret = MHD_queue_response(connection, MHD_HTTP_SEE_OTHER, response);
	MHD_destroy_response(response);
	return ret;

}

int
send_response (struct MHD_Connection *connection, const char *page,
               int status_code, const gchar *sid)
{
  struct MHD_Response *response;
  int ret;

  response = MHD_create_response_from_data (strlen (page),
                                            (void *) page, MHD_NO, MHD_YES);
  MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                           "text/html; charset=utf-8");
  if (sid)
    {
      if (strcmp (sid, "0"))
        {
          if (attach_sid (response, sid) == MHD_NO)
            {
              MHD_destroy_response (response);
              return MHD_NO;
            }
        }
      else
        {
          if (remove_sid (response) == MHD_NO)
            {
              MHD_destroy_response (response);
              return MHD_NO;
            }
        }
    }
  ret = MHD_queue_response (connection, status_code, response);
  MHD_destroy_response (response);
  return ret;
}

static int params_append_mhd(params_t *params, const char *name, const char *filename, const char *chunk_data, int chunk_size, int chunk_offset)
{
	/*保留参数？*/if(filename){};
	params_append_bin(params, name, chunk_data, chunk_size, chunk_offset);
	return MHD_YES;
}

/*post key-value处理函数*/
int serve_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
	       const char *filename, const char *content_type,
	       const char* transfer_encoding, const char *data,
	       uint64_t off, size_t size)
{
	/*保留参数?*/kind = kind; if(content_type){}; if(transfer_encoding){};
	printf("This is in the serve_post\n");
	struct gsad_connection_info *con_info = (struct gsad_connection_info *)coninfo_cls;
	con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;
	con_info->response = SERVER_ERROR;
	if(NULL != key){
		printf("aaaaaaaaaaaaaaaaaaaaaaaaa\n");
		params_append_mhd(con_info->params, key, filename, data, size, off);
		printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
		con_info->answercode = MHD_HTTP_OK;
		return MHD_YES;
	}
	printf("cccccccccccccccccccccccccccccccccccccc\n");
	return MHD_NO;
}
#if 0
static void params_mhd_validate (void *params)
{
  GHashTableIter iter;
  gpointer name, value;

  g_hash_table_iter_init (&iter, (GHashTable *)params);
  while (g_hash_table_iter_next (&iter, &name, &value))
    {    
     if(strcmp(name+strlen(name), (const char *)"test_file"))
       continue
      param_t *param;
      param = (param_t*) value;

      if (openvas_validate (validator, name, param->value))
        {    
          param->original_value = param->value;
          param->value = NULL;
          param->valid = 0; 
        }    
      else 
        {    
          const gchar *alias_for;

          param->valid = 1; 

          alias_for = openvas_validator_alias_for (validator, name);
          if ((param->value && (strcmp ((gchar*) name, "number") == 0))
              || (alias_for && (strcmp ((gchar*) alias_for, "number") == 0))) 
            /* Remove any leading or trailing space from numbers. */
            param->value = g_strstrip (param->value);
        }

      if (param->values)
        params_mhd_validate_values (name, param->values);
    }
}
#endif



/*post 响应处理函数*/
int exec_omp_post(struct gsad_connection_info *con_info, user_t **user_return,
		  gchar **new_sid)
{
	int ret;
	credentials_t *credentials = NULL;
	const char *cmd, *caller;
//hyj	const char *login;
	const char *token;
	sqlite_user_t tips_user;
	if(con_info->params == NULL){
		printf("wofule\n");
		exit(0);
	}
	printf("in the exec_omp_post\n");
	cmd = params_value(con_info->params, "cmd");
	printf("cmd===========================:%s\n",cmd);
	if(cmd && !strcmp(cmd, "login")){
		printf("before get password\n");
		//tips_user->password = params_value(con_info->params, "password");
		tips_user.password = params_value(con_info->params, "password");
		printf("after get password:\n");
		//if((tips_user->password == NULL) && 
		 //  (params_original_value(con_info->params, "password") == NULL))
		//	tips_user->password = "";
		if((tips_user.username = (gchar *)params_value(con_info->params, "login")) != NULL && (strlen(tips_user.username)) && tips_user.password && strlen(tips_user.password)){
			printf("==========================================000000\n");
			/*核对帐号密码*/
			ret = tips_sqlite_handle.User_login_judge(&tips_user);
			printf("after login judge\n");
			printf("%d\n",ret);
			if(ret == 0){/*密码错误*/
				time_t now;
				gchar *xml;
				char *res;
				char ctime_now[200];
				now = time(NULL);
	          		ctime_r_strip_newline (&now, ctime_now);
				xml = g_strdup_printf("<login_page><message>用户名或密码错误</message></login_page>"/*hyj , ctime_now*/);
				res = xsl_transform(xml);
				g_free(xml);
				con_info->response = res;
				return 0;
			}
			else if(ret == 2){/*登陆被限制*/
				time_t now;
				gchar *xml;
				char *res;
				char ctime_now[200];
				now = time(NULL);
	          		ctime_r_strip_newline (&now, ctime_now);
				xml = g_strdup_printf("<login_page><message>用户登陆已经被限制，请找管理员申请权限</message></login_page>"/*hyj , ctime_now*/);
				res = xsl_transform(xml);
				g_free(xml);
				con_info->response = res;
				return 0;
			}
			else if(ret == 3){/*时间超时*/
				time_t now;
				gchar *xml;
				char *res;
				char ctime_now[200];
				now = time(NULL);
	          		ctime_r_strip_newline (&now, ctime_now);
				xml = g_strdup_printf("<login_page><message>用户使用时间到期，请找管理员延期</message></login_page>"/*hyj , ctime_now*/);
				res = xsl_transform(xml);
				g_free(xml);
				con_info->response = res;
				return 0;
			}
			else{	
				user_t *user;
				printf("aaaaaaaaaaaaaaaaaaa %d\n", __LINE__);
				user = user_add(tips_user.username,
						tips_user.password,
						NULL,
						tips_user.role);
				printf("************************************************************\n");
				tips_sqlite_handle.catch_opt((char *)tips_user.username, (char *)"用户登录成功");
				tips_sqlite_handle.insert_login_time((char *)tips_user.username);
				printf("aaaaaa%d\n", __LINE__);
				*user_return = user;
				printf("%s\n",tips_user.username);
				return 1;
			}
		}
		else{
			time_t now;
			gchar *xml;
			char *res;
			char ctime_now[200];
			now = time(NULL);
	          	ctime_r_strip_newline (&now, ctime_now);
			xml = g_strdup_printf("<login_page><message>密码或帐号不能为空</message></login_page>"/*hyj , ctime_now*/);
			res = xsl_transform(xml);
			g_free(xml);
			con_info->response = res;
			return 0;
		}
	}
	user_t *user = NULL;
	token = params_value(con_info->params, "token");
	printf("token:::%s\n",token);
	ret = user_find (con_info->cookie,
           	        token,
                	&user);
	 if (ret == 2){
		time_t now;
      		gchar *xml;
      		char ctime_now[200];

      		now = time (NULL);
      		ctime_r_strip_newline (&now, ctime_now);
      		caller = params_value (con_info->params, "caller");

      		/* @todo Validate caller. */

      		xml = g_markup_printf_escaped ("<login_page>"
                	                       "<message>"
                          	               "会话超时,请重新登录"
                                    	        "</message>"
                              		        "<token></token>"
                             		        "<time>%s</time>"
                                 		"<url>%s</url>"
                                     		"</login_page>",
                                     		ctime_now,
                                     		caller
                                      		? caller
                                      		: "");
      		con_info->response = xsl_transform (xml);
      		g_free (xml);
      		con_info->answercode = MHD_HTTP_OK;
    		return 2;
    	}

  	if (ret == 3){
      		time_t now;
      		gchar *xml;
      		char ctime_now[200];

      		now = time (NULL);
      		ctime_r_strip_newline (&now, ctime_now);

      		xml = g_strdup_printf ("<login_page>"
                             	       "<message>"
                             	       "Cookie 丢失. 请重新登录."
                             	       "</message>"
                             	       "<token></token>"
                             	       "<time>%s</time>"
                             	       "</login_page>",
                             	       ctime_now);
      		con_info->response = xsl_transform (xml);
      		g_free (xml);
      		con_info->answercode = MHD_HTTP_OK;
      		return 2;
    	}
	ret = tips_sqlite_handle.User_admit_login_judge(user);//判断在使用过程中是不是被限制，或者是使用期限到期
	 if (ret == 2){
		time_t now;
      		gchar *xml;
      		char ctime_now[200];

      		now = time (NULL);
      		ctime_r_strip_newline (&now, ctime_now);
      		caller = params_value (con_info->params, "caller");

      		/* @todo Validate caller. */

      		xml = g_markup_printf_escaped ("<login_page>"
                	                       "<message>"
                          	               "用户使用权限被停止,请找管理员申请授权"
                                    	        "</message>"
                              		        "<token></token>"
                             		        "<time>%s</time>"
                                 		"<url>%s</url>"
                                     		"</login_page>",
                                     		ctime_now,
                                     		caller
                                      		? caller
                                      		: "");
      		con_info->response = xsl_transform (xml);
      		g_free (xml);
      		con_info->answercode = MHD_HTTP_OK;
    		return 2;
    	}
	 else if (ret == 3){
		time_t now;
      		gchar *xml;
      		char ctime_now[200];

      		now = time (NULL);
      		ctime_r_strip_newline (&now, ctime_now);
      		caller = params_value (con_info->params, "caller");

      		/* @todo Validate caller. */

      		xml = g_markup_printf_escaped ("<login_page>"
                	                       "<message>"
                          	               "用户使用时间到期，请找管理员申请延期"
                                    	        "</message>"
                              		        "<token></token>"
                             		        "<time>%s</time>"
                                 		"<url>%s</url>"
                                     		"</login_page>",
                                     		ctime_now,
                                     		caller
                                      		? caller
                                      		: "");
      		con_info->response = xsl_transform (xml);
      		g_free (xml);
      		con_info->answercode = MHD_HTTP_OK;
    		return 2;
    	}
	 else if (ret == 4){
		time_t now;
      		gchar *xml;
      		char ctime_now[200];

      		now = time (NULL);
      		ctime_r_strip_newline (&now, ctime_now);
      		caller = params_value (con_info->params, "caller");

      		/* @todo Validate caller. */

      		xml = g_markup_printf_escaped ("<login_page>"
                	                       "<message>"
                          	               "数据库出错"
                                    	        "</message>"
                              		        "<token></token>"
                             		        "<time>%s</time>"
                                 		"<url>%s</url>"
                                     		"</login_page>",
                                     		ctime_now,
                                     		caller
                                      		? caller
                                      		: "");
      		con_info->response = xsl_transform (xml);
      		g_free (xml);
      		con_info->answercode = MHD_HTTP_OK;
    		return 2;
    	}
	else{}
	;
	
  	credentials = (credentials_t *)malloc (sizeof (credentials_t));
  	credentials->username = strdup (user->username);
  	credentials->role = strdup (user->role);
  	credentials->password = strdup (user->password);
  	credentials->token = strdup (user->token);
  	caller = params_value (con_info->params, "caller");
  	credentials->caller = strdup (caller ? caller : "");

  	if (new_sid) *new_sid = g_strdup (user->cookie);
	if(atoi(credentials->role)){
		CAdmin_opt tips_admin_opt(credentials, (char *)cmd, con_info->params, (void *)response_html);
		con_info->response = tips_admin_opt.cmd_opt_post();
	}
	else{
		CAdmin_opt tips_admin_opt(credentials, (char *)cmd, con_info->params, (void *)response_html_user);
		con_info->response = tips_admin_opt.cmd_opt_post();
	}
	return 0;
}


static int
params_mhd_add (void *params, enum MHD_ValueKind kind, const char *name,
                const char *value)
{
/*保留参数？*/kind = kind;
  params_add ((params_t *) params, name, value);
  return MHD_YES;
}


/*methom 为omp的方法为get的处理函数*/
char *exec_omp_get(struct MHD_Connection *connection,
		   credentials_t *credentials,
		   enum content_type* content_type,
		   gchar **content_type_string,
		   gchar **content_disposition,
		   gsize* response_size)
{
/*保留参数？*/response_size = response_size;
	char *cmd = NULL;
	const unsigned int CMD_MAX_SIZE = 27;		//update by hyj
	params_t *params;
	cmd = (char *)MHD_lookup_connection_value(connection,MHD_GET_ARGUMENT_KIND,"cmd");
	if((cmd != NULL) && strlen(cmd) <= CMD_MAX_SIZE)
	{
		params = params_new();
		MHD_get_connection_values(connection,MHD_GET_ARGUMENT_KIND,params_mhd_add,params);
	}
	else
	{
		return NULL;/*填出错返回的页面*/
	}
	printf("cmd in omp_get:%s\n",cmd);
	if((!strcmp(cmd, "tips_event_opt")) || (!strcmp(cmd, "tips_alarm_opt")) || (!strcmp(cmd, "tips_netlog_opt")))	
	{
		cmd = (char *)MHD_lookup_connection_value(connection,MHD_GET_ARGUMENT_KIND,"tips_operator");
	}
	if(!strcmp(cmd, "tips_output_block")){/*通信阻断报告*/
		CTips_block tips_block_handle;
		block_stut_t block_info;
		tips_block_params(params, &block_info);
		tips_block_handle.Block_init_sql(&block_info);
		char *xml = tips_block_handle.Block_exec_sql();
		printf("wxf=xml=%s\n",xml);
		char *realxml = (char *)g_strdup_printf("<block><title>通信阻断报告</title><condition><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OStart_time>%s</OStart_time><OExpired_time>%s</OExpired_time></condition><result>%s</result></block>",block_info.dip,block_info.sip,  block_info.Sport, block_info.Start_time, block_info.Expired_time,xml);
		g_free(xml);
		*content_type = GSAD_CONTENT_TYPE_APP_XML;
		*content_type_string = g_strdup("application/xml; charset=utf-8");
		*content_disposition = g_strdup("attachment; filename=report.xml");
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出通信阻断审计信息成功");
		return realxml;
	}
	if(!strcmp(cmd, "tips_output_alarm")){/*攻击窃密警报报告*/
		int isfind = atoi(params_value(params, "isfind"));
		char *sortstrtemp = (char *)(params_value(params, "tips_sort"));
		int sort   = 0;
		if(sortstrtemp != NULL){
				sort = atoi(sortstrtemp);
		}
		if(isfind != 0){
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
			CTips_alarm tips_alarm_handle;
                	alarm_stut_t alarm_info;
                	tips_alarm_params(params, &alarm_info);
                	tips_alarm_handle.Alarm_init_sql(&alarm_info,nowpage,sort + 1);
                	char *xml = tips_alarm_handle.Alarm_exec_sql();
                	printf("xml=======%s\n",xml);
			char *realxml = (char *)g_strdup_printf("<attack><msg>导出成功</msg><title>攻击窃密警报</title><condition><OType>%s</OType><ORisk>%s</ORisk><Scc_flag>%s</Scc_flag><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time></condition><result>%s</result></attack>",alarm_info.Type, alarm_info.Risk,alarm_info.Scc_flag, alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, xml);
			g_free(xml);
			*content_type_string = g_strdup("application/xml; charset=utf-8");
			*content_disposition = g_strdup("attachment; filename=report.xml");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出攻击窃密警报信息成功");
			return realxml;
		}
		else{
			char *html = NULL;
			if(atoi(credentials->role)){
				html = response_html(credentials, "<attack><msg>导出失败，请先查询</msg><isfind>0</isfind><tips_sort>0</tips_sort></attack>");
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出攻击窃密警报信息失败");
			}
			else{
				html = response_html_user(credentials, "<attack><isfind>0</isfind><tips_sort>0</tips_sort></attack>");
			}
			return html;
		}
	}
	if(!strcmp(cmd, "tips_opt_output"))
	{
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0)
		{
			CTips_opt tips_opt_handle;
    			opt_stut_t opt_info;
    			opt_info.username = (char *)params_value(params, "tips_username");
    			opt_info.Otime = (char *)params_value(params, "tips_Otime");
      			opt_info.Etime = (char *)params_value(params, "tips_Oetime");
      			opt_info.Optdsc = (char *)params_value(params, "tips_Optdsc");
      			tips_opt_handle.Opt_init_sql(&opt_info, 1);
      			char *xml = tips_opt_handle.Opt_exec_sql(1);
      			char *realxml = (char *)g_strdup_printf("<audit><Ousername><![CDATA[%s]]></Ousername><OOtime><![CDATA[%s]]></OOtime><OOetime><![CDATA[%s]]></OOetime><OOptdsc><![CDATA[%s]]></OOptdsc>%s</audit>",opt_info.username, opt_info.Otime, opt_info.Etime, opt_info.Optdsc, xml);
      			g_free(xml);
			*content_type_string = g_strdup("application/xml; charset=utf-8");
			*content_disposition = g_strdup("attachment; filename=report.xml");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出操作审计信息成功");
			return realxml;
		}
		else{
			char *html = NULL;
			if(atoi(credentials->role)){
				html = response_html(credentials, "<audit><msg>导出失败，请先查询</msg><isfind>0</isfind><tips_sort>0</tips_sort></audit>");
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出攻击窃密警报信息失败");
			}
			else{
				html = response_html_user(credentials, "<audit><isfind>0</isfind><tips_sort>0</tips_sort></audit>");
			}
			return html;
		}
	}
	if(!strcmp(cmd, "tips_output_alarmf")){/*攻击窃密警报报告全*/
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0)
		{
		CTips_alarm tips_alarm_handle;
      alarm_stut_t alarm_info;
      tips_alarm_params(params, &alarm_info);
      tips_alarm_handle.Alarm_init_sql(&alarm_info,1);
      char *xml = tips_alarm_handle.Alarm_exec_sql(1);
      printf("xml=======%s\n",xml);
      char *realxml = (char *)g_strdup_printf("<attack><msg>导出全部成功</msg><title>攻击窃密警报</title><condition><OType>%s</OType><ORisk>%s</ORisk><Scc_flag>%s</Scc_flag><OTrogan_id>%s</OTrogan_id><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac><OAlarm_time>%s</OAlarm_time><OAlarm_end_time>%s</OAlarm_end_time></condition><result>%s</result></attack>",alarm_info.Type, alarm_info.Risk, alarm_info.Scc_flag,alarm_info.Trojan_id, alarm_info.Sip, alarm_info.Dip, alarm_info.Dmac, alarm_info.Alarm_time,alarm_info.Alarm_end_time, xml);
			g_free(xml);
			*content_type_string = g_strdup("application/xml; charset=utf-8");
			*content_disposition = g_strdup("attachment; filename=report.xml");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出攻击窃密警报信息成功");
			return realxml;
		}
		else{
			char *html = NULL;
			if(atoi(credentials->role)){
				html = response_html(credentials, "<attack><msg>导出失败，请先查询</msg><isfind>0</isfind><tips_sort>0</tips_sort></attack>");
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出攻击窃密警报信息失败");
			}
			else{
				html = response_html_user(credentials, "<attack><isfind>0</isfind><tips_sort>0</tips_sort></attack>");
			}
			return html;
		}
	}


	if(!strcmp(cmd, "tips_output_alert")){
		CTips_alert tips_alert_handle;
    alert_stut_t alert_info;
    tips_alert_params(params, &alert_info);
    tips_alert_handle.Alert_init_sql(&alert_info);
    char *xml = tips_alert_handle.Alert_exec_sql();
    printf("xml=======%s\n",xml);
    char *realxml = (char *)g_strdup_printf("<shadiness><title>可疑行为报警</title><condition><Oplugin_id>%s</Oplugin_id><OScc_flag>%s</OScc_flag><ORisk>%s</ORisk><OAlarm_time>%s</OAlarm_time><OSip>%s</OSip><ODip>%s</ODip><ODmac>%s</ODmac></condition><result>%s</result></shadiness>",alert_info.plugin_id, alert_info.Scc_flag, alert_info.Risk, alert_info.Alarm_time, alert_info.Sip, alert_info.Dip, alert_info.Dmac, xml);
     g_free(xml);
		*content_type_string = g_strdup("application/xml; charset=utf-8");
		*content_disposition = g_strdup("attachment; filename=report.xml");
		tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出可疑行为报警信息成功");
		return realxml;
	}
	if(!strcmp(cmd, "tips_output_event")){
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0){
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
			CTips_Event tips_Event_handle;
      event_stut_t event_info;
      tips_event_params(params, &event_info);
      tips_Event_handle.Event_init_sql(&event_info, nowpage);
      char *xml = tips_Event_handle.Event_exec_sql();
      printf("xml=======%s\n",xml);
      char *realxml = (char *)g_strdup_printf("<secret><title>涉密标志事件</title><condition><Otype>%s</Otype><OEvent_time>%s</OEvent_time><OSip>%s</OSip><OSport>%s</OSport><ODip>%s</ODip><OSender>%s</OSender><OReceiver>%s</OReceiver><OSen_msg>%s</OSen_msg></condition><result>%s</result></secret>",event_info.Event_class, event_info.Event_time, event_info.Sip, event_info.Sport, event_info.Dip, event_info.Sender, event_info.Receiver, event_info.Sen_msg,xml);
      g_free(xml);
			*content_type_string = g_strdup("application/xml; charset=utf-8");
			*content_disposition = g_strdup("attachment; filename=report.xml");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出涉密标志事件信息成功");
			return realxml;
		}
		else{
			char *html = NULL;
			if(atoi(credentials->role)){
				html = response_html(credentials, "<secret><msg>导出失败，请先查询</msg><isfind>0</isfind></secret>");
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出涉密标志事件信息失败");
			}
			else{
				html = response_html_user(credentials, "<secret><isfind>0</isfind></secret>");
			}
			return html;
		}
	}
	if(!strcmp(cmd, "tips_output_eventf")){
			CTips_Event tips_Event_handle;
      event_stut_t event_info;
      tips_event_params(params, &event_info);
      tips_Event_handle.Event_init_sql(&event_info, 1);
      char *xml = tips_Event_handle.Event_exec_sql(1);
      printf("xml=======%s\n",xml);
      char *realxml = (char *)g_strdup_printf("<secret><title>涉密标志事件</title><condition><Otype>%s</Otype><OEvent_time>%s</OEvent_time><OSip>%s</OSip><OSport>%s</OSport><ODip>%s</ODip><OSender>%s</OSender><OReceiver>%s</OReceiver><OSen_msg>%s</OSen_msg></condition><result>%s</result></secret>",event_info.Event_class, event_info.Event_time, event_info.Sip, event_info.Sport, event_info.Dip, event_info.Sender, event_info.Receiver, event_info.Sen_msg,xml);
      g_free(xml);
			*content_type_string = g_strdup("application/xml; charset=utf-8");
			*content_disposition = g_strdup("attachment; filename=report.xml");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出涉密标志事件信息成功");
			return realxml;
	}
	if(!strcmp(cmd, "tips_output_netlog")){
		int isfind = atoi(params_value(params, "isfind"));
		if(isfind != 0){
			uint32_t nowpage = atoll(params_value(params, "pagenum"));
			CTips_netlog tips_netlog_handle;
      netlog_stut_t netlog_info;
      tips_netlog_params(params, &netlog_info);
      tips_netlog_handle.Netlog_init_sql(&netlog_info, nowpage);
      char *xml = tips_netlog_handle.Netlog_exec_sql();
      printf("xml=======%s\n",xml);
      char *realxml = (char *)g_strdup_printf("<dialog><title>通信会话审计</title><condition><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload></condition><result>%s</result></dialog>",netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload, xml);
      g_free(xml);
			*content_type_string = g_strdup("application/xml; charset=utf-8");
			*content_disposition = g_strdup("attachment; filename=report.xml");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出通信会话审计信息成功");
			return realxml;
		}
		else{
			char *html = NULL;
			if(atoi(credentials->role)){
				html = response_html(credentials, "<tips_dialog><msg>导出失败，请先查询</msg><isfind>0</isfind></tips_dialog>");
				tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出通信会话审计信息失败");
			}
			else{
				html = response_html_user(credentials, "<tips_dialog><isfind>0</isfind></tips_dialog>");
			}
			return html;
		}
	}
	if(!strcmp(cmd, "tips_output_netlogf")){
			CTips_netlog tips_netlog_handle;
      netlog_stut_t netlog_info;
      tips_netlog_params(params, &netlog_info);
      tips_netlog_handle.Netlog_init_sql(&netlog_info, 1);
      char *xml = tips_netlog_handle.Netlog_exec_sql(1);
      printf("xml=======%s\n",xml);
      char *realxml = (char *)g_strdup_printf("<dialog><title>通信会话审计</title><condition><ODip>%s</ODip><OSip>%s</OSip><OSport>%s</OSport><OProtocol>%s</OProtocol><OStart_time>%s</OStart_time><OEnd_time>%s</OEnd_time><OLoad>%s</OLoad><OUpload>%s</OUpload></condition><result>%s</result></dialog>",netlog_info.Dip, netlog_info.Sip, netlog_info.Sport, netlog_info.Protocol, netlog_info.Start_time, netlog_info.End_time, netlog_info.Load, netlog_info.Upload, xml);
      g_free(xml);
			*content_type_string = g_strdup("application/xml; charset=utf-8");
			*content_disposition = g_strdup("attachment; filename=report.xml");
			tips_sqlite_handle.catch_opt(credentials->username, (char *)"导出通信会话审计信息成功");
			return realxml;
	}
	if(atoi(credentials->role)){
		printf("cmd before in admin %s\n", cmd);
		CAdmin_opt tips_admin_opt(credentials, cmd, params, (void *)response_html);
		return tips_admin_opt.cmd_opt_get();
	}
	else{
		CAdmin_opt tips_admin_opt(credentials, cmd, params, (void *)response_html_user);
		return tips_admin_opt.cmd_opt_get();
	}
}
/*请求处理函数*/
int request_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls)
{
/*保留参数?*/cls = cls; if(version){};
	printf("begin\n");
	//struct sockaddr_in peeraddr;
	//socklen_t addrlen = sizeof(peeraddr);
	
	//int i = getpeername(connection->socket_fd, (sockaddr *)&peeraddr, &addrlen);
	//printf("i=======%d\n",i);
	//printf("error==%s\n",strerror(errno));
	//printf("duifanip111============%s\n",inet_ntoa(connection->addr->sin_addr));
	char ip_buf[24];
	memset(ip_buf, 0, 24);
	//printf("datainfo=============%u\n",((struct sockaddr_in*)(connection->addr))->sin_addr.s_addr);
	struct sockaddr_in *addrs = (struct sockaddr_in *)(connection->addr);
	inet_ntop(AF_INET,&(addrs->sin_addr.s_addr),ip_buf,24);
	printf("ip_buf==========%s\n",ip_buf);

	uint32_t iptemp = htonl(((struct sockaddr_in *)(connection->addr))->sin_addr.s_addr);
#if 0
	char strptr[50];
	memset(strptr,0,50);
	sockaddr_in sin;
	memcpy(&sin,connection->addr,sizeof(sin));
	sprintf(strptr,inet_ntoa(sin.sin_addr));
	printf("ip====%s\n",strptr);
	
	printf("duifanip============%u\n",iptemp);
	printf("duifanip11============%u\n",ntohl(((struct sockaddr_in *)(connection->addr))->sin_addr.s_addr));
	//printf("duifanip============%u\n",connection->addr->sin_addr.s_addr);
	//printf("fjeifjie\n");
	printf("duifanip222============%s\n",inet_ntop(AF_INET,(void *)&((struct sockaddr_in *)(connection->addr))->sin_addr.s_addr,strptr,INET_ADDRSTRLEN));//为转换成主机字节序
	printf("duifanip333============%s\n",inet_ntop(AF_INET,(void *)&iptemp,strptr,INET_ADDRSTRLEN));
	printf("duifanip============%s\n",inet_ntop(AF_INET,&((struct sockaddr_in*)(connection->addr))->sin_addr.s_addr,strptr,INET_ADDRSTRLEN));
	exit(0);
#endif
	if(managecp_ip.judge_login_ip(iptemp)){
		return MHD_NO;
	}
	char default_file[] = "/login/login.html";
	enum content_type content_type;
	credentials_t *credentials = NULL;		/*需要该*/
	int http_response_code = MHD_HTTP_OK;
	char *content_disposition = NULL;
	struct MHD_Response *response = NULL;
	int ret;
	gsize response_size = 0;
	if(url == NULL){
		printf("url is null\n");
	}
	else{
		printf("url:%s\n",url);
	}
	printf("method out of new:%s\n",method);
	if((!strcmp(method, "GET")) && (*con_cls == NULL))
	{
		printf("get param===================\n");
		printf("method:%s\n",method);
		struct gsad_connection_info *con_info;
		con_info = (gsad_connection_info*)calloc(1, sizeof(struct gsad_connection_info));
		if(NULL == con_info)
			return MHD_NO;
			printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
		con_info->params = params_new();
		con_info->connectiontype = 2;
		
		*con_cls = (void *)con_info;
		return MHD_YES;
	}
	if (&url[0] == NULL)
	    return MHD_NO;
	if (strcmp (method, "GET") && strcmp (method, "POST")){
     		send_response (connection, ERROR_PAGE, MHD_HTTP_METHOD_NOT_ACCEPTABLE,
                     NULL);
      		return MHD_YES;
    	}
	if((!strcmp(&(url[0]), "/login")) || (!strcmp(&(url[0]), "/")) 
		|| (!strcmp(url,"/login/")))
	//if((!strcmp(url, "/login")))
	if((send_redirect_header(connection, default_file, NULL))==MHD_YES){
		printf("redirect success\n");
		return MHD_YES;
	}
	printf("33333333333333333333333333333333333333333333333333\n");


	if (!strcmp (method, "GET")){
	      printf("=====================get================================\n");
	      const char *token  = NULL;
	      const char *cookie = NULL;
	      user_t *user;
	      gchar *sid;

      /* Second or later call for this request, a GET. */

	      content_type = GSAD_CONTENT_TYPE_TEXT_HTML;
	
		/* Special case the login page, stylesheet and icon. */

	      if (!strcmp (url, default_file)){
	          response = file_content_response (credentials,
                                	            connection, url,
                        	                    &http_response_code,
                	                            &content_type,
        	                                    &content_disposition);
	          return handler_send_response (connection,
        	                                response,
	                                        &content_type,
                                	        content_disposition,
                        	                http_response_code,
                	                        1);
        	}
	      if (!strcmp (url, "/gsa-style.css")
	      	  || !strcmp (url, "/IE6fixes.css")
	          || !strcmp (url, "/logo.gif")|| !strcmp (url, "/script/jquery-1.4.1.min.js") || !strcmp (url, "/script/jquery-1.6.2.min.js")|| !strcmp (url, "/script/script.js")|| !strcmp (url, "/script/jquery.js")|| !strcmp (url, "/script/jscharts.js")|| !strcmp (url, "/script/My97DatePicker/calendar.js")|| !strcmp (url, "/script/My97DatePicker/config.js")|| !strcmp (url, "/script/My97DatePicker/WdatePicker.js")||!strcmp (url, "/script/My97DatePicker/My97DatePicker.htm")|| !strcmp (url, "/script/My97DatePicker/lang/en.js")|| !strcmp (url, "/script/My97DatePicker/lang/zh-cn.js")|| !strcmp (url, "/script/My97DatePicker/lang/zh-tw.js")|| !strcmp (url, "/script/My97DatePicker/skin/datePicker.gif")|| !strcmp (url, "/script/My97DatePicker/skin/WdatePicker.css")|| !strcmp (url, "/script/My97DatePicker/skin/default/datepicker.css")|| !strcmp (url, "/script/My97DatePicker/skin/default/img.gif")|| !strcmp (url, "/script/My97DatePicker/skin/whyGreen/img.gif")|| !strcmp (url, "/script/My97DatePicker/skin/whyGreen/datepicker.css")|| !strcmp (url, "/script/My97DatePicker/skin/whyGreen/bg.jpg")){
	      
	          response = file_content_response (credentials,
                                        	    connection, url,
                                	            &http_response_code,
                        	                    &content_type,
                	                            &content_disposition);
        	  return handler_send_response (connection,
	                                        response,
                                        	&content_type,
                                	        content_disposition,
                        	                http_response_code,
                	                        0);
        	}

	      /* Allow the decorative images to anyone. */

      		if(strncmp (url, "/img/", strlen ("/img/")) == 0){
		printf("444444444444444444444444444444444444444444444444444444\n");
	          response = file_content_response (credentials,
                                	            connection, url,
                        	                    &http_response_code,
                	                            &content_type,
        	                                    &content_disposition);
	          return handler_send_response (connection,
        	                                response,
	                                        &content_type,
                                	        content_disposition,
                        	                http_response_code,
                	                        0);
        	}

		token = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "token");
		if(token == NULL){
			/*返回出错信息*/
		}
		
		cookie = MHD_lookup_connection_value(connection, MHD_COOKIE_KIND,"SID");
		ret = user_find(cookie, token, &user);
		printf("cookie=%s\n",cookie);
		printf("token=%s\n",token);
		if ((ret == 2) || (ret == 3)){
	          time_t now;
        	  gchar *xml;
	          char *res;
	          gchar *full_url;
	          char ctime_now[200];
	          const char *cmd, *report_format_id;
	          int exportv;

	          now = time (NULL);
	          ctime_r_strip_newline (&now, ctime_now);

	          cmd = MHD_lookup_connection_value (connection,
	                                             MHD_GET_ARGUMENT_KIND,
	                                             "cmd");
	          report_format_id = MHD_lookup_connection_value (connection,
        	                                                  MHD_GET_ARGUMENT_KIND,
                	                                          "report_format_id");
	          exportv = cmd ? 1: 0;

	          full_url = (gchar *)reconstruct_url(connection, url);
        	  xml = g_markup_printf_escaped
                	 ("<login_page>"
	                  "<message>"
        	          "%s"
                	  "</message>"
	                  "<token></token>"
        	          "<time>%s</time>"
        	          "<url>%s</url>"
        	          "</login_page>",
        	          ((ret == 2)
	                    ? (strncmp (url, "/logout", strlen ("/logout"))
        	                ? "会话超时，请重新登录"
                	        : "已经注销")
	                    : "Cookie 丢失，请重新登录"),
        	          ctime_now,
	                  (((exportv == 0)
	                    && strncmp (url, "/logout", strlen ("/logout")))
	                    ? full_url
	                    : ""));
	          g_free (full_url);
	          res = xsl_transform (xml);
	          g_free (xml);
	          response = MHD_create_response_from_data (strlen (res), res,
                                                    MHD_NO, MHD_YES);
	          return handler_send_response (connection,
        	                                response,
                	                        &content_type,
                        	                content_disposition,
                                	        http_response_code,
	                                        1);
        	}
		ret = tips_sqlite_handle.User_admit_login_judge(user);
		if ((ret == 2) || (ret == 3)){
	          time_t now;
        	  gchar *xml;
	          char *res;
	          gchar *full_url;
	          char ctime_now[200];
	          const char *cmd, *report_format_id;
	          int exportv;

	          now = time (NULL);
	          ctime_r_strip_newline (&now, ctime_now);

	          cmd = MHD_lookup_connection_value (connection,
	                                             MHD_GET_ARGUMENT_KIND,
	                                             "cmd");
	          report_format_id = MHD_lookup_connection_value (connection,
        	                                                  MHD_GET_ARGUMENT_KIND,
                	                                          "report_format_id");
	          exportv = cmd ? 1: 0;

	          full_url = (gchar *)reconstruct_url(connection, url);
        	  xml = g_markup_printf_escaped
                	 ("<login_page>"
	                  "<message>"
        	          "%s"
                	  "</message>"
	                  "<token></token>"
        	          "<time>%s</time>"
        	          "<url>%s</url>"
        	          "</login_page>",
        	          ((ret == 2)
	                    ? "用户使用权限被停止,请找管理员申请授权"
	                    : "用户使用时间到期，请找管理员申请延期"),
        	          ctime_now,
	                  (((exportv == 0)
	                    && strncmp (url, "/logout", strlen ("/logout")))
	                    ? full_url
	                    : ""));
	          g_free (full_url);
	          res = xsl_transform (xml);
	          g_free (xml);
	          response = MHD_create_response_from_data (strlen (res), res,
                                                    MHD_NO, MHD_YES);
	          return handler_send_response (connection,
        	                                response,
                	                        &content_type,
                        	                content_disposition,
                                	        http_response_code,
	                                        1);
        	}
		printf("dashuaige==============================\n");
		/*登出操作还没做*/
		if (!strncmp (url, "/logout", strlen ("/logout"))){
          		time_t now;
          		gchar *xml;
          		char *res;
          		char ctime_now[200];

          		now = time (NULL);
          		ctime_r_strip_newline (&now, ctime_now);

							tips_sqlite_handle.catch_opt(user->username, (char *)"用户注销成功");

          		user_remove (user);

          		xml = g_strdup_printf ("<login_page>"
                        		         "<message>"
		                                 "成功注销."
                		                 "</message>"
                                		 "<token></token>"
		                                 "<time>%s</time>"
                		                 "</login_page>",
                                		 ctime_now);
          		res = xsl_transform (xml);
          		g_free (xml);
          		response = MHD_create_response_from_data (strlen (res), res,
                        		                            MHD_NO, MHD_YES);
          		return handler_send_response (connection,
                        		              response,
                                        	      &content_type,
                                       		      content_disposition,
                                        	      http_response_code,
                                                      1);
        	}

		credentials = (credentials_t *)malloc(sizeof(credentials_t));
		if(credentials == NULL) abort();
		credentials->username = strdup(user->username);
		credentials->role = strdup(user->role);
		credentials->password = strdup(user->password);
		credentials->token = strdup(user->token);
		credentials->caller = (gchar *)reconstruct_url(connection, url);
		sid = g_strdup(user->cookie);
		//user_release(user);
		/*自己组装xml*/
		/*char tipxml[] = "<html-head></html-head>";
	         char* res = xsl_transform (tipxml);
		printf("%s\n",res);
	         response = MHD_create_response_from_data (strlen (res), res,
                                                    MHD_NO, MHD_YES);
	          return handler_send_response (connection,
        	                                response,
                	                        &content_type,
                        	                content_disposition,
                                	        http_response_code,
	                                        0);
		*/
		gchar *content_type_string = NULL;
		char *res = exec_omp_get(connection, credentials, &content_type, &content_type_string, &content_disposition, &response_size);
		if(res != NULL){
		 printf("html================%s\n",res);
	         response = MHD_create_response_from_data (strlen (res), res,
                                                    MHD_NO, MHD_YES);
		 if (content_type_string){
             	     MHD_add_response_header(response, 
					     MHD_HTTP_HEADER_CONTENT_TYPE,
                                             content_type_string);
              	     g_free (content_type_string);
                  }
		  /*if(content_disposition){
             	     MHD_add_response_header(response, 
					     "Content-Disposition",
                                             content_disposition);
              	     g_free (content_disposition);
		  }*/
		  g_free(res);
		  if (response){
     		     if (attach_sid (response, sid) == MHD_NO){
        		      g_free (sid);
              		      MHD_destroy_response (response);
              		      return MHD_NO;
            	      }
          	      g_free (sid);
	              return handler_send_response (connection,
        	                                    response,
                	                            &content_type,
                        	                    content_disposition,
                                	            http_response_code,
	                                            1);
		  }
		}else{
			if((send_redirect_header(connection, default_file, NULL))==MHD_YES){
				printf("redirect success\n");
				return MHD_YES;
			}
			return MHD_NO;
		}
	}
	if(!strcmp(method, "POST")){
		printf("=====================post================================\n");
		user_t *user;
		gchar *new_sid;
		if(NULL == *con_cls){
			printf("The first post\n");
			struct gsad_connection_info *con_info;
			con_info = (gsad_connection_info*)calloc(1, sizeof(struct gsad_connection_info));
			if(NULL == con_info)
				return MHD_NO;
			printf("before the serve_post\n");
			con_info->postprocessor = MHD_create_post_processor(connection, 500000, serve_post, (void *)con_info);
      printf("after the serve_post\n");
			if(NULL == con_info->postprocessor)
				return MHD_NO;
			printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
			con_info->params = params_new();//初始化一个用来存参数的HASH表
			con_info->connectiontype = 1;
			con_info->answercode = MHD_HTTP_OK;
			*con_cls = (void *)con_info;
			return MHD_YES;
		}
		printf("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkk\n");
		struct gsad_connection_info *con_info = (gsad_connection_info*)*con_cls;
		if(0 != *upload_data_size){
			MHD_post_process(con_info->postprocessor, upload_data, 
					*upload_data_size);
			*upload_data_size = 0;
			return MHD_YES;
			/*调用MHD_create_post_processor注册的serve_post处理参数*/
		}
		const char *sid = MHD_lookup_connection_value(connection,
							MHD_COOKIE_KIND,
							"SID");
		con_info->cookie = g_strdup(sid);
		const char *token = params_value(con_info->params,
							"token");
		printf("token==%s\n",token);
		new_sid = NULL;
		printf("before into exec_omp_post\n");
		int ret = exec_omp_post(con_info, &user, &new_sid);
		printf("ret:%d\n",ret);
		if(ret == 1){/*验证成功，转到相应的url*/
			if(user == NULL){
				ret = user_find(sid, token, &user);
			}
			gchar *url;
			url = g_strdup_printf("%s&token=%s",params_value(con_info->params, "text"),user->token);
			if((send_redirect_header(connection, url, user))==MHD_YES){
				printf("redirect success\n");
				return MHD_YES;
			}

			//send_redirect_header(connection, url, NULL);
			printf("post:%s\n",url);
			g_free(url);
			return MHD_YES;
		}
		if(NULL != con_info->response){
			ret = send_response(connection, con_info->response, MHD_HTTP_OK,
					    new_sid ? new_sid : "0");
				return MHD_YES;
		}
		else{
			if((send_redirect_header(connection, default_file, NULL))==MHD_YES){
				printf("redirect success\n");
				return MHD_YES;
			}
			return MHD_NO;
		}
	}
	return MHD_YES;
}


void
free_resources (void *cls, struct MHD_Connection *connection,
                void **con_cls, enum MHD_RequestTerminationCode toe)
{
/*保留参数?*/cls = cls; connection = connection; toe = toe;
printf("This in the free_resources\n");
  struct gsad_connection_info *con_info =
    (struct gsad_connection_info *) *con_cls;

  if (NULL == con_info)
    {
      return;
    }
printf("kkkkkkkkkkkkkkkkkkkkkkkkkk\n");
  if (con_info->connectiontype == 1)
    {
      if (NULL != con_info->postprocessor)
        {
          MHD_destroy_post_processor (con_info->postprocessor);
        }
    }
printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
if(con_info->params == NULL)
	printf("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz\n");
  //params_free (con_info->params);
printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyy\n");
  free (con_info->cookie);
  free (con_info);
  *con_cls = NULL;
 printf("leave the free\n");
}

int main(int argc, char *argv[])
{
/*保留参数?*/argc = argc;
	//fclose(stdout);
	char id_buf[10];
	memset(id_buf,0,10);
	strcpy(ETH_USER,argv[1]);
	pid_t pid = getpid();
//hyj	pid_t pid2=0;
	sprintf(id_buf,"%d",pid);
	//printf("pid===%d\n",pid);
	FILE *fp = fopen("/usr/local/tipschecker/etc/tipscheck/userpid", "w");
	if(NULL == fp){
		printf("can not open /usr/local/tipschecker/etc/tipscheck/pid_user\n");
		exit(0);
	}
	fwrite(id_buf,strlen(id_buf),1,fp);
	fclose(fp);
	//fwrite(&pid, 1, sizeof(pid), fp);
	//fclose(fp);
	/*fp = fopen("/etc/tipscheck/pid_user", "r");
	fread(&pid2, 1, sizeof(pid), fp);
	fclose(fp);
	printf("pid2===%d\n",pid2);*/
	if (!g_thread_supported ()) g_thread_init (NULL);
 	if (mutex == NULL)
  		  mutex = g_mutex_new ();
 	if (filemutex == NULL)
  		  filemutex = g_mutex_new ();
	 users = g_ptr_array_new ();
				//tips_address.sin_addr.s_addr = get_local_ip((char *)"eth2");
				//tips_address.sin_addr.s_addr = get_local_ip((char *)"eth0");//获取IP函数
				tips_address.sin_addr.s_addr = get_local_ip(ETH_USER);//获取IP函数
				if(-1 == (int)tips_address.sin_addr.s_addr){			//update by hyj
					printf("获取%s IP失败\n",ETH_USER);
					return -1;
				}
	char buf[20];
	memset(buf, 0, 20);
	inet_ntop(AF_INET, &(tips_address.sin_addr.s_addr),buf,20),
	printf("IP====%s\n",buf);
  tips_address.sin_family = AF_INET;
  tips_address.sin_port = htons(12300);
	managecp_ip.read_manage_ip();
	gchar *ssl_private_key = NULL;
	gchar *ssl_certificate = NULL;
	printf("xxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
	if(!g_file_get_contents("/usr/local/var/lib/openvas/private/CA/serverkey.pem", &ssl_private_key, NULL , NULL)){
		printf("get ssl_private_key error\n");
		exit(0);
	}
	if(!g_file_get_contents("/usr/local/var/lib/openvas/CA/servercert.pem", &ssl_certificate, NULL , NULL)){
		printf("get ssl_private_key error\n");
		exit(0);
	}
	int retmem = obj.Init_shm();
	if(-1 == retmem){
		printf("Init_shm failed\n");
		return -1;
	}
	printf("ssl_private_key:%s\n",ssl_private_key);
	printf("ssl_certificate:%s\n",ssl_certificate);
	/*开启一个httpd，MHD_USE_THERAD_PER_CONNECTION表示每一个请求都用一个子线程处理，MHD_USE_DEBUG表示打印出错信息，服务端口号：12300，请求响应函数为request_handler,MHD_OPTION_END表示传参结束*/
	struct MHD_Daemon* tips_daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG | MHD_USE_SSL, 12300, NULL, NULL, request_handler, NULL,MHD_OPTION_HTTPS_MEM_KEY, ssl_private_key, MHD_OPTION_HTTPS_MEM_CERT, ssl_certificate, MHD_OPTION_NOTIFY_COMPLETED, free_resources, NULL,MHD_OPTION_SOCK_ADDR, &tips_address, MHD_OPTION_END);
	//struct MHD_Daemon* tips_daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG, 12300, NULL, NULL, request_handler, NULL, MHD_OPTION_NOTIFY_COMPLETED, free_resources, NULL,MHD_OPTION_SOCK_ADDR, &tips_address, MHD_OPTION_END);
	  //struct MHD_Daemon* tips_daemon =
    //MHD_start_daemon (MHD_USE_SELECT_INTERNALLY | MHD_USE_SSL, 12300, NULL,
    //                  NULL, request_handler, NULL,
     //                 MHD_OPTION_HTTPS_MEM_KEY, ssl_private_key,
      //                MHD_OPTION_HTTPS_MEM_CERT, ssl_certificate, MHD_OPTION_END);
	if(tips_daemon == NULL){
		printf("strart daemon error\n");
		exit(0);
	}
	int flag= fcntl(tips_daemon->socket_fd, F_GETFD);
	flag |= FD_CLOEXEC;
	printf("aaaaaaaaaaaaaaaaaaaaaaaaa\n");
	int i = fcntl(tips_daemon->socket_fd, F_SETFD, flag);
	printf("i===================================%d\n", i);
	while (1)
    {
      select (0, NULL, NULL, NULL, NULL);
    }
	return 0;
}
