<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output
      method="xml"
      encoding="UTF-8"/>


<xsl:template match="/">
 <title><xsl:value-of select="envelope/title"/></title>
	<xsl:apply-templates select="envelope/block"/>
	<xsl:apply-templates select="envelope/attack"/>
 <xsl:comment><![CDATA[<标题>]]><xsl:value-of select="./title"/><![CDATA[</标题>]]></xsl:comment>
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="block">
  <筛选条件>
  <内部主机IP>aa</内部主机IP>
  <外部主机IP>ee</外部主机IP>
  <端口号>bb</端口号>
  <开始阻断时间>cc</开始阻断时间>
  <阻断失效时间>dd</阻断失效时间>
  </筛选条件>
  <筛选结果>
	ff
  </筛选结果>
</xsl:template>

<xsl:template match="stop_list" mode="block">
  <内部主机IP><xsl:value-of select="./Dip"/></内部主机IP>
  <外部主机IP><xsl:value-of select="./Sip"/></外部主机IP>
  <端口号><xsl:value-of select="./Sport"/></端口号>
  <开始阻断时间><xsl:value-of select="./Start_time"/></开始阻断时间>
  <阻断失效时间><xsl:value-of select="./Expired_time"/></阻断失效时间>
  <阻断原因><xsl:value-of select="./Block_desc"/></阻断原因>
</xsl:template>

</xsl:stylesheet>
