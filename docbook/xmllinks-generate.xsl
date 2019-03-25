<?xml version='1.0'?>
<xsl:stylesheet
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="text" />

<xsl:template match="/tags/tag">
  <xsl:text>&lt;!ENTITY </xsl:text>
  <xsl:value-of select="@name" />
  <xsl:text> &quot;</xsl:text>
  <xsl:value-of select="@value" />
  <xsl:text>&quot;&gt;&#10;</xsl:text>
</xsl:template>

<xsl:template match="@*|node()">
  <xsl:apply-templates select="@*|node()"/>
</xsl:template>

</xsl:stylesheet>
