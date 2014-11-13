<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output
      method="html"
      doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"
      doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
      encoding="UTF-8"/>

<!-- HEADERS, FOOTER, SIDEBARS -->

<xsl:template name="html-head">
  <head>
    <link rel="stylesheet" type="text/css" href="/gsa-style.css"/> 
    <script src="/script/jquery-1.4.1.min.js" language="javascript" type="text/javascript"></script>
    <script src="/script/script.js" language="javascript"  type="text/javascript"></script>

    <!-- Internet Explorer CSS Fixes -->
    <xsl:comment>[if IE 6]&gt;
      <!-- HACK: Since this will become a comment for the webserver,
           URLS are not rewritten within here. Try some locations so
           one should match. This is ugly, but IE6 is too.
      -->
        &lt;link rel="stylesheet" type="text/css" href="IE6fixes.css"/&gt;
        &lt;link rel="stylesheet" type="text/css" href="../IE6fixes.css"/&gt;
    &lt;![endif]</xsl:comment>

    <xsl:apply-templates select="envelope/autorefresh" mode="html-header-meta" />

  </head>
</xsl:template>

<xsl:template match="autorefresh" mode="html-header-meta">
  <xsl:if test="@interval &gt; 0">
   <xsl:choose>
    <xsl:when test="(//nowpage mod 10) = 1">
     <meta http-equiv="refresh" content="{@interval};{//tips_url1}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 2">
     <meta http-equiv="refresh" content="{@interval};{//tips_url2}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 3">
     <meta http-equiv="refresh" content="{@interval};{//tips_url3}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 4">
     <meta http-equiv="refresh" content="{@interval};{//tips_url4}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 5">
     <meta http-equiv="refresh" content="{@interval};{//tips_url5}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 6">
     <meta http-equiv="refresh" content="{@interval};{//tips_url6}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 7">
     <meta http-equiv="refresh" content="{@interval};{//tips_url7}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 8">
     <meta http-equiv="refresh" content="{@interval};{//tips_url8}" />
    </xsl:when>
    <xsl:when test="(//nowpage mod 10) = 9">
     <meta http-equiv="refresh" content="{@interval};{//tips_url9}" />
    </xsl:when>
    <xsl:otherwise>
     <meta http-equiv="refresh" content="{@interval};{//tips_url10}" />
		</xsl:otherwise>
   </xsl:choose>
  </xsl:if>
</xsl:template>


<xsl:template name="html-gsa-logo">
  <xsl:param name="username"/>
  <xsl:param name="time"/>
<script type="text/javascript">
	$(function(){
		$("#div_navigation .ul_first .a_first").bind("click",function(){
			
			$(this).toggleClass("a_open");
			$(this).siblings("ul").toggle();
			
		});

		var height=document.documentElement.clientHeight;
		var width=window.screen.width-4;
		$("body").css("width",width);
		$("#div_menu").css("height",height-91);	
		$("#ul_navigation").css("height",height-114);
		$("#div_navigation").css("height",height-96);
		$("#div_main").css("height",height-130);

		

		$("#div_menu").css("min-height",500);	
		$("#ul_navigation").css("min-height",477);
		$("#div_navigation").css("min-height",495);
		$("#div_main").css("min-height",462);

		$("#div_main").css("width",width-165);

		setDate();
		setInterval(setDate,1000);

	});
	function setDate(){
	//alert("");
		var time=$("#lblTime").html();	
		var strArray=time.split(' ');
		var tstr=strArray[1].split(':');
		
		 var data = new Date();
            	//data.setYear(2013);
            	//data.setMonth(Number("2") - 1);
            	//data.setDate(5);
            	data.setHours(Number(tstr[0]));
            	data.setMinutes(Number(tstr[1]));
		data.setSeconds(Number(tstr[2])+1);

            	//var year = data.getYear();
            	//var month = (data.getMonth() + 1).toString().length == 1 ? "0" + (data.getMonth() + 1) : (data.getMonth() + 1);
            	//var date = data.getDate().toString().length == 1 ? "0" + data.getDate() : data.getDate();
            	var hours = data.getHours().toString().length == 1 ? "0" + data.getHours() : data.getHours();
            	var minute = data.getMinutes().toString().length == 1 ? "0" + data.getMinutes() : data.getMinutes();
		var second = data.getSeconds().toString().length == 1 ? "0" + data.getSeconds() : data.getSeconds();

            //alert(year + "-" + month + "-" + date + " " + hours + ":" + minute);

		

		$("#lblTime").html(strArray[0]+" "+ hours + ":" + minute+":"+second+" "+strArray[2]);
		
	}
	//$(function(){
	//	$("#btnUpdateUser").bind("click",function(){
	//		
	//	}
	//}

	function userDetailUpdate(e){
		$("#txtUserNameTop").val($(e).html());
		$("#txtUserNameTop_e").val($(e).html());
		$("#div_userinfo_top").show();
	}
	
	function CancelUpdate_Top(){
		$("#div_userinfo_top").hide();
	}
