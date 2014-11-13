<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:str="http://exslt.org/strings"
    xmlns:func = "http://exslt.org/functions"
    xmlns:gsa="http://openvas.org"
    xmlns:vuln="http://scap.nist.gov/schema/vulnerability/0.4"
    xmlns:cpe-lang="http://cpe.mitre.org/language/2.0"
    xmlns:scap-core="http://scap.nist.gov/schema/scap-core/0.1"
    xmlns:cve="http://scap.nist.gov/schema/feed/vulnerability/2.0"
    xmlns:cvss="http://scap.nist.gov/schema/cvss-v2/0.2"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:patch="http://scap.nist.gov/schema/patch/0.1"
    xmlns:meta="http://scap.nist.gov/schema/cpe-dictionary-metadata/0.2"
    xmlns:ns6="http://scap.nist.gov/schema/scap-core/0.1"
    xmlns:config="http://scap.nist.gov/schema/configuration/0.1"
    xmlns:cpe="http://cpe.mitre.org/dictionary/2.0"
    xsi:schemaLocation="http://scap.nist.gov/schema/configuration/0.1 http://nvd.nist.gov/schema/configuration_0.1.xsd http://scap.nist.gov/schema/scap-core/0.3 http://nvd.nist.gov/schema/scap-core_0.3.xsd http://cpe.mitre.org/dictionary/2.0 http://cpe.mitre.org/files/cpe-dictionary_2.2.xsd http://scap.nist.gov/schema/scap-core/0.1 http://nvd.nist.gov/schema/scap-core_0.1.xsd http://scap.nist.gov/schema/cpe-dictionary-metadata/0.2 http://nvd.nist.gov/schema/cpe-dictionary-metadata_0.2.xsd"
    xmlns:date="http://exslt.org/dates-and-times"
    extension-element-prefixes="str func date">
    <xsl:output
      method="html"
      doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"
      doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
      encoding="UTF-8"/>

<!-- XPATH FUNCTIONS -->

<func:function name="gsa:long-time">
  <xsl:param name="time"></xsl:param>
  <func:result>
    <xsl:if test="string-length ($time) &gt; 0">
      <xsl:value-of select="concat (date:day-abbreviation ($time), ' ', date:month-abbreviation ($time), ' ', date:day-in-month ($time), ' ', format-number(date:hour-in-day($time), '00'), ':', format-number(date:minute-in-hour($time), '00'), ':', format-number(date:second-in-minute($time), '00'), ' ', date:year($time))"/>
    </xsl:if>
  </func:result>
</func:function>


<!--用户管理-->
<xsl:template name="html-user-table">
<div class="workSpace">
<div class="location">您的位置：用户管理</div>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>用户信息</div>
   <div class="content">
  <xsl:variable name="apply-overrides" select="apply_overrides"/>
  <form action="/omp?cmd=tips_create_user" method="post">
   <input type="hidden" name="token" value="{/envelope/token}"/>
   <input type="hidden" name="cmd" value="tips_create_user"/>
   <input type="hidden" name="text" value="/omp?cmd=get_users&amp;overrides=1" />
   <table  cellspacing="0">
    <tr class="searchTr">
     <td class="tag">用户名称：</td>
     <td ><input type="text" autocomplete="off" maxlength="50" name="username" value=""/></td>
     <td class="tag">用户角色：</td>
     <td >     
      <select name="newrole">
        <option value="0">普通用户</option>
        <option value="2">管理员</option>
     </select>
     </td>
    <td colspan="4"><input type="submit"  name="submit" value="搜索" style="margin-left:44px;"/></td>
    </tr>
    <tr  class="searchTr">
     <td class="tag" >用户密码：</td>
     <td><input  type="password" autocomplete="off" maxlength="50" name="password" value="" /></td>
     <td class="tag"> 确认密码：</td>
     <td><input type="password" autocomplete="off" maxlength="50" name="checkpassword" value="" /></td>

     <td ><input type="submit" name="submit" value="新建" style="margin-left:44px;"/></td>
    </tr>
  <tr class="msgTr"><td colspan="4" ><xsl:apply-templates select="msg"/></td></tr>
   </table>
  </form> 

 </div></div>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>用户列表</div>
   <div class="content" style="height:410px;">
  <table class="list" cellspacing="0">
   <thead >
    <th style="width:20%">用户名</th>
    <th style="width:20%">用户角色</th>
    <th style="width:20%">创建时间</th>
    <th style="width:10%">删除</th>
    <th>修改</th>
   </thead>
</table>
<div class="divTbOuter" style="height:345px;">
<table class="list">
<tbody>
   <xsl:apply-templates select="tips_user"/>
</tbody>  
</table>
</div>
 </div>
</div>
</div>
</xsl:template>

<!--用户列表-->
<xsl:template match="tips_user">
 <tr>
  <td style="width:20%"><xsl:value-of select="username"/></td>
  <td style="width:20%"><xsl:value-of select="userrole"/></td>
  <td style="width:20%"><xsl:value-of select="createtime"/></td> 
  <td style="width:10%"><a href="/omp?cmd=user_delete&amp;login={./username}&amp;token={/envelope/token}">删除</a></td>
  <td><a href="/omp?cmd=user_modify_page&amp;login={./username}&amp;token={/envelope/token}&amp;userrole={userrole}">修改</a></td>
 </tr>
</xsl:template>

<!--提示信息-->
<xsl:template match="msg">
 <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="msg_product">
 <xsl:value-of select="."/>
</xsl:template>

<!--修改用户-->
<xsl:template name="user_amend">
<div class="workSpace">
<div class="location">您的位置：修改用户</div>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>修改用户信息</div>
   <div class="content">
 <form action="/omp" method="post">
  <input type="hidden" name="token" value="{/envelope/token}"/>
  <input type="hidden" name="cmd" value="user_modify"/>
  <table cellspacing="0" cellpadding="0"   width="300px">
    <tr>
     <td class="tag" style="width:100px;">用户名：</td>
     <td colspan="2"><input style="width:100%;border:0" maxlength="50" type="text" autocomplete="off" name="username" value="{./tip_user}" readonly="readonly"/></td>
    </tr>
    <tr>
     <td class="tag" style="width:100px;">用户角色：</td>
     <td colspan="2"><input style="width:100%;border:0" maxlength="50" type="text" autocomplete="off" name="userrole" value="{./userrole}" readonly="readonly"/></td>
    </tr>
    <tr>
     <td class="tag">密码：</td>
     <td colspan="2"><input style="width:100%" maxlength="50" type="password" autocomplete="off" name="passwd" value="{./tip_password}"/></td>
    </tr>
    <tr>
     <td class="tag">确认密码：</td>
     <td colspan="2"><input style="width:100%" maxlength="50" type="password" autocomplete="off" name="checkpasswd" value=""/></td>
    </tr>
   
    <tr>
     <td></td>
     <td width="140"><input type="submit" name="submit" value="保存"/></td>
     <td><a class="commonBtn" href="/omp?cmd={/envelope/get_tasks_response/gotocmd}&amp;overrides=1&amp;token={/envelope/token}">取消</a></td>
    </tr>
 	<tr class="msgTr">
     <td colspan="2"  style="color:#F00"> <xsl:value-of select="./tip_errmsg"/></td>
    </tr>
   </table>
 </form>
</div>
</div></div>
</xsl:template>

<xsl:template match="all">
</xsl:template>

<!--配置管理-->
<xsl:template name="config">
<div class="workSpace">
<div class="location">您的位置：配置管理</div>
<form action="/omp?cmd=configadmin" method="post">
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>本地信息配置</div>
   <div class="content" style="height:200px;padding-top:20px;padding-bottom:20px;">
   <input type="hidden" name="token" value="{/envelope/token}"/>
   <input type="hidden" name="cmd" value="configadmin"/>
   <table cellspacing="0" cellpadding="0" >
    <tr class=" heighTr">
     <td class="tag"> 管理口IP地址： </td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="manage_IP" value="{./MANAGE_IP}"/><span class="notice"></span></td>
     <td class="tag"> 通信口IP地址： </td>
     <td><input type="text" autocomplete="off" maxlength="50" name="net_IP" value="{./NET_IP}"/><span class="notice"></span></td>
     <td class="tag">通信网关地址：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="gateway" value="{./GATEWAY}"/><span class="notice"></span></td>
  </tr>
  <tr class=" heighTr"> 
     <td class="tag">静态路由地址：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="static_route" value="{./STATIC_ROUTE}"/><span class="notice"></span></td>
     <td class="tag">管理机IP地址：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="supervisor" value="{./SUPERVISOR}"/><span class="notice"></span></td>
    <td class="tag">部属单位信息：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="unit_name" value="{./UNIT_NAME}"/><span class="notice"></span></td>
</tr>
   <tr class="heighTr">
     <td colspan="6" style="text-align:center;"><input type="submit" name="submit" value="提交"/></td>
   </tr>
	<tr class="msg">
	  <xsl:apply-templates select="config_submit_msg"/>
	</tr>
</table>

</div></div>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>通信信息配置</div>
   <div class="content" style="height:200px;padding-top:30px;padding-bottom:40px;">
<table>
    <tr class=" heighTr">
     <td class="tag">代理服务地址：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="serve_addr" value="{./SERVE_ADDR}"/><span class="notice"></span></td>

     <td class="tag">代理服务账号：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="serve_user" value="{./SERVE_USER}"/><span class="notice"></span></td>

     <td class="tag">代理服务口令：</td>
     <td><input  type="password" autocomplete="off" maxlength="50" name="serve_password" value="{./SERVE_PASSWORD}"/><span class="notice"></span></td>
</tr>
<tr class=" heighTr"> 
     <td class="tag">主中心IP地址：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="administration_IP" value="{./ADMINSTRATION_IP}"/><span class="notice"></span></td>
     <td class="tag">主中心port：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="manage_PORT" value="{./MANAGE_PORT}"/><span class="notice"></span></td>
     <td class="tag">副中心IP地址：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="administration_IP1" value="{./ADMINSTRATION_IP1}"/><span class="notice"></span></td>
      </tr>
    <tr class=" heighTr">
     <td class="tag">副中心port：</td>
     <td><input  type="text" autocomplete="off" maxlength="50" name="manage_PORT1" value="{./MANAGE_PORT1}"/><span class="notice"></span></td>
    </tr><tr>
     <td colspan="6" style="text-align:center;"><input type="submit" name="submit" value="保存"/></td>
    </tr>

    <tr class="msg">
      <xsl:apply-templates select="config_msg"/>
    </tr>   
   </table>
   </div>
