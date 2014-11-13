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
<div class="head_bg">
  <div class="header">
    <div class="top">
        <xsl:choose>
          <xsl:when test="$username = ''">
          </xsl:when>
          <xsl:when test="string-length ($username) &gt; 45">      
            欢迎您： <span style="display: inline;"><xsl:value-of select="/envelope/role"/></span>
            <b><a href="/omp?cmd=user_modify_page&amp;login={/envelope/login}&amp;token={/envelope/token}"><xsl:value-of select="substring ($username, 1, 45)"/>...</a></b> 		           
     </xsl:when>
          <xsl:otherwise>
            欢迎您： <span><xsl:value-of select="/envelope/role"/></span>
            <b><a href="/omp?cmd=user_modify_page&amp;login={/envelope/login}&amp;token={/envelope/token}"><xsl:value-of select="$username"/></a></b> 	
          </xsl:otherwise>
        </xsl:choose>
	<image src="img/style/exit.png" class="exitImg"/>
        <a href="/logout?token={/envelope/token}" title="Logout" style="margin-left:3px;">安全退出</a>
    </div>
  </div>
</div>
</xsl:template>

<xsl:template name="html-footer">

</xsl:template>

<xsl:template name="html-gsa-navigation">
  <div class="menu">
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
  <div class="menu">
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
  <div id="login">
          <div class="loginItem" style="color: red;height:16px;padding-left:400px;"><xsl:value-of select="message"/></div><!-- 登录失败提示-->
          <form action="/omp" method="post" enctype="multipart/formdata">
            <input type="hidden" name="cmd" value="login" />
            <xsl:choose>
              <xsl:when test="string-length(url) = 0">
                <input type="hidden" name="text" value="/omp?cmd=get_information_attack&amp;overrides=1" />
              </xsl:when>
              <xsl:otherwise>
                <input type="hidden" name="text" value="{url}" />
              </xsl:otherwise>
            </xsl:choose>
            
                <div class="loginItem"><input id="uName" type="text" autocomplete="off" name="login" value="admin" /></div>
                       
                <div class="loginItem"><input id="uPwd" type="password" autocomplete="off" name="password" value="admin123456" /></div>
          
            <div class="loginItem"><input id="btnLogin" type="submit" value=" " /></div>

          </form>
  </div>
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
    <body style="background-color:#093478">
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