</script>
<div class="head_bg">
  <div class="header">
    <div class="top">
      <div class="div_user_image"></div>
      <div class="div_user">
        <xsl:choose>
          <xsl:when test="$username = ''">
          </xsl:when>
          <xsl:when test="string-length ($username) &gt; 45">      
            欢迎您： <span style="display: inline;" href="#"><xsl:value-of select="/envelope/role"/></span>
            <b><a onclick="userDetailUpdate(this)"><xsl:value-of select="substring ($username, 1, 45)"/>...</a></b> 		           
        </xsl:when>
          <xsl:otherwise>
            欢迎您： <span><xsl:value-of select="/envelope/role"/></span>
            <b><a onclick="userDetailUpdate(this)" href="#"><xsl:value-of select="$username"/></a></b> 	
          </xsl:otherwise>
        </xsl:choose>
      </div>
      <!--div id="login_user_modify" style="display:none">
	<td>修改用户</td>
      </div-->
      <div class="div_exist_image"></div>
      <a href="/logout?token={/envelope/token}" title="Logout" style="margin-left:3px;" onclick="javascript:return confirm('确定要退出吗？')">安全退出</a>
      <div class="div_close_image"></div>

      <a href="/omp?cmd=close&amp;token={/envelope/token}" style="margin-left:3px" onclick="javascript:return confirm('确定要关机吗？')">关机</a>
      <div class="div_restart_image"></div>
      <a href="/omp?cmd=reboot&amp;token={/envelope/token}" style="margin-left:3px" onclick="javascript:return confirm('确定要重启吗？')">重启</a>
    </div>
    <div class="div_header_bottom">
      <div class="div_time_image"></div>
      <span>当前时间：<label id="lblTime"><xsl:value-of select="time"/></label></span>
    </div>
  </div>
</div>

<div id="div_userinfo_top" style="display:none" >
 <div class="div_info_top">
 <form action="/omp" method="post">
  <input type="hidden" name="token" value="{/envelope/token}"/>
  <input type="hidden" name="cmd" value="user_modify_myself"/>
  <table cellspacing="0" cellpadding="0" style="width:100%;">
		<tr>
			  <td class="title"  colspan="2">修改用户</td>			
			</tr>
    <tr>
     <td class="tag" style="width:100px;height:40px;"><pre>用 户 名：</pre></td>
     <td><input maxlength="50" type="text" disabled="disabled" autocomplete="off" id="txtUserNameTop" name="username_hidden" value="{./tip_user}" readonly="1"/></td>
     <td><input type="hidden" name="username" id="txtUserNameTop_e" value="{./tips_user}"/></td>
    </tr>
    <tr>
     <td class="tag" style="width:100px;height:40px;"><pre>密    码：</pre></td>
     <td><input type="password" maxlength="50" autocomplete="off" name="password" value="{./tip_password}"/></td>
    </tr>
    <tr>
     <td class="tag" style="width:100px;height:40px;"><pre>确认密码：</pre></td>
     <td><input type="password" maxlength="50" autocomplete="off" name="checkpassword" value=""/></td>
    </tr>
   
    <tr>
     
     <td colspan="2" style="text-align:center;height:40px">  <input type="submit" name="submit" class="btn" value="保存"/>
     <input type="button" value="取消" class="btn" onclick="CancelUpdate_Top()"/></td>
    </tr>
 	
   </table>
 </form>
</div>
</div>
</xsl:template>

<xsl:template name="html-footer">

</xsl:template>

