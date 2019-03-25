<?xml version='1.0'?>
<xsl:stylesheet
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/tags/tag">
    <xsl:choose>
      <xsl:when test="@name=preceding-sibling::tag/@name" />
      <xsl:otherwise>
	<xsl:copy>
	  <xsl:apply-templates select="@*|node()"/>
	</xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