</div>
</form>
</div>
</xsl:template>

<!--配置管理的提示-->
<xsl:template match="config_msg">
	<td colspan="4" style="text-align:center;"><font color="red"><xsl:value-of select="."/></font></td>
</xsl:template>

<xsl:template match="config_submit_msg">
	<td colspan="4" style="text-align:center;"><font color="green"><xsl:value-of select="."/></font></td>
</xsl:template>


<!--数据管理-->
<xsl:template match="data">
<div class="workSpace">
<div class="location">您的位置：数据管理</div>

<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>数据信息</div>
   <div class="content">
  <form action="/omp?cmd=data_display" method="get">
   <input type="hidden" name="token" value="{/envelope/token}"/>
   <input type="hidden" name="pagenum" value="1"/>
   <input type="hidden" name="cmd" value="data_display"/>
   <input type="hidden" name="refresh_interval" value="{/envelope/autorefresh/@interval}"/>
<p  class="searchTr">
	<span class="tag">备份名称：</span><input class="text" type="text" maxlength="50" name="bak_name" value="{./bakname}"/><span class="tag"> 备份时间：</span><input type="text" maxlength="50" name="bak_time" value="{./baktime}" class="Wdate colMid" onfocus="WdatePicker()"/>
<input type="submit" name="" value="搜索" style="margin-left:44px;"/>
</p>
</form>
<div class="msgTr">
  <tr><td colspan="4" ><font color="red"><xsl:value-of select="msg_data"/></font></td></tr>
</div>
</div></div>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>数据列表</div>
   <div class="content" style="height:435px;">
  <table class="list" cellspacing="0">
   <thead>
    <th  style="width:30%">备份名</th>
    <th  style="width:30%">备份时间</th>
    <th  style="width:20%">恢复</th>
    <th>删除</th>
   </thead>
</table>
<div class="divTbOuter" style="height:370px;">
<table class="list">
  <tbody>
  <xsl:apply-templates select="list"/>
  </tbody>
  </table>
</div>
  <xsl:apply-templates select="page"/>
</div>
</div></div>
</xsl:template>

<!--数据列表-->
<xsl:template match="list">
 <tr>
  <td style="width:30%"><xsl:value-of select="name"/></td>
  <td style="width:30%"><xsl:value-of select="Start_time"/></td>
  <td style="width:20%"><a href="/omp?cmd=data_rebuild&amp;BackID={./backid}&amp;tips_table={./tips_table}&amp;token={/envelope/token}&amp;bak_time={//baktime}&amp;bak_name={//bakname}&amp;pagenum={//nowpage}">恢复</a></td>
  <td><a href="/omp?cmd=data_delete&amp;BackID={./backid}&amp;tips_table={./tips_table}&amp;token={/envelope/token}&amp;bak_time={//baktime}&amp;bak_name={//bakname}&amp;pagenum={//nowpage}">删除</a></td>
 </tr>
</xsl:template>

<!-- 检索信息-->

<!--攻击窃密-->
<xsl:template match="attack">
 <xsl:param name="tipsuser">user</xsl:param>
<div class="workSpace">
<div class="location">您的位置：检索信息>>攻击窃密</div>
 
<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">
  <form action="/omp" method="get">
   <input type="hidden" name="token" value="{/envelope/token}"/>
   <input type="hidden" name="pagenum" value="1"/>
   <input type="hidden" name="cmd" value="tips_alarm_display"/>
   <input type="hidden" name="refresh_interval" value="{/envelope/autorefresh/@interval}"/>
   <table  cellspacing="0" cellpadding="0"  >
    <tr  class="searchTr">
     <td class="tag"> 木马编号：</td>
     <td><input class="shortText" type="text" autocomplete="off" maxlength="50" name="tips_Trogan_id" value="{./OTrogan_id}"/></td>
     <td class="tag">警报级别：</td>
     <td>
      <select name="tips_Risk" class="shortText">
       <xsl:choose>
	<xsl:when test="@Risk=4">
         <option value="" >所有</option>
         <option value="1" >一般级</option>
         <option value="2" >关注级</option>
         <option value="3" >严重级</option>
         <option value="4" selected="1">紧急级</option>
	</xsl:when>
	<xsl:when test="@Risk=2">
         <option value="" >所有</option>
         <option value="1" >一般级</option>
         <option value="2" selected="1">关注级</option>
         <option value="3" >严重级</option>
         <option value="4" >紧急级</option>
	</xsl:when>
	<xsl:when test="@Risk=3">
         <option value="" >所有</option>
         <option value="1" >一般级</option>
         <option value="2" >关注级</option>
         <option value="3" selected="1">严重级</option>
         <option value="4" >紧急级</option>
	</xsl:when>
	<xsl:when test="@Risk=1">
         <option value="" >所有</option>
         <option value="1" selected="1">一般级</option>
         <option value="2" >关注级</option>
         <option value="3" >严重级</option>
         <option value="4" >紧急级</option>
	</xsl:when>
	<xsl:otherwise>
         <option value="" selected="1">所有</option>
         <option value="1" >一般级</option>
         <option value="2" >关注级</option>
         <option value="3" >严重级</option>
         <option value="4" >紧急级</option>
	</xsl:otherwise>
       </xsl:choose>
      </select>
     </td>  
     <td class="tag">报警类型：</td>
     <td>
       <select name="tips_Type" class="shortText">
       <xsl:choose>
        <xsl:when test="@Type=90">
       	 <option value="">所有</option>
       	 <option value="10">特种木马行为</option>
         <option value="20" >普通木马行为</option>
         <option value="30" >疑似木马行为</option>
         <option value="80" >非木马类攻击窃密行为</option>
         <option value="90" selected="1">其他可疑行为</option>
	</xsl:when>
        <xsl:when test="@Type=20">
       	 <option value="">所有</option>
       	 <option value="10" >特种木马行为</option>
         <option value="20" selected="1">普通木马行为</option>
         <option value="30" >疑似木马行为</option>
         <option value="80" >非木马类攻击窃密行为</option>
         <option value="90" >其他可疑行为</option>
	</xsl:when>
        <xsl:when test="@Type=30">
       	 <option value="">所有</option>
       	 <option value="10" >特种木马行为</option>
         <option value="20" >普通木马行为</option>
         <option value="30" selected="1">疑似木马行为</option>
         <option value="80" >非木马类攻击窃密行为</option>
         <option value="90" >其他可疑行为</option>
	</xsl:when>
        <xsl:when test="@Type=80">
       	 <option value="">所有</option>
       	 <option value="10" >特种木马行为</option>
         <option value="20" >普通木马行为</option>
         <option value="30" >疑似木马行为</option>
         <option value="80" selected="1">非木马类攻击窃密行为</option>
         <option value="90" >其他可疑行为</option>
	</xsl:when>
        <xsl:when test="@Type=10">
       	 <option value="">所有</option>
       	 <option value="10" selected="1">特种木马行为</option>
         <option value="20" >普通木马行为</option>
         <option value="30" >疑似木马行为</option>
         <option value="80" >非木马类攻击窃密行为</option>
         <option value="90" >其他可疑行为</option>
	</xsl:when>
        <xsl:otherwise>
       	 <option value="" selected="1">所有</option>
       	 <option value="10" >特种木马行为</option>
         <option value="20" >普通木马行为</option>
         <option value="30" >疑似木马行为</option>
         <option value="80" >非木马类攻击窃密行为</option>
         <option value="90" >其他可疑行为</option>
	</xsl:otherwise>
       </xsl:choose>
      </select>
     </td>
     <td class="tag">警报时间：</td>
     <td><input type="text" autocomplete="off" maxlength="50" name="tips_Alarm_time" value="{./OAlarm_time}" class="Wdate colMid" onfocus="WdatePicker()"/>--<input type="text" autocomplete="off" maxlength="50" name="tips_endtime" value="{./OAlarm_end_time}" class="Wdate colMid" onfocus="WdatePicker()"/></td>
     </tr>
     <tr  class="searchTr">
     <td class="tag">内部地址：</td>
     <td><input class="shortText" type="text" autocomplete="off" maxlength="50" name="tips_Dip" value="{./ODip}"/></td>
 
     <td class="tag">内部MAC：</td>
     <td><input class="shortText" type="text" autocomplete="off" maxlength="50" name="tips_Dmac" value="{./ODmac}"/></td>
  <td class="tag">外部地址：</td>
     <td><input class="shortText" type="text" autocomplete="off" maxlength="50" name="tips_Sip" value="{./OSip}"/></td>    
     <td class="tag">外部位置：</td>
     <td>
      <select name="tips_Scc_flag" class="shortText">
	<xsl:call-template name="scc_addr_flag"/>
      </select>

     <input style="margin-left:125px;" type="submit" name="submit" value="搜索"/>
     </td>
    </tr>
    <tr class="msgTr"><td colspan="8"><xsl:apply-templates select="//msg"/></td></tr>
   </table>

  </form>
 </div>
</div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询列表</div>
   <div class="content" style="height:410px;">
	<div class="listOper">
     <form method="get" action="/omp" style="display:inline-block;">
     	    <input type="hidden" name="token" value="{/envelope/token}"/>
     			<input type="hidden" name="cmd" value="tips_alarm_display"/>
    			<input type="hidden" name="isfind" value="{./isfind}"/>
    			<input type="hidden" name="pagenum" value="{//nowpage}"/>
    			<input type="hidden" name="tips_Trogan_id" value="{./OTrogan_id}"/>
    			<input type="hidden" name="tips_Sip" value="{./OSip}"/>
   	 			<input type="hidden" name="tips_Dip" value="{./ODip}"/>
    			<input type="hidden" name="tips_Dmac" value="{./ODmac}"/>
    			<input type="hidden" name="tips_Alarm_time" value="{./OAlarm_time}"/>
    			<input type="hidden" name="tips_endtime" value="{./OAlarm_end_time}"/>
    			<input type="hidden" name="tips_Type" value="{./@Type}"/>
    			<input type="hidden" name="tips_Risk" value="{./@Risk}"/>
    			<input type="hidden" name="tips_Scc_flag" value="{./@Scc_flag}"/>
          <select class="titleSelect" name="refresh_interval" size="1">
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='0'">
                <option value="0" selected="1">&#8730;不自动刷新</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="0">不自动刷新</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='10'">
                <option value="10" selected="1">&#8730;每10秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="10">每10秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='30'">
                <option value="30" selected="1">&#8730;每30秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="30">每30秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='60'">
                <option value="60" selected="1">&#8730;每60秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="60">每60秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
          </select>
          <input style="background:url(img/style/fresh.png) 0 0 no-repeat;height:26px;padding-left: 20px;" type="submit" value="刷新" name="Update" />
     </form>
  
