/* Greenbone Security Assistant
 * $Id$
 * Description: Base functionalities of GSA.
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
 * @file gsad_base.c
 * @brief Base functionality of GSA.
 */

/**
 * @brief Location of XSL file.
 */
#define XSL_PATH "gsad.xsl"

#include "gsad_base.h"
#include "tracef.h"

#include <glib.h>
#include <libxml/parser.h>
#include <libexslt/exslt.h>
#include <string.h> /* for strlen() */
#include <libxslt/xsltInternals.h> /* for xsltStylesheetPtr */
#include <libxslt/transform.h> /* for xsltApplyStylesheet() */
#include <libxslt/xsltutils.h> /* for xsltSaveResultToString() */
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#undef G_LOG_DOMAIN
/**
 * @brief GLib log domain.
 */
#define G_LOG_DOMAIN "gsad base"

/**
 * @brief Base init.
 *
 * @return 0 success, 1 XML needs thread support.
 */
int
gsad_base_init ()
{
  if (!xmlHasFeature (XML_WITH_THREAD))
    return 1;
  /* Required by libxml for thread safety. */
  xmlInitParser ();
  return 0;
}

/**
 * @brief Return string from ctime_r with newline replaces with terminator.
 *
 * @param[in]  time    Time.
 * @param[out] string  Time string.
 *
 * @return Return from ctime_r applied to time, with newline stripped off.
 */
char*
ctime_r_strip_newline (time_t *time, char *string)
{
  struct tm *tm;

  tm = localtime (time);
  char buf[40];
  char day[100];
 memset(day,0,sizeof(day));
 sprintf(day,"%d年%d月%d日",(1900+tm->tm_year),(1+tm->tm_mon),tm->tm_mday);
 memset(string,0,sizeof(string));
 strcpy(string,day);
 memset(day,0,sizeof(day));
 strftime(day,sizeof(day)," %H:%M:%S",tm);
 strcat(string,day);
 strftime(buf,sizeof(buf),"%u",tm);
 memset(day,0,sizeof(day));
  switch(atoi(buf))
  {   
          case 1:
                  sprintf(day," 星期%s","一");
                  break;
          case 2:
                  sprintf(day," 星期%s","二");
                  break;
          case 3:
                  sprintf(day," 星期%s","三");
                  break;
          case 4:
                  sprintf(day," 星期%s","四");
                  break;
          case 5:
                  sprintf(day," 星期%s","五");
                  break;
          case 6:
                  sprintf(day," 星期%s","六");
                  break;
          case 7:
                  sprintf(day," 星期%s","日");
                  break;
   }
  strcat(string,day);
#if 0
   memset(string,0,sizeof(string));
  if (tm == NULL
      || (strftime (string,
                    199,
                    "%Y-%m-%d %H:%M:%S",
                    tm)
          == 0))
    {
      string[0] = '\0';
      return string;
    }
  strcat(string,day);
#endif
  return string;
}

/**
 * @brief XSL Transformation.
 *
 * Does the transformation from XML to HTML applying omp.xsl.
 *
 * @param[in]  xml_text  The XML text to tranform.
 *
 * @return HTML output from XSL transformation, or NULL on error.
 */
char *
xsl_transform (const char *xml_text)
{
  xsltStylesheetPtr cur = NULL;
  xmlDocPtr doc, res;
  xmlChar *doc_txt_ptr = NULL;
  int doc_txt_len;

  exsltRegisterAll ();

  xmlSubstituteEntitiesDefault (1);
  xmlLoadExtDtdDefaultValue = 1;
  cur = xsltParseStylesheetFile ((const xmlChar *) XSL_PATH);
  if (cur == NULL)
    {
      g_warning ("Failed to parse stylesheet " XSL_PATH);
      return NULL;
    }

  doc = xmlParseMemory (xml_text, strlen (xml_text));
  if (doc == NULL)
    {
      g_warning ("Failed to parse stylesheet " XSL_PATH);
      xsltFreeStylesheet (cur);
      return NULL;
    }

  res = xsltApplyStylesheet (cur, doc, NULL);
  if (res == NULL)
    {
      g_warning ("Failed to apply stylesheet " XSL_PATH);
      xsltFreeStylesheet (cur);
      return NULL;
    }

  if (xsltSaveResultToString (&doc_txt_ptr, &doc_txt_len, res, cur) < 0)
    {
      g_warning ("Failed to store transformation result.");
      xsltFreeStylesheet (cur);
      xmlFreeDoc (res);
      xmlFreeDoc (doc);
      return NULL;
    }

  xsltFreeStylesheet (cur);
  xmlFreeDoc (res);
  xmlFreeDoc (doc);

  xsltCleanupGlobals ();
  xmlCleanupParser ();
  return (char *) doc_txt_ptr;
}

/**
 * @brief Handles fatal errors.
 *
 * @todo Make it accept formatted strings.
 *
 * @param[in]  credentials  User authentication information.
 * @param[in]  title     The title for the message.
 * @param[in]  function  The function in which the error occurred.
 * @param[in]  line      The line number at which the error occurred.
 * @param[in]  msg       The response message.
 * @param[in]  backurl   The URL offered to get back to a sane situation.
 *                       If NULL, the tasks page is used.
 *
 * @return An HTML document as a newly allocated string.
 */
