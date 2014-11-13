	if ((strncmp (name, "tipsnvt:", strlen ("tipsnvt:")) == 0))
    {
	  if((NULL == filename) || (0 == strlen(filename)))
		  return MHD_YES;
      param_t *param;
      const char *colon;
      gchar *prefix;

      colon = strchr (name, ':');

      prefix = g_strndup (name, 1 + colon - name);
      param = params_get (params, prefix);

      if (param == NULL)
        {
          param = params_add (params, prefix, "");
          param->values = params_new ();
					param->filelen = 0;
        }
      else if (param->values == NULL)
        param->values = params_new ();
	  //printf("lastname=%s\n",prefix);
      g_free (prefix);
	  //printf("colon===%s\n",colon+1);
	  	param->filelen += chunk_size;
      params_append_bin (param->values, filename, chunk_data, chunk_size,
                         chunk_offset);
	  //char *aaa = (char *)params_value(param->values, filename);
	  //printf("aaaaaaaaaaaaaaadadada==%s\n", aaa);
      /*if (filename){
        param->filename = g_strdup (filename);
	  }*/
	  //printf("gggggggggggg=%s\n", filename);
      return MHD_YES;
    }
