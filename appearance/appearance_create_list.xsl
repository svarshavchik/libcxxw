<?xml version='1.0'?>

<!--

Copyright 2019 Double Precision, Inc.
See COPYING for distribution information.

Stylesheet for creating #includes of headers for appearance objects that are
defined in appearance/*.xml

-->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/root">
    <xsl:element name="informaltable">
      <xsl:element name="thead">
	<xsl:attribute name="cols">3</xsl:attribute>

	<xsl:element name="tr">
	  <xsl:element name="th">
	    <xsl:element name="simpara">
	      <xsl:text>type="</xsl:text>
	      <xsl:element name='replaceable'>
		<xsl:text>type</xsl:text>
	      </xsl:element>
	      <xsl:text>"</xsl:text>
	    </xsl:element>
	  </xsl:element>
	  <xsl:element name="th">
	    <xsl:element name="simpara">
	      Appearance object
	    </xsl:element>
	  </xsl:element>
	  <xsl:element name="th">
	    <xsl:element name="simpara">
	      Built-in themes
	    </xsl:element>
	  </xsl:element>
	</xsl:element>
      </xsl:element>

      <xsl:element name="tbody">
	<xsl:for-each select="appearance">
	  <xsl:call-template name="appearance" />
	</xsl:for-each>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template name="appearance">
    <xsl:element name="tr">
      <xsl:element name="td">
	<simpara>
	  <xsl:element name='tag'>
	    <xsl:attribute name='class'>attribute</xsl:attribute>
	    <xsl:value-of select="name" />
	  </xsl:element>
	</simpara>
      </xsl:element>
      <xsl:element name="td">
	<simpara>
	  <xsl:element name='ulink'>
	    <xsl:attribute name='url'>
	      <xsl:text>&amp;link-x--w--</xsl:text>
	      <xsl:value-of select="translate(name,'_','-')" />
	      <xsl:text>-appearance-properties;</xsl:text>
	    </xsl:attribute>
	    <xsl:element name='classname'>
	      <xsl:text>&amp;ns;::w::const_</xsl:text>
	      <xsl:value-of select="name" />
	      <xsl:text>_appearance</xsl:text>
	    </xsl:element>
	  </xsl:element>
	</simpara>
      </xsl:element>
      <xsl:element name="td">
	<xsl:element name="itemizedlist">
	  <xsl:for-each select="default">
	    <xsl:element name="listitem">
	      <xsl:element name="simpara">
		<xsl:element name="literal">
		  <xsl:value-of select="node()" />
		</xsl:element>
		<xsl:if test="position()=1">
		  <xsl:text> (default)</xsl:text>
		</xsl:if>
	      </xsl:element>
	    </xsl:element>
	  </xsl:for-each>
	</xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>
