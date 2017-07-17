<?xml version='1.0'?>

<!--

Copyright 2015 Double Precision, Inc.
See COPYING for distribution information.

Stylesheet for transforming the XML in gridlayoutapi.xml

-->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="text" />

  <xsl:template match="constant">

    <xsl:text>&#10;static const struct {&#10;    </xsl:text>
    <xsl:value-of select="type" />
    <xsl:text> value;&#10;    const char *name;&#10;} </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table[] = {</xsl:text>

    <xsl:for-each select="value">
      <xsl:if test="position() &gt; 1">
	<xsl:text>,</xsl:text>
      </xsl:if>

      <xsl:text>&#10;    {</xsl:text>
      <xsl:value-of select="raw" />
      <xsl:text>, "</xsl:text>
      <xsl:value-of select="name" />
      <xsl:text>"}</xsl:text>
    </xsl:for-each>

    <xsl:text>&#10;};&#10;&#10;static inline </xsl:text>
    <xsl:value-of select="type" />
    <xsl:text> </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_from_string(const std::string_view &amp;str)&#10;{&#10;    size_t n=sizeof(</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table)/sizeof(</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table[0]);&#10;    for (size_t i=0; i&lt;n;++i)&#10;        if (chrcasecmp::str_equal_to()(str, </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table[i].name))&#10;            return </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table[i].value;&#10;&#10;    throw EXCEPTION(gettextmsg(_("Unknown </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text> value: %1%"), str));&#10;}&#10;&#10;static inline const char *</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_to_string(</xsl:text>
    <xsl:value-of select="type" />
    <xsl:text> value)&#10;{&#10;    size_t n=sizeof(</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table)/sizeof(</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table[0]);&#10;    for (size_t i=0; i&lt;n;++i)&#10;        if (value == </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table[i].value)&#10;            return </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_lookup_table[i].name;&#10;&#10;    throw EXCEPTION(gettextmsg(_("Unknown </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text> value: %1%"), (int)value));&#10;}&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="*|/">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="text()" />

</xsl:stylesheet>