<xsl:if test="$tipsuser!='user'">  

   <form action="/omp" method="get"  style="display:inline-block;">
    <select class="titleSelect" name="tips_operator">
     <option value="tips_output_alarm">导出当页</option>
     <option value="tips_copy_alarm">备份当页</option>
<xsl:comment><option value="tips_delete_alarm">删除当页</option></xsl:comment>
     <option value="tips_output_alarmf">导出全部</option>
     <option value="tips_copy_alarmf">备份全部</option>
<xsl:comment><option value="tips_delete_alarmf">删除全部</option></xsl:comment>
    </select>
    <input type="hidden" name="token" value="{/envelope/token}"/>
    <input type="hidden" name="cmd" value="tips_alarm_opt"/>
    <input type="hidden" name="refresh_interval" value="{/envelope/autorefresh/@interval}"/>
    <input type="hidden" name="isfind" value="{./isfind}"/>
    <input type="hidden" name="pagenum" value="{//nowpage}"/>
    <input type="hidden" name="tips_Trogan_id" value="{./OTrogan_id}"/>
    <input type="hidden" name="tips_Sip" value="{./OSip}"/>
    <input type="hidden" name="tips_Dip" value="{./ODip}"/>
    <input type="hidden" name="tips_Dmac" value="{./ODmac}"/>
    <input type="hidden" name="tips_Alarm_time" value="{./OAlarm_time}"/>
    <input type="hidden" name="tips_endtime" value="{./OAlarm_end_time}"/>
    <input type="hidden" name="tips_Type" value="{./@Type}"/>
    <input type="hidden" name="tips_Risk" value="{./@Risk}"/>
    <input type="hidden" name="tips_Scc_flag" value="{./@Scc_flag}"/>
    <input type="hidden" name="tips_sort" value="{./tips_sort}"/>
    <input type="submit" name="output" value="提交" />
   </form>
 
</xsl:if>
 
 </div>  

  <table class="list" cellspacing="0" cellpadding="0" >
   <thead class="listhead"><th style="width:10px;"></th>
    <th style="width:20%;" >报警木马编号</th>
    <th style="width:20%;" >警报级别信息</th>
    <th style="width:20%;" >内部主机地址</th>
    <th style="width:20%;" >内部主机MAC</th>
	<th ><a style="color:#333" href="/omp?cmd=tips_alarm_display&amp;token={/envelope/token}&amp;refresh_interval={/envelope/autorefresh/@interval}&amp;isfind={./isfind}&amp;pagenum={//nowpage}&amp;tips_Trogan_id={./OTrogan_id}&amp;tips_Sip={./OSip}&amp;tips_Dip={./ODip}&amp;tips_Dmac={./ODmac}&amp;tips_Alarm_time={./OAlarm_time}&amp;tips_endtime={./OAlarm_end_time}&amp;tips_Type={./@Type}&amp;tips_Risk={./@Risk}&amp;tips_Scc_flag={./@Scc_flag}&amp;tips_sort={./tips_sort}&amp;tips_sortis=1"><font color="blue">警报产生时间</font></a></th>  
   </thead>
 </table>
<div class="divTbOuter" style="height:345px;">
<table class="openCloseList">
<tbody>
   <tr>
    <xsl:apply-templates select="list" mode="attach"/>
   </tr>
</tbody>
  </table>
</div>
  <xsl:apply-templates select="page"/>
</div>
</div></div>
</xsl:template>

<!--攻击窃密列表-->
<xsl:template match="list" mode="attach">
 <tr>
  <td style="width:10px;" ><image src="img/style/close.png" class="openCloseImg"/></td>
  <td style="width:20%;" ><xsl:value-of select="Trogan_id"/></td><!--mmbh=木马编号首拼音-->
  <td style="width:20%;" ><xsl:value-of select="Risk"/></td>
  <td style="width:20%;" ><xsl:value-of select="Dip"/></td>
  <td style="width:20%;" ><xsl:value-of select="Dmac"/></td>
  <td><xsl:value-of select="Alarm_time"/></td>
 </tr>
<tr style="display:none"><td></td><td colspan="6">
描述： <xsl:value-of select="Alarm_desc"/><br/>
外部主机位置：<xsl:value-of select="Scc_flag"/><br/>
外部主机地址： <xsl:value-of select="Sip"/><br/>
报警类型信息：<xsl:value-of select="Type"/>
</td></tr>
</xsl:template>


<!--地址标识-->
<xsl:template name="scc_addr_flag">
<xsl:choose>
	 <xsl:when test="@Scc_flag=6">
          <option value="" >所有</option>
          <option value="0" >国外</option>
          <option value="1" >大陆</option>
          <option value="2" >台湾</option>
          <option value="3" >香港</option>
          <option value="4" >澳门</option>
          <option value="5" >其他</option>
          <option value="6" selected="1">局域网</option>
	 </xsl:when>
	 <xsl:when test="@Scc_flag=1">
          <option value="" >所有</option>
          <option value="0" >国外</option>
          <option value="1" selected="1">大陆</option>
          <option value="2" >台湾</option>
          <option value="3" >香港</option>
          <option value="4" >澳门</option>
          <option value="5" >其他</option>
          <option value="6" >局域网</option>
	 </xsl:when>
	 <xsl:when test="@Scc_flag=2">
          <option value="" >所有</option>
          <option value="0" >国外</option>
          <option value="1" >大陆</option>
          <option value="2" selected="1">台湾</option>
          <option value="3" >香港</option>
          <option value="4" >澳门</option>
          <option value="5" >其他</option>
          <option value="6" >局域网</option>
	 </xsl:when>
	 <xsl:when test="@Scc_flag=3">
          <option value="" >所有</option>
          <option value="0" >国外</option>
          <option value="1" >大陆</option>
          <option value="2" >台湾</option>
          <option value="3" selected="1">香港</option>
          <option value="4" >澳门</option>
          <option value="5" >其他</option>
          <option value="6" >局域网</option>
	 </xsl:when>
	 <xsl:when test="@Scc_flag=4">
          <option value="" >所有</option>
          <option value="0" >国外</option>
          <option value="1" >大陆</option>
          <option value="2" >台湾</option>
          <option value="3" >香港</option>
          <option value="4" selected="1">澳门</option>
          <option value="5" >其他</option>
          <option value="6" >局域网</option>
	 </xsl:when>
	 <xsl:when test="@Scc_flag=5">
          <option value="" >所有</option>
          <option value="0" >国外</option>
          <option value="1" >大陆</option>
          <option value="2" >台湾</option>
          <option value="3" >香港</option>
          <option value="4" >澳门</option>
          <option value="5" selected="1">其他</option>
          <option value="6" >局域网</option>
	 </xsl:when>
	 <xsl:when test="@Scc_flag=0">
          <option value="" >所有</option>
          <option value="0" selected="1">国外</option>
          <option value="1" >大陆</option>
          <option value="2" >台湾</option>
          <option value="3" >香港</option>
          <option value="4" >澳门</option>
          <option value="5" >其他</option>
          <option value="6" >局域网</option>
	 </xsl:when>
	 <xsl:otherwise>
          <option value="" selected="1">所有</option>
          <option value="0" >国外</option>
          <option value="1" >大陆</option>
          <option value="2" >台湾</option>
          <option value="3" >香港</option>
          <option value="4" >澳门</option>
          <option value="5" >其他</option>
          <option value="6" >局域网</option>
	 </xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!--涉密标志-->
<xsl:template match="secret">
 <xsl:param name="tipsuser">user</xsl:param>
<div class="workSpace">
<div class="location">您的位置：检索信息>>涉密标志</div>  
<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">
   <form action="/omp" method="get">
   <input type="hidden" name="token" value="{/envelope/token}"/>
   <input type="hidden" name="cmd" value="tips_event_display"/>
   <input type="hidden" name="pagenum" value="1"/>
   <input type="hidden" name="refresh_interval" value="{/envelope/autorefresh/@interval}"/>
   <table   cellspacing="0" cellpadding="0"  >

    <tr class="searchTr">
     <td class="tag">事件类型：</td>
     <td>
       <select name="tips_Event_class" class="shortText">
       <xsl:choose>
	<xsl:when test="@type = 1011">
       	  <option value="">所有</option>
       	  <option value="1011" selected="1">发现涉嫌违规传输泄密信息</option>
          <option value="1002" >发现涉嫌传输含有恶意程序文件</option>
	</xsl:when>
	<xsl:when test="@type = 1002">
       	  <option value="">所有</option>
       	  <option value="1011">发现涉嫌违规传输泄密信息</option>
          <option value="1002" selected="1">发现涉嫌传输含有恶意程序文件</option>
	</xsl:when>
	<xsl:otherwise>
       	  <option value="" selected="1">所有</option>
       	  <option value="1011" >发现涉嫌违规传输泄密信息</option>
          <option value="1002">发现涉嫌传输含有恶意程序文件</option>
	</xsl:otherwise>
      </xsl:choose>
      </select>
     </td>
     
     <td class="tag">外部主机IP：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Sip" value="{./OSip}"/></td>
   
     <td class="tag">端口号：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Sport" value="{./OSport}"/></td>
     <td class="tag">内部主机IP：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Dip" value="{./ODip}"/></td>   
    </tr>
    <tr class="searchTr">     
      
     <td class="tag">发送人ID：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Sender" value="{./OSender}"/></td>
     <td class="tag">接受人ID：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Receiver" value="{./OReceiver}"/></td>
     <td class="tag">敏感词：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Sen_msg" value="{./OSen_msg}"/></td>
     <td class="tag">产生时间：</td>
     <td><input type="text" autocomplete="on" maxlength="50" name="tips_Event_time" value="{./OEvent_time}" class="Wdate colMid" onfocus="WdatePicker()"/></td>
    </tr>
    <tr class="searchTr">
     <td colspan="8" align="center"><input type="submit" name="submit" value="搜索"/></td>
    </tr>

   </table>
  </form>  
 <div class="msgTr"><xsl:apply-templates select="//msg"/></div>