char *
gsad_message (credentials_t *credentials, const char *title,
              const char *function, int line, const char *msg,
              const char *backurl)
{
  gchar *xml, *resp;

  xml = g_strdup_printf ("<gsad_response>"
                         "<title>%s: %s:%i</title>"
                         "<message>%s</message>"
                         "<backurl>%s</backurl>"
                         "<token>%s</token>"
                         "</gsad_response>",
                         title,
                         function,
                         line,
                         msg,
                         backurl ? backurl : "/omp?cmd=get_tasks",
                         credentials ? credentials->token : "");
  resp = xsl_transform (xml);
  if (resp == NULL)
    resp = g_strdup ("<html>"
                     "<body>"
                     "An internal server error has occurred during XSL"
                     " transformation."
                     "</body>"
                     "</html>");
  g_free (xml);
  return resp;
}


/* Params. */

/**
 * @brief Free a param.
 *
 * @param[in]  param  Param.
 */
static void
param_free (gpointer param)
{
  g_free (((param_t*) param)->value);
  g_free (((param_t*) param)->original_value);
  g_free (((param_t*) param)->filename);
  g_free (param);
}

/**
 * @brief Make a params.
 *
 * @return Freshly allocated params.  Free with params_free.
 */
params_t *
params_new ()
{
  return g_hash_table_new_full (g_str_hash, g_str_equal, g_free, param_free);
}

/**
 * @brief Make a params.
 *
 * @param[in]  Params.
 */
void
params_free (params_t *params)
{
  if (params)
    g_hash_table_destroy (params);
}

/**
 * @brief Get param.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return Param if present, else NULL.
 */
param_t *
params_get (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param;
}

/**
 * @brief Get whether a param was given at all.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return 1 if given, else 0.
 */
int
params_given (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param ? 1 : 0;
}

/**
 * @brief Get value of param.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return Value if param present, else NULL.
 */
const char *
params_value (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param ? param->value : NULL;
}

/**
 * @brief Get the size of the value of param.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return Size if param present, else -1.
 */
int
params_value_size (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param ? param->value_size : -1;
}

/**
 * @brief Get original value of param, before validation.
 *
 * Only set if validation failed.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return Value if param present, else NULL.
 */
const char *
params_original_value (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param ? param->original_value : NULL;
}

/**
 * @brief Get filename of param.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return Filename if param present and has a filename, else NULL.
 */
const char *
params_filename (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param ? param->filename : NULL;
}

/**
 * @brief Get values of param.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return Values if param present, else NULL.
 */
params_t *
params_values (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param ? param->values : NULL;
}

/**
 * @brief Get whether a param is valid.
 *
 * @param[in]  Params.
 * @param[in]  Name.
 *
 * @return 1 if param present and valid, else 0.
 */
int
params_valid (params_t *params, const char *name)
{
  param_t *param;
  param = (param_t *)g_hash_table_lookup (params, name);
  return param ? param->valid : 0;
}

/**
 * @brief Add a param.
 *
 * @param[in]  params  Params.
 * @param[in]  name    Name.
 * @param[in]  value   Value.  Must be a string.
 */
param_t *
params_add (params_t *params, const char *name, const char *value)
{
  param_t *param;
  param = (param_t *)g_malloc0 (sizeof (param_t));
  param->valid = 0;
  param->value = g_strdup (value);
  param->value_size = strlen (value);
  g_hash_table_insert (params, g_strdup (name), param);
  return param;
}

/**
 * @brief Append binary data to a param.
 *
 * Appended data always has an extra NULL terminator.
 *
 * @param[in]  params        Params.
 * @param[in]  name          Name.
 * @param[in]  chunk_data    Data to append.
 * @param[in]  chunk_size    Number of bytes to copy.
 * @param[in]  chunk_offset  Offset in bytes into data from which to start.
 *
 * @return Param appended to, or NULL on memory error.
 */
param_t *
params_append_bin (params_t *params, const char *name, const char *chunk_data,
                   int chunk_size, int chunk_offset)
{
  param_t *param;
  char *new_value;

  param = params_get (params, name);

  if (param == NULL)
    {
      char *value;

      value = (char *)malloc (chunk_size + 1);
      if (value == NULL)
        return NULL;
      memcpy (value + chunk_offset,
              chunk_data,
              chunk_size);
      value[chunk_offset + chunk_size] = '\0';

      param = params_add (params, name, "");
      g_free (param->value);
      param->value = value;
      param->value_size = chunk_size;
      return param;
    }

  new_value = (char *)realloc (param->value,
                       param->value_size + chunk_size + 1);
  if (new_value == NULL)
    return NULL;
  param->value = new_value;
  memcpy (param->value + chunk_offset,
          chunk_data,
          chunk_size);
  param->value[chunk_offset + chunk_size] = '\0';
  param->value_size += chunk_size;

  return param;
}

/**
 * @brief Increment a params iterator.
 *
 * @param[in]   iterator  Iterator.
 * @param[out]  name      Name of param.
 * @param[out]  param     Param.
 *
 * @return TRUE if there was a next element, else FALSE.
 */
gboolean
params_iterator_next (params_iterator_t *iterator, char **name,
                      param_t **param)
{
  return g_hash_table_iter_next (iterator, (gpointer*) name, (gpointer*) param);
}