<xsl:template name="html-gsa-navigation">
  <div class="div_menu" id="div_menu">
     <ul class="ul_navigation" id="ul_navigation">
        <li class="li_warning"><a href="/omp?cmd=get_Alarm_info&amp;token={/envelope/token}">报警查询</a></li>
        <li class="li_audit"><a href="/omp?cmd=get_netlog_info&amp;token={/envelope/token}">审计查询</a></li>
        <!--li class="li_stragety"><a  href="/omp?cmd=stragetymanage&amp;token={/envelope/token}">策略管理</a></li-->
        <li class="li_sysmanage"><a  href="/omp?cmd=get_users&amp;overrides=1&amp;token={/envelope/token}">系统管理</a></li>
        <li class="li_sysmonitor"><a  href="/omp?cmd=sitestatus&amp;token={/envelope/token}">系统监控</a></li>
     </ul>
  </div>
<div class="menu" style="display:none">
<center>
   <ul class="grade_1">
    <li><a href="/omp?cmd=get_users&amp;overrides=1&amp;token={/envelope/token}">用户管理</a>
	<ul>
		<li id="1"><a href="/omp?cmd=get_users&amp;overrides=1&amp;token={/envelope/token}">用户管理</a></li>
	</ul>
    </li>
    <li><a href="/omp?cmd=get_config&amp;type=assets&amp;overrides=1&amp;levels=hm&amp;token={/envelope/token}">配置管理</a>
	<ul>
		<li  id="2"><a href="/omp?cmd=get_config&amp;type=assets&amp;overrides=1&amp;levels=hm&amp;token={/envelope/token}">配置管理</a></li>
	</ul>
    </li>
    <li><a href="/omp?cmd=get_data&amp;token={/envelope/token}&amp;pagenum=1">数据管理</a>
	<ul>
		<li  id="3"><a href="/omp?cmd=get_data&amp;token={/envelope/token}&amp;pagenum=1">数据管理</a></li>
	</ul>
    </li>
    <li ><a href="/omp?cmd=get_audit&amp;token={/envelope/token}">审计查询</a>
	<ul>
		<li id="4"><a href="/omp?cmd=get_audit&amp;token={/envelope/token}">审计查询</a></li>
	</ul>
    </li>
    <li ><a href="/omp?cmd=get_Alarm_info&amp;token={/envelope/token}">检索信息</a>
	<ul>
		<li id="5"><a href="/omp?cmd=get_Alarm_info&amp;token={/envelope/token}">攻击窃密报警</a></li>
 		<li id="6"><a href="/omp?cmd=get_event_info&amp;token={/envelope/token}">涉密标志事件</a></li>
      		<li id="7"><a href="/omp?cmd=get_netlog_info&amp;token={/envelope/token}">上网行为审计</a></li>
	</ul>
    </li>

    <li><a href="/omp?cmd=get_product&amp;token={/envelope/token}">产品维护</a>
	<ul>
		<li  id="8"><a href="/omp?cmd=get_product&amp;token={/envelope/token}">设备状态</a></li>
		<li  id="9"><a href="/omp?cmd=get_sysSetting&amp;token={/envelope/token}">系统设置</a></li>
	</ul>
    </li>
    <li id="7" class="current" ><a href="/omp?cmd=get_information_attack&amp;token={/envelope/token}">统计信息</a>
	<ul><!--add by mjn 2013-08-29-->
		<li  id="10" class="g2_current" ><a href="/omp?cmd=get_information_attack&amp;token={/envelope/token}">攻击窃密</a></li>
		<li  id="11"><a  href="/omp?cmd=get_information_operation&amp;token={/envelope/token}">操作审计</a></li>
		<li  id="12"><a   href="/omp?cmd=get_information_secret&amp;token={/envelope/token}">涉密标识</a></li>
		<li  id="13"><a   href="/omp?cmd=get_information_action&amp;token={/envelope/token}">行为审计</a></li>
	</ul>
     </li>

   </ul>
</center>
  </div>
</xsl:template>