</div>
</div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询列表</div>
   <div class="content" style="height:380px;">
    <div class="listOper">
     <form method="get" action="/omp" style="display:inline-block">
    	    <input type="hidden" name="token" value="{/envelope/token}"/>
     			<input type="hidden" name="cmd" value="tips_event_display"/>
    			<input type="hidden" name="isfind" value="{./isfind}"/>
    			<input type="hidden" name="pagenum" value="{//nowpage}"/>
				<input type="hidden" name="tips_Event_class" value="{./@type}"/>
    			<input type="hidden" name="tips_Event_time" value="{./OEvent_time}"/>
    			<input type="hidden" name="tips_Sip" value="{./OSip}"/>
    			<input type="hidden" name="tips_Sport" value="{./OSport}"/>
    			<input type="hidden" name="tips_Dip" value="{./ODip}"/>
    			<input type="hidden" name="tips_Sender" value="{./OSender}"/>
    			<input type="hidden" name="tips_Receiver" value="{./@Receiver}"/>
    			<input type="hidden" name="tips_Sen_msg" value="{./OSen_msg}"/>
				<input type="hidden" name="tips_sort" value="{./tips_sort}"/>
 
 
          <select class="titleSelect" name="refresh_interval" size="1">
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='0'">
                <option value="0" selected="1">&#8730;不自动刷新</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="0">不自动刷新</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='10'">
                <option value="10" selected="1">&#8730;每10秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="10">每10秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='30'">
                <option value="30" selected="1">&#8730;每30秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="30">每30秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='60'">
                <option value="60" selected="1">&#8730;每60秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="60">每60秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
          </select>
          <input style="background:url(img/style/fresh.png) 0 0 no-repeat;height:26px;padding-left: 20px;" type="submit" value="刷新" name="Update"/>
     </form>
 
<xsl:if test="$tipsuser!='user'">
   <form action="/omp" method="get" style="display:inline-block">
    <select name="tips_operator" class="titleSelect">
     <option value="tips_output_event">导出当页</option>
     <option value="tips_copy_event">备份当页</option>
<xsl:comment><option value="tips_delete_event">删除当页</option></xsl:comment>
     <option value="tips_output_eventf">导出全部</option>
     <option value="tips_copy_eventf">备份全部</option>
<xsl:comment><option value="tips_delete_eventf">删除全部</option></xsl:comment>
    </select>
    <input type="hidden" name="cmd" value="tips_event_opt"/>
    <input type="hidden" name="token" value="{/envelope/token}"/>
    <input type="hidden" name="isfind" value="{./isfind}"/>
    <input type="hidden" name="pagenum" value="{//nowpage}"/>
    <input type="hidden" name="refresh_interval" value="{/envelope/autorefresh/@interval}"/>
    <input type="hidden" name="tips_Event_class" value="{./@type}"/>
    <input type="hidden" name="tips_Event_time" value="{./OEvent_time}"/>
    <input type="hidden" name="tips_Sip" value="{./OSip}"/>
    <input type="hidden" name="tips_Sport" value="{./OSport}"/>
    <input type="hidden" name="tips_Dip" value="{./ODip}"/>
    <input type="hidden" name="tips_Sender" value="{./OSender}"/>
    <input type="hidden" name="tips_Receiver" value="{./@Receiver}"/>
    <input type="hidden" name="tips_Sen_msg" value="{./OSen_msg}"/>
    <input type="hidden" name="tips_sort" value="{./tips_sort}"/>
    <input type="submit" name="output" value="提交" />
   </form>

</xsl:if>
</div>

  <table class="openCloseList" cellspacing="0">
   <thead>
    <th style="width:10px;"></th>
    <th style="width:15%;">事件类型</th>
    <th style="width:15%;">事件子类</th>   
    <th style="width:15%;">内部主机IP</th> 
    <th style="width:20%;">内部主机MAC</th>
    <th style="width:15%;">发送人ID</th>
	<th ><a style="color:#333" href="/omp?cmd=tips_event_display&amp;token={/envelope/token}&amp;refresh_interval={/envelope/autorefresh/@interval}&amp;isfind={./isfind}&amp;pagenum={//nowpage}&amp;tips_Event_class={./@type}&amp;tips_Event_time={./OEvent_time}&amp;tips_Sip={./OSip}&amp;tips_Dip={./ODip}&amp;tips_Sport={./OSport}&amp;tips_Sender={./OSender}&amp;tips_Receiver={./@Receiver}&amp;tips_Sen_msg={./@Osen_msg}&amp;tips_sort={./tips_sort}&amp;tips_sortis=1"><font color="blue">产生时间</font></a></th>  
   </thead>
   </table>
<div class="divTbOuter" style="height:315px;">
<table class="openCloseList">
<tbody>
  <xsl:apply-templates select="list" mode="secret_list"/>
</tbody>
  </table>
</div>
  <xsl:apply-templates select="page"/>
</div>
</div>
</div>
</xsl:template>

<!--涉密标志列表-->
<xsl:template match="list" mode="secret_list">
 <tr>
  <td style="width:10px;"><image src="img/style/close.png" class="openCloseImg"/></td>
  <td style="width:15%;"><xsl:value-of select="./Event_class"/></td>
  <td style="width:15%;">子类--无</td>
  <td style="width:15%;"><xsl:value-of select="./Dip"/></td>
  <td style="width:20%;"><xsl:value-of select="./Dmac"/></td>
  <td style="width:15%;"><xsl:value-of select="./Sender"/></td>
  <td><xsl:value-of select="./Event_time"/></td>

 </tr>
<tr style="display:none"><td></td><td colspan="6">
<br/>
  外部主机IP：<xsl:value-of select="./Sip"/><br/>
  端口号：<xsl:value-of select="./Sport"/><br/>
  接受人ID:<xsl:value-of select="./Receiver"/><br/>
  文件名/敏感内容:<xsl:value-of select="./File_name"/><br/>
  匹配的敏感词:<xsl:value-of select="./Sen_msg"/>

</td></tr>
</xsl:template>

<!--通信会话-->
<xsl:template match="tips_dialog">

 <xsl:param name="tipsuser">user</xsl:param>
<div class="workSpace">
<div class="location">您的位置：检索信息>>通信会话</div>
 <script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">
  <form action="/omp" method="get">
   <input type="hidden" name="token" value="{/envelope/token}"/>
   <input type="hidden" name="cmd" value="tips_netlog_display"/>
   <input type="hidden" name="pagenum" value="1"/>
   <input type="hidden" name="refresh_interval" value="{/envelope/autorefresh/@interval}"/>
   <table  cellspacing="0" cellpadding="0"   >
    <tr class="searchTr">
     <td class="tag">内部主机IP：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Dip" value="{./ODip}"/></td>
     <td class="tag">外部主机IP：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Sip" value="{./OSip}"/></td>
     <td class="tag">端口号：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Sport" value="{./OSport}"/></td>
     <td class="tag">通信开始时间：</td>
     <td><input type="text" autocomplete="on" maxlength="50" name="tips_Start_time" value="{./OStart_time}" class="Wdate colMid" onfocus="WdatePicker()"/></td>
     
   </tr>
    <tr class="searchTr">
    <td class="tag">流入数据字节：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Load" value="{./OLoad}"/></td>  
 <td class="tag">流出数据字节：</td>
     <td><input class="shortText" type="text" autocomplete="on" maxlength="50" name="tips_Upload" value="{./OUpload}"/></td>  
	 <td class="tag">协议类型：</td>
	 <td>
      <select name="tips_Protocol" class="shortText">
        <option value="0">所有</option>
        <option value="1">FTP</option>
        <option value="2">POP</option>
        <option value="3">SMTP</option>
        <option value="4">IMAP</option>
        <option value="5">DNS</option>
        <option value="6">IPP</option>
        <option value="7">HTTP</option>
        <option value="8">MDNS</option>
        <option value="9">NTP</option>
        <option value="10">NETBIONS</option>
        <option value="11">NFS</option>
        <option value="12">SSDP</option>
        <option value="13">BGP</option>
        <option value="14">SNMP</option>
        <option value="15">XDMCP</option>
        <option value="16">SMB</option>
        <option value="17">SYSLOG</option>
        <option value="18">DHCP</option>
        <option value="19">POSTGRES</option>
        <option value="20">MYSQL</option>
        <option value="21">TDS</option>
        <option value="22">DDL</option>
        <option value="23">I23V5</option>
        <option value="24">APPLE</option>
        <option value="25">DIRECTCONNECT</option>
        <option value="26">SOCRATES</option>
        <option value="27">WINMX</option>
        <option value="28">MANOLITO</option>
        <option value="29">PANDO</option>
        <option value="30">FILETOPIA</option>
        <option value="31">IMESH</option>
        <option value="32">KONTIKI</option>
        <option value="33">OPENFT</option>
        <option value="34">FASTTRACK</option>
        <option value="35">GNUTELLA</option>
        <option value="36">EDONKEY</option>
        <option value="37">BITTORRENT</option>
        <option value="38">OFF</option>
        <option value="39">AVI</option>
        <option value="40">FLASH</option>
        <option value="41">OGG</option>
        <option value="42">MPEG</option>
        <option value="43">QUICKTIME</option>
        <option value="44">REALMEDIA</option>
        <option value="45">WINDOWSMEDIA</option>
        <option value="46">MMS</option>
        <option value="47">XBOX</option>
        <option value="48">QQ</option>
        <option value="49">MOVE</option>
        <option value="50">RTSP</option>
        <option value="51">FEIDIAN</option>
        <option value="52">ICECAST</option>
        <option value="53">PPLIVE</option>
        <option value="54">PPSTREAM</option>
        <option value="55">ZATTOO</option>
        <option value="56">SHOUTCAST</option>
        <option value="57">SOPCAST</option>
        <option value="58">TVANTS</option>
        <option value="59"></option>
        <option value="60"></option>
        <option value="70"></option>
        <option value="71"></option>
        <option value="72"></option>
        <option value="73"></option>
        <option value="74"></option>
        <option value="75"></option>
        <option value="76"></option>
        <option value="77"></option>
        <option value="78"></option>
        <option value="79"></option>
        <option value="80"></option>
        <option value="81"></option>
        <option value="82"></option>
        <option value="83"></option>
        <option value="84"></option>
        <option value="85"></option>
        <option value="86"></option>
        <option value="87"></option>
        <option value="88"></option>
        <option value="89"></option>
        <option value="90"></option>
        <option value="91"></option>
        <option value="92"></option>
        <option value="93"></option>
        <option value="94"></option>
        <option value="95"></option>
        <option value="96"></option>
        <option value="97"></option>
        <option value="98"></option>
        <option value="99"></option>
        <option value="100"></option>
        <option value="101"></option>
        <option value="102"></option>
        <option value="103"></option>
        <option value="104"></option>
        <option value="105"></option>
        <option value="106"></option>
        <option value="107"></option>
        <option value="108"></option>
        <option value="109"></option>
        <option value="110"></option>
        <option value="111"></option>
        <option value="112"></option>
        <option value="113"></option>
        <option value="114"></option>
        <option value="115"></option>
        <option value="116"></option>
        <option value="117"></option>
        <option value="118"></option>
     </select>
	 </td>

     <td class="tag">通信结束时间：</td>
     <td><input type="text" autocomplete="on" maxlength="50" name="tips_End_time" value="{./OEnd_time}" class="Wdate colMid" onfocus="WdatePicker()"/></td>

    
    </tr>
    <tr class="searchTr">
     <td colspan="8" align="center"><input type="submit" name="submit" value="搜索"/></td>
    </tr>   
    <tr class="msgTr"><td colspan="8"><xsl:apply-templates select="//msg"/></td></tr>
   </table>
  </form> 
  </div></div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询列表</div>
   <div class="content" style="height:380px;">
