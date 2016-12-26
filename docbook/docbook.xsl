<?xml version='1.0'?>
<xsl:stylesheet  
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template name="user.head.content">
    <xsl:element name="script" namespace="http://www.w3.org/1999/xhtml">
      <xsl:attribute name="type">text/javascript</xsl:attribute>
      <xsl:attribute name="src">
	<xsl:text>/frame.js</xsl:text>
      </xsl:attribute>
      <xsl:text> </xsl:text>
    </xsl:element>

    <xsl:element name="link" namespace="http://www.w3.org/1999/xhtml">
      <xsl:attribute name="rel">icon</xsl:attribute>
      <xsl:attribute name="href">
	<xsl:text>/icon.gif</xsl:text>
      </xsl:attribute>
      <xsl:attribute name="type">image/gif</xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template name="body.attributes">
    <xsl:choose>
      <xsl:when test='@id'>
	<xsl:attribute name="id"><xsl:text>body</xsl:text><xsl:value-of select='@id' /></xsl:attribute>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:include href="http://docbook.sourceforge.net/release/xsl/current/xhtml-1_1/chunk.xsl" />
</xsl:stylesheet>
