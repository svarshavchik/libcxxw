<?xml version='1.0'?>

<!--

Copyright 2020 Double Precision, Inc.
See COPYING for distribution information.

Stylesheet for transforming the XML in uicompiler.xml

-->

<xsl:stylesheet
    xmlns:exsl="http://exslt.org/common"
    xmlns:str="http://exslt.org/strings"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    extension-element-prefixes="exsl str"
>

  <!--
    <parameter>
      <type>gridlayoutmanager</type>
      <name>layout</name>
    </parameter>

    This returns "gridlayout"
  -->

  <xsl:template name="layoutmanager-type">
    <xsl:for-each select="parameter[name='layout']">
      <xsl:value-of select="str:replace(type,'manager','')" />
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="make-code">
    <exsl:document
	href="get_layoutmanagers.inc.C"
	method="text">

      <xsl:for-each select="parser[@type = 'layoutmanager']">
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>manager containerObj::</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>()&#10;{&#10;&#9;return get_layoutmanager();&#10;}&#10;&#10;const_</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>manager containerObj::</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>() const&#10;{&#10;&#9;return get_layoutmanager();&#10;}&#10;&#10;</xsl:text>
      </xsl:for-each>
    </exsl:document>

    <exsl:document
	href="get_layoutmanagers.inc.H"
	method="text">

      <xsl:for-each select="parser[@type = 'layoutmanager']">
	<xsl:text>#include "x/w/</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>manager.H"&#10;</xsl:text>
      </xsl:for-each>
    </exsl:document>

    <exsl:document
	href="includes/x/w/get_layoutmanagers.inc.H"
	method="text">

      <xsl:text>#ifdef x_w_containerobj_h&#10;&#10;</xsl:text>
      <xsl:for-each select="parser[@type = 'layoutmanager']">
	<xsl:text>//! Return this container's \ref </xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>manager "INSERT_LIBX_NAMESPACE::w::</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>manager".&#10;&#10;//! An exception gets thrown if this container has a different layout manager.&#10;&#10;</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>manager </xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>();&#10;&#10;//! \overload&#10;&#10;const_</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>manager </xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>() const;&#10;&#10;</xsl:text>
      </xsl:for-each>
      <xsl:text>&#10;#endif&#10;</xsl:text>
    </exsl:document>

    <exsl:document
	href="includes/x/w/get_layoutmanagersfwd.inc.H"
	method="text">

      <xsl:for-each select="parser[@type = 'layoutmanager']">
	<xsl:text>#include &lt;x/w/</xsl:text>
	<xsl:call-template name="layoutmanager-type" />
	<xsl:text>managerfwd.H&gt;&#10;</xsl:text>
      </xsl:for-each>
    </exsl:document>

  </xsl:template>

  <!-- Top level element -->

  <xsl:template match="/api">
    <xsl:call-template name="make-code" />
  </xsl:template>
</xsl:stylesheet>