<div class="listOper">  
     <form method="get" action="/omp" style="display:inline-block">
 			<input type="hidden" name="token" value="{/envelope/token}"/>
    	<input type="hidden" name="cmd" value="tips_netlog_display"/>
    	<input type="hidden" name="isfind" value="{./isfind}"/>
    	<input type="hidden" name="pagenum" value="{//nowpage}"/>
    	<input type="hidden" name="tips_Dip" value="{./ODip}"/>
    	<input type="hidden" name="tips_Sip" value="{./OSip}"/>
    	<input type="hidden" name="tips_Sport" value="{./OSport}"/>
    	<input type="hidden" name="tips_Protocol" value="{./OProtocol}"/>
    	<input type="hidden" name="tips_Start_time" value="{./OStart_time}"/>
    	<input type="hidden" name="tips_End_time" value="{./OEnd_time}"/>
    	<input type="hidden" name="tips_Load" value="{./OLoad}"/>
    	<input type="hidden" name="tips_Upload" value="{./OUpload}"/>
		<input type="hidden" name="tips_sort" value="{./tips_sort}"/>
          <select class="titleSelect" name="refresh_interval">
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='0'">
                <option value="0" selected="1">&#8730;不自动刷新</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="0">不自动刷新</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='10'">
                <option value="10" selected="1">&#8730;每10秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="10">每10秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='30'">
                <option value="30" selected="1">&#8730;每30秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="30">每30秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="/envelope/autorefresh/@interval='60'">
                <option value="60" selected="1">&#8730;每60秒刷新一次</option>
              </xsl:when>
              <xsl:otherwise>
                <option value="60">每60秒刷新一次</option>
              </xsl:otherwise>
            </xsl:choose>
          </select>
          <input style="background:url(img/style/fresh.png) 0 0 no-repeat;height:26px;padding-left: 20px;" type="submit" value="刷新" name="Update"/>
     </form>

<xsl:if test="$tipsuser!='user'">
   <form action="/omp" method="get" style="display:inline-block;">
    <select name="tips_operator" class="titleSelect">
     <option value="tips_output_netlog">导出当页</option>
     <option value="tips_copy_netlog">备份当页</option>
<xsl:comment><option value="tips_delete_netlog">删除当页</option></xsl:comment>
     <option value="tips_output_netlogf">导出全部</option>
     <option value="tips_copy_netlogf">备份全部</option>
<xsl:comment><option value="tips_delete_netlogf">删除全部</option></xsl:comment>
    </select>
    <input type="hidden" name="token" value="{/envelope/token}"/>
    <input type="hidden" name="cmd" value="tips_netlog_opt"/>
    <input type="hidden" name="isfind" value="{./isfind}"/>
    <input type="hidden" name="pagenum" value="{//nowpage}"/>
    <input type="hidden" name="refresh_interval" value="{/envelope/autorefresh/@interval}"/>
    <input type="hidden" name="tips_Dip" value="{./ODip}"/>
    <input type="hidden" name="tips_Sip" value="{./OSip}"/>
    <input type="hidden" name="tips_Sport" value="{./OSport}"/>
    <input type="hidden" name="tips_Protocol" value="{./OProtocol}"/>
    <input type="hidden" name="tips_Start_time" value="{./OStart_time}"/>
    <input type="hidden" name="tips_End_time" value="{./OEnd_time}"/>
    <input type="hidden" name="tips_Load" value="{./OLoad}"/>
    <input type="hidden" name="tips_Upload" value="{./OUpload}"/>
	<input type="hidden" name="tips_sort" value="{./tips_sort}"/>
    <input type="submit" name="output" value="提交" />
   </form>
 
</xsl:if>

 </div>
  <table class="list" cellspacing="0" rownum="20">
   <thead class="listhead"><tr>
    <th style="width:10px;"></th>
    <th style="width:20%;">源主机IP--内部主机ip</th>
    <th style="width:20%;">源端口号--内部主机端口</th>
    <th style="width:20%;">协议类型</th>
	<th style="width:20%;"><a style="color:#333" href="/omp?cmd=tips_netlog_display&amp;token={/envelope/token}&amp;refresh_interval={/envelope/autorefresh/@interval}&amp;isfind={./isfind}&amp;pagenum={//nowpage}&amp;tips_Sip={./OSip}&amp;tips_Dip={./ODip}&amp;tips_Sport={./OSport}&amp;tips_Protocol={./OProtocol}&amp;tips_Start_time={./OStart_time}&amp;tips_End_time={./OEnd_time}&amp;tips_Load={./OLoad}&amp;tips_Upload={./OUpload}&amp;tips_sort={./tips_sort}&amp;tips_sortis=1"><font color="blue">通信开始时间</font></a></th>  
   <th>流入数据字节</th></tr>
   </thead>
   </table>
<div class="divTbOuter" style="height:315px;">
<table class="openCloseList">
<tbody>
  <xsl:apply-templates select="list" mode="dialog_list"/>
</tbody>  
</table>
</div>
  <xsl:apply-templates select="page"/>
</div>
</div>
</div>
</xsl:template>

<!--通信会话列表-->
<xsl:template match="list" mode="dialog_list">
 <tr>
  <td style="width:10px;"><image src="img/style/close.png" class="openCloseImg"/></td>
  <td style="width:20%;"><xsl:value-of select="./Dip"/></td>
  <td style="width:20%;"><xsl:value-of select="./Dport"/></td>
  <td style="width:20%;"><xsl:value-of select="./Protocol"/></td>
  <td style="width:20%;"><xsl:value-of select="./Start_time"/></td>
  <td><xsl:value-of select="./Load"/></td>
 </tr>
<tr style="display:none;"><td></td>
<td colspan="5">
  外部主机ip:<xsl:value-of select="./Sip"/><br/>
外部主机端口：<xsl:value-of select="./Sport"/><br/>
 通信结束时间:<xsl:value-of select="./End_time"/><br/>
  流出数据字节:<xsl:value-of select="./Upload"/>
</td></tr>
</xsl:template>

<!--审计查询-->
<xsl:template match="audit">
<div class="workSpace">
<div class="location">您的位置：审计查询</div>


<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">
  <form action="/omp" method="get">
   <input type="hidden" name="token" value="{/envelope/token}"/>
   <input type="hidden" name="cmd" value="tips_opt_display"/>
   <input type="hidden" name="pagenum" value="1"/>
   <table  cellspacing="0"  >
    <tr class="searchTr">
     <td class="tag">用户名：</td>
     <td><input class="text" type="text" autocomplete="on" maxlength="50" name="tips_username" value="{./Ousername}"/></td>
     <td class="tag">操作描述：</td>
     <td><input class="text" type="text" autocomplete="on" maxlength="50" name="tips_Optdsc" value="{./OOptdsc}"/></td>
     <td class="tag">操作时间：</td>
     <td><input type="text" autocomplete="on" maxlength="50" name="tips_Otime" value="{./OOtime}" class="Wdate colMid" onfocus="WdatePicker()"/>--<input type="text" autocomplete="on" maxlength="50" name="tips_Oetime" value="{./OOetime}" class="Wdate colMid" onfocus="WdatePicker()"/></td>
     <comment><td><xsl:apply-templates select="period"/></td></comment>
     <td><input type="submit" name="submit" value="查询"/></td>
    </tr>
<tr class="msgTr"><td colspan="4"> <xsl:apply-templates select="msg"/></td></tr>
   </table>
  </form> 
</div></div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询列表</div>
   <div class="content" style="height:435px;">
 
  <div class="listOper">
 
    <form action="/omp" method="get"  style="display:inline-block;">
   	 <input type="hidden" name="token" value="{/envelope/token}"/>
   	 <input type="hidden" name="cmd" value="tips_opt_output"/>
     <input type="hidden" name="pagenum" value="{//nowpage}"/>
     <input type="hidden" name="tips_username" value="{./Ousername}"/>
     <input type="hidden" name="tips_Optdsc" value="{./OOptdsc}"/>
     <input type="hidden" name="tips_Otime" value="{./OOtime}"/>
     <input type="hidden" name="tips_Oetime" value="{./OOetime}"/>
	 <input type="hidden" name="tips_sort" value="{./tips_sort}"/>
    <input type="submit" name="submit" value="导出全部"/>
    </form>
    <form action="/omp" method="get"  style="display:inline-block;">
   	 <input type="hidden" name="token" value="{/envelope/token}"/>
   	 <input type="hidden" name="cmd" value="tips_opt_copy"/>
     <input type="hidden" name="pagenum" value="{//nowpage}"/>
     <input type="hidden" name="tips_username" value="{./Ousername}"/>
     <input type="hidden" name="tips_Optdsc" value="{./OOptdsc}"/>
     <input type="hidden" name="tips_Otime" value="{./OOtime}"/>
     <input type="hidden" name="tips_Oetime" value="{./OOetime}"/>
     <input type="hidden" name="tips_sort" value="{./tips_sort}"/>
