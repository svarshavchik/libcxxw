<?xml version='1.0'?>
<xsl:stylesheet
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="text" />

<xsl:template match="/doxygenindex/compound[@kind='class']">
  <xsl:text>&lt;!ENTITY link-</xsl:text>
  <xsl:value-of select="translate(name,': &amp;&lt;&gt;,_[]()*~','-----------ZD')" />
  <xsl:text> &quot;ref/</xsl:text>
  <xsl:value-of select="@refid" />
  <xsl:text>.html&quot;&gt;&#10;</xsl:text>
</xsl:template>

<xsl:template match="/doxygenindex/compound[@kind='singleton']">
  <xsl:text>&lt;!ENTITY link-</xsl:text>
  <xsl:value-of select="translate(name,': &amp;&lt;&gt;,_[]()*~','-----------ZD')" />
  <xsl:text> &quot;ref/</xsl:text>
  <xsl:value-of select="@refid" />
  <xsl:text>.html&quot;&gt;&#10;</xsl:text>
</xsl:template>

<xsl:template match="/doxygenindex/compound[@kind='namespace']">
  <xsl:text>&lt;!ENTITY namespace-</xsl:text>
  <xsl:value-of select="translate(name,': &amp;&lt;&gt;,_[]()*~','-----------ZD')" />
  <xsl:text> &quot;ref/</xsl:text>
  <xsl:value-of select="@refid" />
  <xsl:text>.html&quot;&gt;&#10;</xsl:text>
  <xsl:apply-templates select="@*|node()"/>
</xsl:template>

<xsl:template match="member">
  <xsl:text>&lt;!ENTITY link-</xsl:text>
  <xsl:value-of select="@kind" /><xsl:text>-</xsl:text>
  <xsl:value-of select="translate(../name,': &amp;&lt;&gt;,_[]()*~!=+','-----------ZDNEP')" />
  <xsl:text>-</xsl:text>
  <xsl:value-of select="translate(name,': &amp;&lt;&gt;,_[]()*~!=+','-----------ZDNEP')" />
  <xsl:text> &quot;ref/</xsl:text>
  <xsl:value-of select="substring(@refid, 1, string-length(@refid)-35)" />
  <xsl:text>.html#</xsl:text>
  <xsl:value-of select="substring(@refid, string-length(@refid)-32)" />
  <xsl:text>"&gt;&#10;</xsl:text>
</xsl:template>

<xsl:template match="@*|node()">
  <xsl:apply-templates select="@*|node()"/>
</xsl:template>

</xsl:stylesheet>
