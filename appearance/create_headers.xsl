<?xml version='1.0'?>

<!--

Copyright 2019 Double Precision, Inc.
See COPYING for distribution information.

Stylesheet for creating class member declarations from appearance/*.xml

-->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="text" />

  <xsl:template match="/appearance">
    <xsl:for-each select="/appearance/field">

      <xsl:text>/*!</xsl:text>
      <xsl:value-of select="descr" />
      <xsl:text>*/&#10;&#10;        </xsl:text>
      <xsl:choose>
	<xsl:when test="ref">
	  <xsl:text>const_</xsl:text>
	  <xsl:value-of select="ref" />
	  <xsl:text>_appearance</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:if test="optional">
	    <xsl:text>std::optional&lt;</xsl:text>
	  </xsl:if>
	  <xsl:if test="vector">
	    <xsl:text>std::vector&lt;</xsl:text>
	  </xsl:if>
	  <xsl:value-of select="type" />
	  <xsl:if test="optional">
	    <xsl:text>&gt;</xsl:text>
	  </xsl:if>
	  <xsl:if test="vector">
	    <xsl:text>&gt;</xsl:text>
	  </xsl:if>
	</xsl:otherwise>
      </xsl:choose>

      <xsl:text> </xsl:text>
      <xsl:value-of select="name" />
      <xsl:text>;&#10;</xsl:text>
    </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>