<input type="submit" name="submit" value="备份全部"/>
		</form>
  
  </div>
  <table class="list" cellspacing="0">
   <thead>
    <th width="25%">用户名</th>
    <th width="50%">操作描述</th>
	<th width="25%"><a style="color:#333" href="/omp?cmd=tips_opt_display&amp;token={/envelope/token}&amp;isfind={./isfind}&amp;pagenum={//nowpage}&amp;tips_username={./Ousername}&amp;tips_Optdsc={./OOptdsc}&amp;tips_Otime={./OOtime}&amp;tips_Oetime={./OOetime}&amp;tips_sort={./tips_sort}&amp;tips_sortis=1"><font color="blue">操作时间</font></a></th>  
   </thead>
</table>
<div class="divTbOuter" style="height:370px;">
<table class="list">
<tbody>
  <xsl:apply-templates select="list" mode="audit_list"/>

</tbody>
  </table>
</div>
  <xsl:apply-templates select="page"/>
</div>
</div>
</div>
</xsl:template>

<!--审计列表-->
<xsl:template match="list" mode="audit_list">
 <tr>
  <td width="25%"><xsl:value-of select="username"/></td>
  <td width="50%"><xsl:value-of select="Optdsc"/></td>
  <td width="25%"><xsl:value-of select="Otime"/></td>
 </tr>
</xsl:template>

<!--产品维护-设备状态-->
<xsl:template match="product">
<div class="workSpace">
<div class="location">您的位置：产品维护</div>
<div class="divBlock">	
   	<div class="title"><div class="title_icon"></div>系统操作</div>
	<div class="content">    
	<p class=" heighTr"  style="text-align:center">
	    <input type="button" name="update" value="系统升级" onclick="javascript:top.location='/omp?cmd=upgrade&amp;overrides=1&amp;token={/envelope/token}'"/>
	     <input type="button" name="format" value="恢复出厂" onclick="javascript:self.location='/omp?cmd=restore&amp;overrides=1&amp;token={/envelope/token}'"/>
	    <input type="button" name="shut" value="系统关机"  onclick="javascript:self.location='/omp?cmd=close&amp;overrides=1&amp;token={/envelope/token}'"/>
	    <input type="button" name="restart" value="系统重启" onclick="javascript:self.location='/omp?cmd=reboot&amp;overrides=1&amp;token={/envelope/token}'"/>
	    <input type="button" name="remote_debugging" value="远程调试" onclick="javascript:self.location='/omp?cmd=debuging&amp;overrides=1&amp;token={/envelope/token}'"/>
	</p>
	</div>
</div>
 <table cellspacing="2" cellpadding="4" border="0">
   <tr>
      <td align="center"><font color="red"><xsl:value-of select="debug"/></font></td>
    </tr>
 </table>
<div class="divBlock">	
   	<div class="title"><div class="title_icon"></div>设备状态信息</div>
	<div class="content" style="height:400px;">  
	  <fieldset class="fSet">
	   <legend>设备基本信息>></legend>
	   <form action="/omp" method="post">
	   <input type="hidden" name="token" value=""/>
	   <input type="hidden" name="cmd" value=""/> 
	   <table cellspacing="0">
	   <tr>
	    <td class="tag">本次开机时间</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{dstart}"/></td>
	   </tr>
	   <tr>
	   	<xsl:choose>
	   	<xsl:when test="cpucolor='1'">
	    	<td class="tag" style="color:#FF0000">CUP利用率</td>
	    	<td><input class="show" type="text" readonly="1"  style="color:#FF0000" name="" value="{dcpu}"/></td>
	    </xsl:when>
	    <xsl:otherwise>
	    	<td class="tag">CUP利用率</td>
	    	<td><input class="show" type="text" readonly="1" name="" value="{dcpu}"/></td>
	    </xsl:otherwise>
	    </xsl:choose>
	   </tr>
	   <tr>
	    <xsl:choose>
	    <xsl:when test="memcolor='1'">
	     <td class="tag" style="color:#FF0000">内存利用率</td>
	     <td><input class="show" type="text" readonly="1"  style="color:#FF0000" name="" value="{dmem}"/></td>
	    </xsl:when>
	    <xsl:otherwise>
	     <td class="tag">内存利用率</td>
	     <td><input class="show" type="text" readonly="1" name="" value="{dmem}"/></td>
	    </xsl:otherwise>
	    </xsl:choose>
	   </tr>
	   <tr>
	   <xsl:choose>
	   <xsl:when test="diskcolor='1'">
	    <td class="tag" style="color:#FF0000">磁盘剩余空间</td>
	    <td><input class="show" type="text" readonly="1" style="color:#FF0000" name="" value="{ddisk}"/></td>
	   </xsl:when>
	   <xsl:otherwise>
	    <td class="tag">磁盘剩余空间</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{ddisk}"/></td>
	   </xsl:otherwise>
	   </xsl:choose>
	   </tr>
	   <tr>
	    <td class="tag">镜像数据接入状态</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{dmirror}"/></td>
	   </tr>
	   <tr>
	    <td class="tag">镜像数据流量</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{dflow}"/></td>
	   </tr> 
	  </table>
	  </form>
	  </fieldset>
	  <fieldset class="fSet">
	   <legend>功能运行状态>></legend>
          <table>
	   <tr>
	    <td class="tag">攻击窃密检测</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{dattach}"/></td>
	   </tr>
	   <tr>
	    <td class="tag">网络行为审计</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{dnetlog}"/></td>
	   </tr>
	   <tr>
	    <td class="tag">涉密标识检测</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{dsen}"/></td>
	   </tr>
	   <tr>
	    <td class="tag">网络窃密阻断</td>
	    <td><input class="show" type="text" readonly="1" name="" value="{dblock}"/></td>
	   </tr>
	</table>
	  </fieldset> 
	</div>
</div>
</div>
</xsl:template>


<!--产品维护-系统设置-->
<xsl:template match="sysSetting">
<div class="workSpace">
<div class="location">您的位置：产品维护</div>
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>系统操作</div>
   <div class="content">
    <p class="heighTr" style="text-align:center">
     <input type="button" name="update" value="系统升级" onclick="javascript:top.location='/omp?cmd=upgrade_1&amp;overrides=1&amp;token={/envelope/token}'"/>
     <input type="button" name="format" value="恢复出厂" onclick="javascript:self.location='/omp?cmd=restore&amp;overrides=1&amp;token={/envelope/token}'"/>
     <input type="button" name="shut" value="系统关机"  onclick="javascript:self.location='/omp?cmd=close&amp;overrides=1&amp;token={/envelope/token}'"/>
     <input type="button" name="restart" value="系统重启" onclick="javascript:self.location='/omp?cmd=reboot&amp;overrides=1&amp;token={/envelope/token}'"/>
     <input type="button" name="remote_debugging" value="远程调试" onclick="javascript:self.location='/omp?cmd=debuging_1&amp;overrides=1&amp;token={/envelope/token}'"/>
    </p>
</div></div>
 <table cellspacing="2" cellpadding="4" border="0">
   <tr>
      <td align="center"><font color="red"><xsl:value-of select="debug"/></font></td>
    </tr>
 </table>
<div class="divBlock">	
   	<div class="title"><div class="title_icon"></div>系统设置</div>
	<div class="content"  style="height:400px;">  
	<form action="/omp?cmd=system_config" method="post">
  		<fieldset class="fSet">
	   		<legend>阈值设置</legend>
	   		
	   		<input type="hidden" name="token" value="{/envelope/token}"/>
	  		<input type="hidden" name="cmd" value="system_config"/> 
	   		<table cellspacing="0">			  
			   <tr class="searchTr">
			    <td class="tag" style="margin-right:20px;">CPU利用率</td>
			    <td style="padding-left:20px;"><input class="text" type="text" autocomplete="on" maxlength="50" name="cpu" value="{./cpu}"/>%</td>
			   </tr>
			   <tr class="searchTr">
			    <td class="tag">内存利用率</td>
			    <td style="padding-left:20px;"><input class="text" type="text" autocomplete="on" maxlength="50" name="mem" value="{./mem}"/>%</td>
			   </tr>
			   <tr class="searchTr">
			    <td class="tag">磁盘剩余空间</td>
			    <td style="padding-left:20px;"><input class="text" type="text" autocomplete="on" maxlength="50" name="disk" value="{./disk}"/>GB</td>
			   </tr>
   			    <tr class="msgTr">			
			    <td colspan="2"><xsl:apply-templates select="msg"/></td>
			   </tr>
			</table>
		</fieldset>
		<fieldset class="fSet">
			 <legend>定时上报设置</legend>
			<table>
			   
			   <tr class="searchTr">
			    <td class="tag">上报运行状态间隔时间</td>
			    <td style="padding-left:20px;"><input class="text" type="text" autocomplete="on" maxlength="50" name="runtime" value="{./runtime}"/>s</td>
			   </tr>
			   <tr class="searchTr">
			    <td class="tag">上报报警信息间隔时间</td>
			    <td style="padding-left:20px;"><input class="text" type="text" autocomplete="on" maxlength="50" name="alarmtime" value="{./alarmtime}"/>s</td>
			   </tr>
			   <tr class="msgTr">			
			    <td colspan="2"><xsl:apply-templates select="msg_product"/></td>
			   </tr>
			 
			  </table>
		  </fieldset> 
		  <table>
		<tr align="center"><td colspan="2" style="padding-left:417px;"><input type="submit" name="submit" value="提交" /></td></tr>
			  </table>
  	</form>
	</div>
</div>
</div>
</xsl:template>


<!--统计信息  攻击窃密-->
<xsl:template match="information_attack">
<div class="workSpace">
<div class="location">您的位置：统计信息>>攻击窃密</div>