<xsl:template name="user-navigation">

 <div class="div_menu" id="div_menu">
     <ul class="ul_navigation" id="ul_navigation">
        <li class="li_warning"><a href="/omp?cmd=get_Alarm_info&amp;token={/envelope/token}">报警查询</a></li>
        <li class="li_audit"><a href="/omp?cmd=get_netlog_info&amp;token={/envelope/token}">审计查询</a></li>
     </ul>
  </div>
  <div class="menu" style="display:none">
  <center>
   <ul class="grade_1">
       <li ><a href="/omp?cmd=get_Alarm_info&amp;token={/envelope/token}">检索信息</a>
	<ul>
		<li id="5"><a href="/omp?cmd=get_Alarm_info&amp;token={/envelope/token}">攻击窃密报警</a></li>
 		<li id="6"><a href="/omp?cmd=get_event_info&amp;token={/envelope/token}">涉密标志事件</a></li>
      		<li id="7"><a href="/omp?cmd=get_netlog_info&amp;token={/envelope/token}">上网行为审计</a></li>
	</ul>
    </li>
   </ul>
  </center>
  </div>

</xsl:template>

<!-- OMP -->

<xsl:include href="omp.xsl"/>

<!-- Login page  登录页-->

<xsl:template match="login_page">
<div class="loginBody">
<div class="div_top"></div>
<div class="outer">
<div id="login">
	
          
          <form action="/omp" method="post" enctype="multipart/formdata">
            <input type="hidden" name="cmd" value="login" />
            <xsl:choose>
              <xsl:when test="string-length(url) = 0">
                <!--input type="hidden" name="text" value="/omp?cmd=get_information_attack&amp;overrides=1" /-->
                <input type="hidden" name="text" value="/omp?cmd=get_Alarm_info&amp;overrides=1" />
              </xsl:when>
              <xsl:otherwise>
                <input type="hidden" name="text" value="{url}" />
              </xsl:otherwise>
            </xsl:choose>
            <div class="loginItem wrongmessage"><span></span><xsl:value-of select="message"/></div> <!--登录失败提示-->
                <div class="loginItem"><span>用户名<label></label></span><input id="uName" type="text" autocomplete="off" maxlength="50" name="login" value="tips" /></div>
                       
                <div class="loginItem"><span>密<label></label>码<label></label></span><input id="uPwd" type="password" autocomplete="off" maxlength="50" name="password" value="tips123456" /></div>
           <div class="loginItem"><span></span><a href="#">点击下载IE8浏览器</a></div>
            <div class="loginItem"><span></span><input id="btnLogin" type="submit" value=" " /></div>
	    
          </form>
  </div>

</div>
   <div class="div_loginbottom">版权所有©2013<span style="width:12px;line-height:12px;display:inline-block;"></span>北京鼎普科技股份有限公司</div>
</div>
</xsl:template>

<!-- ROOT, ENVELOPE -->

<xsl:template match="caller">
</xsl:template>

<xsl:template match="token">
</xsl:template>

<xsl:template match="login">
</xsl:template>

<xsl:template match="time">
</xsl:template>

<xsl:template match="timezone">
</xsl:template>

<xsl:template match="role">
</xsl:template>

<xsl:template match="envelope">
  <div class="envelope">
    <xsl:call-template name="html-gsa-logo">
      <xsl:with-param name="username">
        <xsl:value-of select="login/text()"/>
      </xsl:with-param>
      <xsl:with-param name="time">
        <xsl:value-of select="time"/>
      </xsl:with-param>
    </xsl:call-template>
<div class="wrap">
    <xsl:choose>
     <xsl:when test="@tipsuser">
       <xsl:call-template name="user-navigation"/>
       <xsl:apply-templates/>
     </xsl:when>
     <xsl:otherwise>
       <xsl:call-template name="html-gsa-navigation"/>
       <xsl:apply-templates><xsl:with-param name="tipsuser">admin</xsl:with-param></xsl:apply-templates>
     </xsl:otherwise>
    </xsl:choose>
    <xsl:call-template name="html-footer"/>
  </div>
</div>
</xsl:template>

<xsl:template match="/">
  <html xmlns="http://www.w3.org/1999/xhtml">
	<xsl:call-template name="html-head"/>
	<xsl:choose>
	<xsl:when test="login_page">
		<body>
   		  <xsl:apply-templates/> 
   		</body>
	</xsl:when>
	<xsl:otherwise>
		<body>
   		  <xsl:apply-templates/> 
   		</body>
	</xsl:otherwise>
	</xsl:choose>
		 
  </html>
</xsl:template>

</xsl:stylesheet>
