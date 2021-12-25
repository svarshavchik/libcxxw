<?xml version='1.0'?>
<xsl:stylesheet
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="text" />

<xsl:template match="/doxygenindex">
  <xsl:apply-templates select="@*|node()"/>
</xsl:template>

<xsl:template name="munge-one-char">
  <xsl:param name="ch" />

  <xsl:choose>
    <xsl:when test="contains('0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ', $ch)">
      <xsl:value-of select='$ch' />
    </xsl:when>
    <xsl:when test='$ch = " "'>
      <xsl:text>-20</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "!"'>
      <xsl:text>-21</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = &#39;"&#39;'>
      <xsl:text>Q</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "#"'>
      <xsl:text>-23</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "$"'>
      <xsl:text>-24</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "%"'>
      <xsl:text>-25</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "&amp;"'>
      <xsl:text>-26</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "&#39;"'>
      <xsl:text>-27</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "("'>
      <xsl:text>-28</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = ")"'>
      <xsl:text>-29</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "*"'>
      <xsl:text>-2a</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "+"'>
      <xsl:text>-2b</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = ","'>
      <xsl:text>-2c</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "."'>
      <xsl:text>-2e</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "/"'>
      <xsl:text>-2f</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = ":"'>
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = ";"'>
      <xsl:text>-3b</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "&lt;"'>
      <xsl:text>-3c</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "="'>
      <xsl:text>-3d</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "&gt;"'>
      <xsl:text>-3e</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "?"'>
      <xsl:text>-3f</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "@"'>
      <xsl:text>-40</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "["'>
      <xsl:text>-5b</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "\"'>
      <xsl:text>-5c</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "]"'>
      <xsl:text>-5d</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "^"'>
      <xsl:text>-5e</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "_"'>
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "`"'>
      <xsl:text>-60</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "{"'>
      <xsl:text>-7b</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "|"'>
      <xsl:text>-7c</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "}"'>
      <xsl:text>-7d</xsl:text>
    </xsl:when>
    <xsl:when test='$ch = "~"'>
      <xsl:text>-7e</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="munge">
  <xsl:param name="string" />
  <xsl:choose>
    <xsl:when test="$string = ''" />
    <xsl:otherwise>
      <xsl:call-template name='munge-one-char'>
	<xsl:with-param name="ch" select="substring($string, 1, 1)" />
      </xsl:call-template>
      <xsl:call-template name='munge'>
	<xsl:with-param name="string" select="substring($string, 2)" />
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template match="/doxygenindex/compound[@kind='class' or @kind='struct']">
  <xsl:text>&lt;tag name="link-</xsl:text>
  <xsl:call-template name="munge">
    <xsl:with-param name="string" select='name' />
  </xsl:call-template>
  <xsl:text>" value="</xsl:text>
  <xsl:value-of select='$path' /><xsl:text>/</xsl:text>
  <xsl:value-of select="@refid" />
  <xsl:text>.html&quot;/&gt;&#10;</xsl:text>
</xsl:template>

<xsl:template match="/doxygenindex/compound[@kind='singleton']">
  <xsl:text>&lt;tag name="link-</xsl:text>
  <xsl:call-template name="munge">
    <xsl:with-param name="string" select='name' />
  </xsl:call-template>
  <xsl:text>" value="</xsl:text>
  <xsl:value-of select='$path' /><xsl:text>/</xsl:text>
  <xsl:value-of select="@refid" />
  <xsl:text>.html&quot;/&gt;&#10;</xsl:text>
</xsl:template>

<xsl:template match="/doxygenindex/compound[@kind='namespace']">
  <xsl:text>&lt;tag name="namespace-</xsl:text>
  <xsl:call-template name="munge">
    <xsl:with-param name="string" select='name' />
  </xsl:call-template>
  <xsl:text>" value="</xsl:text>
  <xsl:value-of select='$path' /><xsl:text>/</xsl:text>
  <xsl:value-of select="@refid" />
  <xsl:text>.html&quot;/&gt;&#10;</xsl:text>
  <xsl:apply-templates select="@*|node()"/>
</xsl:template>

<xsl:template match="member[@kind != 'enumvalue']">
  <xsl:text>&lt;tag name="link-</xsl:text>
  <xsl:value-of select="@kind" /><xsl:text>-</xsl:text>
  <xsl:call-template name="munge">
    <xsl:with-param name="string" select='../name' />
  </xsl:call-template>
  <xsl:text>-</xsl:text>
  <xsl:call-template name="munge">
    <xsl:with-param name="string" select='name' />
  </xsl:call-template>
  <xsl:text>" value="</xsl:text>
  <xsl:value-of select='$path' /><xsl:text>/</xsl:text>
  <xsl:value-of select="substring(@refid, 1, string-length(@refid)-35)" />
  <xsl:text>.html#</xsl:text>
  <xsl:value-of select="substring(@refid, string-length(@refid)-32)" />
  <xsl:text>"/&gt;&#10;</xsl:text>
</xsl:template>

<xsl:template match="@*|node()">
  <xsl:apply-templates select="@*|node()"/>
</xsl:template>

</xsl:stylesheet>