<script src="/script/jscharts.js" language="javascript"  type="text/javascript"></script>
<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>
 <script type="text/javascript">
        $(function () {

	    var lineDataStr="<xsl:value-of select="lineDataStr"/>";
             lineDataStr = "new Array(" + lineDataStr + ")";
            var lineDataArr = eval(lineDataStr); 
	   var countX=lineDataArr.length;   
             
           var myChart = new JSChart('lineChart', 'line');
            myChart.setDataArray(lineDataArr);
    
            myChart.setAxisNameX('报警时间');
            myChart.setAxisNameY('报警数量');
	    myChart.setAxisValuesNumberY(6);
	    myChart.setAxisValuesNumberX(countX);
	    myChart.setAxisPaddingLeft(50);
	    myChart.setAxisPaddingBottom(50);

            myChart.setLineColor('#2d5797');  
            myChart.setLineWidth(3);  
            myChart.setAxisValuesColor('#000');  
            myChart.setTitle(' '); 
            myChart.draw();
         
      

	    var barDataStr="<xsl:value-of select="barDataStr"/>";
             barDataStr = "new Array(" + barDataStr + ")";
            var barDataArr = eval(barDataStr);                    
            var colors=['#97d1f7','#e3deb8','#c9e8bf','#85abcf'];

            var myChart2 = new JSChart('barChart', 'bar');
            myChart2.setDataArray(barDataArr);
	    myChart2.colorizeBars(colors);
            myChart2.setAxisNameX('报警类型');
            myChart2.setAxisNameY('报警数量');
	    myChart2.setAxisPaddingLeft(50);
	    myChart2.setAxisPaddingBottom(50);
            myChart2.setAxisValuesNumberY(6);
	    myChart2.setBarSpacingRatio(50);//柱状之间的距离，决定着柱的宽度
            myChart2.setTitle(' ');     
            myChart2.draw();
            
        })

    </script>
     <form action="/omp" method="get">
      <input type="hidden" name="token" value="{/envelope/token}"/>
      <input type="hidden" name="cmd" value="get_information_attack"/>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">

 <p class="searchTr"><span class="tag">操作时间：</span><td><input autocomplete="off" maxlength="50" name="strTime" type="text" value="{./StrTime}" class="Wdate colMid" onfocus="WdatePicker()"/>--<input name="endTime" type="text" maxlength="50" value="{./EndTime}" class="Wdate colMid" onfocus="WdatePicker()"/><input type="submit" name="submit" value="查询" style="margin-left:44px;"/></td></p>
 
<div class="msgTr"><xsl:apply-templates select="picture_msg"/></div>
</div></div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>统计分析</div>
   <div class="content" style="height:430px;">
 <div id="lineChart" style="width:400px;height:300px;float:left;padding-left:20px;">正在加载... </div>
 <div id="barChart" style="width:400px;height:300px;display:inline-block;"> 正在加载...</div>
</div></div>
  
     </form>
</div>
</xsl:template>

<!--统计信息  操作审计-->
<xsl:template match="information_operation">
<div class="workSpace">
<div class="location">您的位置：统计信息>>操作审计</div>
<script src="/script/jscharts.js" language="javascript"  type="text/javascript"></script>
<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>
 <script type="text/javascript">
     $(function () {

	    var lineDataStr="<xsl:value-of select="lineDataStr"/>";
             lineDataStr = "new Array(" + lineDataStr + ")";
            var lineDataArr = eval(lineDataStr); 
	   var countX=lineDataArr.length;   
             
           var myChart = new JSChart('lineChart', 'line');
            myChart.setDataArray(lineDataArr);
    
            myChart.setAxisNameX('操作时间');
            myChart.setAxisNameY('操作描述');
	    myChart.setAxisValuesNumberY(6);
	    myChart.setAxisValuesNumberX(countX);
	    myChart.setAxisPaddingLeft(50);
	    myChart.setAxisPaddingBottom(50);

            myChart.setLineColor('#2d5797');  
            myChart.setLineWidth(3);  
            myChart.setAxisValuesColor('#000');  
            myChart.setTitle(' '); 
            myChart.draw();
         
      

        })
    </script>
     <form action="/omp" method="get">
      <input type="hidden" name="token" value="{/envelope/token}"/>
      <input type="hidden" name="cmd" value="get_information_operation"/>


<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">
    <p class="searchTr"><span class="tag">操作时间：</span><input autocomplete="off" maxlength="50" name="strTime" type="text" value="{./StrTime}" class="Wdate colMid" onfocus="WdatePicker()"/>--<input name="endTime" type="text" maxlength="50" value="{./EndTime}" class="Wdate colMid" onfocus="WdatePicker()"/><input type="submit" name="submit" value="查询" style="margin-left:44px;"/></p>
<div class="msgTr"><xsl:apply-templates select="picture_msg"/></div>
</div></div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>统计分析</div>
   <div class="content" style="height:430px;">
 <div id="lineChart" style="width:400px;height:300px;float:left;padding-left:20px;">正在加载... </div>
</div></div>
    </form> 
</div>
</xsl:template>

<!--统计信息  涉密标识-->
<xsl:template match="information_secret">
<div class="workSpace">
<div class="location">您的位置：统计信息>>涉密标识</div>
<script src="/script/jscharts.js" language="javascript"  type="text/javascript"></script>
<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>
 <script type="text/javascript">
    $(function () {

	    var lineDataStr="<xsl:value-of select="lineDataStr"/>";
             lineDataStr = "new Array(" + lineDataStr + ")";
            var lineDataArr = eval(lineDataStr); 
	   var countX=lineDataArr.length;   
             
           var myChart = new JSChart('lineChart', 'line');
            myChart.setDataArray(lineDataArr);
    
            myChart.setAxisNameX('产生时间');
            myChart.setAxisNameY('事件数量');
	    myChart.setAxisValuesNumberY(6);
	    myChart.setAxisValuesNumberX(countX);
	    myChart.setAxisPaddingLeft(50);
	    myChart.setAxisPaddingBottom(50);

            myChart.setLineColor('#2d5797');  
            myChart.setLineWidth(3);  
            myChart.setAxisValuesColor('#000');  
            myChart.setTitle(' '); 
            myChart.draw();
         
      

	    var barDataStr="<xsl:value-of select="barDataStr"/>";
             barDataStr = "new Array(" + barDataStr + ")";
            var barDataArr = eval(barDataStr);                    
            var colors=['#97d1f7','#e3deb8','#c9e8bf','#85abcf'];

            var myChart2 = new JSChart('barChart', 'bar');
            myChart2.setDataArray(barDataArr);
	    myChart2.colorizeBars(colors);
            myChart2.setAxisNameX('事件类型');
            myChart2.setAxisNameY('事件数量');
	    myChart2.setAxisPaddingLeft(50);
	    myChart2.setAxisPaddingBottom(50);
            myChart2.setAxisValuesNumberY(6);
	    myChart2.setBarSpacingRatio(50);//柱状之间的距离，决定着柱的宽度
            myChart2.setTitle(' ');     
            myChart2.draw();
            
        })
    </script>
     <form action="/omp" method="get">
      <input type="hidden" name="token" value="{/envelope/token}"/>
      <input type="hidden" name="cmd" value="get_information_secret"/>


<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">
<p class="searchTr"><span class="tag">操作时间：</span><input autocomplete="off" maxlength="50" name="strTime" type="text" value="{./StrTime}" class="Wdate colMid" onfocus="WdatePicker()"/>--<input name="endTime" maxlength="50" type="text" value="{./EndTime}" class="Wdate colMid" onfocus="WdatePicker()"/><input type="submit" name="submit" value="查询" style="margin-left:44px;"/></p>
<div class="msgTr"><xsl:apply-templates select="picture_msg"/></div>
</div></div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>统计分析</div>
   <div class="content" style="height:430px;">
 <div id="lineChart" style="width:400px;height:300px;float:left;padding-left:20px;">正在加载... </div>
 <div id="barChart" style="width:400px;height:300px;display:inline-block;"> 正在加载...</div>
</div></div>
</form>  
</div>
</xsl:template>

<!--统计信息  行为审计-->
<xsl:template match="information_action">
<div class="workSpace">
<div class="location">您的位置：统计信息>>行为审计</div>
<script src="/script/jscharts.js" language="javascript"  type="text/javascript"></script>
<script src="/script/My97DatePicker/WdatePicker.js" language="javascript"  type="text/javascript"></script>
<link href="/script/My97DatePicker/skin/WdatePicker.css" rel="stylesheet"/>
 <script type="text/javascript">
         $(function () {

	    var lineDataStr="<xsl:value-of select="lineDataStr"/>";
             lineDataStr = "new Array(" + lineDataStr + ")";
            var lineDataArr = eval(lineDataStr); 
	   var countX=lineDataArr.length;   
             
           var myChart = new JSChart('lineChart', 'line');
            myChart.setDataArray(lineDataArr);
    
            myChart.setAxisNameX('通信时间');
            myChart.setAxisNameY('行为数量');
	    myChart.setAxisValuesNumberY(6);
	    myChart.setAxisValuesNumberX(countX);
	    myChart.setAxisPaddingLeft(50);
	    myChart.setAxisPaddingBottom(50);

            myChart.setLineColor('#2d5797');  
            myChart.setLineWidth(3);  
            myChart.setAxisValuesColor('#000');  
            myChart.setTitle(' '); 
            myChart.draw();
         
      

        })
    </script>
       <form action="/omp" method="get">
        <input type="hidden" name="token" value="{/envelope/token}"/>
        <input type="hidden" name="cmd" value="get_information_action"/>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>查询信息</div>
   <div class="content">
<p class="searchTr"><span class="tag">操作时间：</span><input autocomplete="off" maxlength="50" name="strTime" type="text" value="{./StrTime}" class="Wdate colMid" onfocus="WdatePicker()"/>--<input name="endTime" type="text" maxlength="50" value="{./EndTime}" class="Wdate colMid" onfocus="WdatePicker()"/><input type="submit" name="submit" value="查询" style="margin-left:44px;"/></p>

<div class="msgTr"><xsl:apply-templates select="picture_msg"/></div>
</div></div>

<div class="divBlock">
   <div class="title"><div class="title_icon"></div>统计分析</div>
   <div class="content" style="height:430px;">
 <div id="lineChart" style="width:400px;height:300px;float:left;padding-left:20px;">正在加载... </div>
</div></div>
 </form> 
</div>
</xsl:template>


<!--配置管理的提示-->
<xsl:template match="picture_msg">
	<td colspan="4" style="text-align:center;"><font color="red"><xsl:value-of select="."/></font></td>
</xsl:template>


<!--账户管理-->
<xsl:template name="html-create-config-form">
<div class="divBlock">
   <div class="title"><div class="title_icon"></div>修改密码</div>
   <div class="content">
  <form action="" method="post">
   <table  cellspacing="0">
    <tr>
     <td>用户名:</td>
     <td> <input maxlength="50" type="text" /></td>
     <td>新密码：</td>
     <td><input maxlength="50" type="password" /></td>
     <td>确认密码：</td>d
     <td><input maxlength="50" type="password" /></td>
     <td colspan="2"><input type="submit" name="submit" value="保存"/></td>
    </tr>
   </table>
  </form> 
</div></div>
</xsl:template>

<!--时间选择-->
<xsl:template name="period">
 <select name="hour">
                <option value="00" selected="1">00</option>
                <option value="01">01</option>
                <option value="02">02</option>
                <option value="03">03</option>
                <option value="04">04</option>
                <option value="05">05</option>
                <option value="06">06</option>
                <option value="07">07</option>
                <option value="08">08</option>
                <option value="09">09</option>
                <option value="10">10</option>
                <option value="11">11</option>
                <option value="12">12</option>
                <option value="13">13</option>
                <option value="14">14</option>
                <option value="15">15</option>
                <option value="16">16</option>
                <option value="17">17</option>
                <option value="18">18</option>
                <option value="19">19</option>
                <option value="20">20</option>
                <option value="21">21</option>
                <option value="22">22</option>
                <option value="23">23</option>
                <option value="24">24</option>
              </select>时
 <select name="minute">
                <option value="00" selected="1">00</option>
                <option value="01">01</option>
                <option value="02">02</option>
                <option value="03">03</option>
                <option value="04">04</option>
                <option value="05">05</option>
                <option value="06">06</option>
                <option value="07">07</option>
                <option value="08">08</option>
                <option value="09">09</option>
                <option value="10">10</option>
                <option value="11">11</option>
                <option value="12">12</option>
                <option value="13">13</option>
                <option value="14">14</option>
                <option value="15">15</option>
                <option value="16">16</option>
                <option value="17">17</option>
                <option value="18">18</option>
                <option value="19">19</option>
                <option value="20">20</option>
                <option value="21">21</option>
                <option value="22">22</option>
                <option value="23">23</option>
                <option value="24">24</option>
              </select>分
<select name="day">
                <option value="00" selected="1">00</option>
                <option value="01">01</option>
                <option value="02">02</option>
                <option value="03">03</option>
                <option value="04">04</option>
                <option value="05">05</option>
                <option value="06">06</option>
                <option value="07">07</option>
                <option value="08">08</option>
                <option value="09">09</option>
                <option value="10">10</option>
                <option value="11">11</option>
                <option value="12">12</option>
                <option value="13">13</option>
                <option value="14">14</option>
                <option value="15">15</option>
                <option value="16">16</option>
                <option value="17">17</option>
                <option value="18">18</option>
                <option value="19">19</option>
                <option value="20">20</option>
                <option value="21">21</option>
                <option value="22">22</option>
                <option value="23">23</option>
                <option value="24">24</option>
              </select>
<select name="month">
                <option value="00" selected="1">00</option>
                <option value="01">01</option>
                <option value="02">02</option>
                <option value="03">03</option>
                <option value="04">04</option>
                <option value="05">05</option>
                <option value="06">06</option>
                <option value="07">07</option>
                <option value="08">08</option>
                <option value="09">09</option>
                <option value="10">10</option>
                <option value="11">11</option>
                <option value="12">12</option>
                <option value="13">13</option>
                <option value="14">14</option>
                <option value="15">15</option>
                <option value="16">16</option>
                <option value="17">17</option>
                <option value="18">18</option>
                <option value="19">19</option>
                <option value="20">20</option>
                <option value="21">21</option>
                <option value="22">22</option>
                <option value="23">23</option>
                <option value="24">24</option>
              </select>月
<select name="year">
                <option value="00" selected="1">00</option>
                <option value="01">01</option>
                <option value="02">02</option>
                <option value="03">03</option>
                <option value="04">04</option>
                <option value="05">05</option>
                <option value="06">06</option>
                <option value="07">07</option>
                <option value="08">08</option>
                <option value="09">09</option>
                <option value="10">10</option>
                <option value="11">11</option>
                <option value="12">12</option>
                <option value="13">13</option>
                <option value="14">14</option>
                <option value="15">15</option>
                <option value="16">16</option>
                <option value="17">17</option>
                <option value="18">18</option>
                <option value="19">19</option>
                <option value="20">20</option>
                <option value="21">21</option>
                <option value="22">22</option>
                <option value="23">23</option>
                <option value="24">24</option>
              </select>年
</xsl:template>

<!-- 和用户管理的列表有关 -->

<xsl:template match="get_tasks_response">
  <xsl:choose>
    <xsl:when test="substring(@status, 1, 1) = '4' or substring(@status, 1, 1) = '5'">
      <xsl:call-template name="command_result_dialog">
        <xsl:with-param name="operation">Get Tasks</xsl:with-param>
        <xsl:with-param name="status">
          <xsl:value-of select="@status"/>
        </xsl:with-param>
        <xsl:with-param name="msg">
          <xsl:value-of select="@status_text"/>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="task/reports">
      <xsl:call-template name="html-report-table"/>
    </xsl:when>
    <xsl:when test="@modify_user_page">
	<xsl:call-template name="user_amend"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="html-user-table"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!-- 和配置管理有关 -->

<xsl:template match="get_reports_response">
  <xsl:choose>
    <xsl:when test="substring(@status, 1, 1) = '4' or substring(@status, 1, 1) = '5'">
      <xsl:call-template name="command_result_dialog">
        <xsl:with-param name="operation">
          Get Report
        </xsl:with-param>
        <xsl:with-param name="status">
          <xsl:value-of select="@status"/>
        </xsl:with-param>
        <xsl:with-param name="msg">
          <xsl:value-of select="@status_text"/>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:for-each select="report">
        <xsl:choose>
          <xsl:when test="@type = 'config'">
            <xsl:call-template name="config"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="html-report-details"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="page">
  <div class="page">
    <a href="{tips_urlf}">首页</a>
    <xsl:choose>
     <xsl:when test="nowpage = 1">
      <a class="unable">上一页</a>
     </xsl:when>
     <xsl:otherwise>
      <a href="{tips_urlp}">上一页</a> 
     </xsl:otherwise>
    </xsl:choose>

     <xsl:if test="nowpage &gt; 10">
      <a href="{tips_urlpp}">...</a>
     </xsl:if>
    <xsl:choose>
     <xsl:when test="(nowpage mod 10) != 1">
      <a href="{tips_url1}"><xsl:value-of select="page1"/></a>
     </xsl:when>
     <xsl:otherwise>
      <a class="unable"><xsl:value-of select="page1"/></a>
     </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 1">
     <xsl:choose>
      <xsl:when test="(nowpage mod 10) != 2">
       <a href="{tips_url2}"><xsl:value-of select="page2"/></a>
      </xsl:when>
      <xsl:otherwise>
       <a class="unable"><xsl:value-of select="page2"/></a>
      </xsl:otherwise>
     </xsl:choose>
     <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 2">
      <xsl:choose>
       <xsl:when test="(nowpage mod 10) != 3">
        <a href="{tips_url3}"><xsl:value-of select="page3"/></a>
       </xsl:when>
       <xsl:otherwise>
        <a class="unable"><xsl:value-of select="page3"/></a>
       </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 3">
       <xsl:choose>
        <xsl:when test="(nowpage mod 10) != 4">
         <a href="{tips_url4}"><xsl:value-of select="page4"/></a>
        </xsl:when>
        <xsl:otherwise>
         <a class="unable"><xsl:value-of select="page4"/></a>
	</xsl:otherwise>
       </xsl:choose>
       <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 4">
	<xsl:choose>
	 <xsl:when test="(nowpage mod 10) != 5">
          <a href="{tips_url5}"><xsl:value-of select="page5"/></a>
	 </xsl:when>
	 <xsl:otherwise>
          <a class="unable"><xsl:value-of select="page5"/></a>
	 </xsl:otherwise>
	</xsl:choose>
        <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 5">
	 <xsl:choose>
	  <xsl:when test="(nowpage mod 10) != 6">
           <a href="{tips_url6}"><xsl:value-of select="page6"/></a>
	  </xsl:when>
	  <xsl:otherwise>
           <a class="unable"><xsl:value-of select="page6"/></a>
	  </xsl:otherwise>
	 </xsl:choose>
         <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 6">
	  <xsl:choose>
	   <xsl:when test="(nowpage mod 10) != 7">
            <a href="{tips_url7}"><xsl:value-of select="page7"/></a>
	   </xsl:when>
	   <xsl:otherwise>
            <a class="unable"><xsl:value-of select="page7"/></a>
	   </xsl:otherwise>
	  </xsl:choose>
          <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 7">
	   <xsl:choose>
	    <xsl:when test="(nowpage mod 10) != 8">
             <a href="{tips_url8}"><xsl:value-of select="page8"/></a>
	    </xsl:when>
	    <xsl:otherwise>
             <a class="unable"><xsl:value-of select="page8"/></a>
	    </xsl:otherwise>
	   </xsl:choose>
           <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 8">
	    <xsl:choose>
	     <xsl:when test="(nowpage mod 10) != 9">
              <a href="{tips_url9}"><xsl:value-of select="page9"/></a>
	     </xsl:when>
	     <xsl:otherwise>
              <a class="unable"><xsl:value-of select="page9"/></a>
	     </xsl:otherwise>
	    </xsl:choose>
            <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 9">
	     <xsl:choose>
	      <xsl:when test="(nowpage mod 10) != 0">
               <a href="{tips_url10}"><xsl:value-of select="page10"/></a>
	      </xsl:when>
	      <xsl:otherwise>
               <a class="unable"><xsl:value-of select="page10"/></a>
	      </xsl:otherwise>
	     </xsl:choose>
	    </xsl:if>
	   </xsl:if>
	  </xsl:if>
	 </xsl:if>
	</xsl:if>
       </xsl:if>
      </xsl:if>
     </xsl:if>
    </xsl:if>
    <xsl:if test="(lastpage -(nowpage - (((nowpage - 1) mod 10)+1))) &gt; 10">
      <a href="{tips_urlnp}">...</a>
    </xsl:if>
    <xsl:choose>
     <xsl:when test="(lastpage &gt; nowpage)">
      <a href="{tips_urln}">下一页</a>
     </xsl:when>
     <xsl:otherwise>
      <a class="unable">下一页</a>
     </xsl:otherwise>
    </xsl:choose>
    <a href="{tips_urll}">尾页</a>
  </div>
</xsl:template>

</xsl:stylesheet>
