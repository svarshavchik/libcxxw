<?xml version='1.0'?>

<!--

Copyright 2017-2019 Double Precision, Inc.
See COPYING for distribution information.

Stylesheet for transforming the XML in gridlayoutapi.xml

-->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="text" />

  <!-- <parameter>

Generates:

    auto {name}_value={type}(lock, "<parameter>")


  -->

  <xsl:template name="parse-parameter">

    <!-- Except when there's a <scalar> -->

    <xsl:choose>
      <xsl:when test="scalar" />

      <xsl:otherwise>
	<xsl:text>        auto </xsl:text>
	<xsl:value-of select="name" />
	<xsl:text>_value=&#10;            </xsl:text>
	<xsl:value-of select="type" />
	<xsl:text>(lock, "</xsl:text>
	<xsl:value-of select="name" />
	<xsl:text>", "</xsl:text>
	<xsl:value-of select="../name" />
	<xsl:text>"</xsl:text>
	<xsl:text>);&#10;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!--
      Declare parameters.

Called from a for-each loop over <parameter>s. Generate parameter list.

  -->

  <xsl:template name="declare-parameter">
    <xsl:if test="position() &gt; 1">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:choose>
      <xsl:when test="@mutable='1'">
	<xsl:text> </xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>const </xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:value-of select="type" />
    <xsl:text> &amp;</xsl:text>
    <xsl:value-of select="name" />
  </xsl:template>


  <!-- <function> generates:

    if (name == "{name}"   { conditions } )
    {
         <loop over parameters, call parse-parameter>

         return [=]
             ( {forwarded parameters}
	         {object}->{invoke}( {forwarded_parameters}, {looped parameters}

    }
}
  -->

  <xsl:template name="parse-function">
    if (name == "<xsl:value-of select="name"/>"<xsl:for-each select="condition">

    <!-- loop over the conditions -->

    <xsl:text>&#10;        &amp;&amp; </xsl:text>
    <xsl:choose>
      <xsl:when test="exists">
	<xsl:text>single_value_exists(lock, "</xsl:text>
	<xsl:value-of select="exists" />
	<xsl:text>")</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>lowercase_single_value(lock, "</xsl:text>
	<xsl:value-of select="name" />
	<xsl:text>", "</xsl:text>
	<xsl:value-of select="../name" />
	<xsl:text>") == "</xsl:text>
	<xsl:value-of select="value" />
	<xsl:text>"</xsl:text>
      </xsl:otherwise>
      </xsl:choose></xsl:for-each>)
    {
<xsl:for-each select="parameter">
  <xsl:call-template name="parse-parameter"/>
</xsl:for-each>
        return [=]
            (<xsl:for-each select="../parameter">
	    <xsl:call-template name="declare-parameter" />
	  </xsl:for-each>)
	    {
                <xsl:if test="object">
	            <xsl:value-of select="object"/>-&gt;</xsl:if>
		<xsl:value-of select="invoke" />(<xsl:for-each select="parameter">
		  <xsl:if test="position() &gt; 1">
		    <xsl:text>, </xsl:text>
		  </xsl:if>

		  <!--

Maybe we should use a <scalar>?

		  -->

		  <xsl:choose>
		    <xsl:when test="scalar">
		      <xsl:value-of select="scalar" />
		    </xsl:when>
		    <xsl:otherwise>
		      <xsl:value-of select="name" /><xsl:text>_value</xsl:text>
		    </xsl:otherwise>
		  </xsl:choose>
		</xsl:for-each>);
            };
    }
</xsl:template>

<!-- <parser> generates:

functionref<void, ...parameters> {name}_parser()
{
    auto name=lock->name();

Loops over each <function>

Throws an exception

-->

  <xsl:template name="make-parser">
functionref&lt;void (<xsl:for-each select="parameter">
<xsl:call-template name="declare-parameter" /></xsl:for-each>)&gt;&#10;      uicompiler::<xsl:value-of select="name" />_parser(const theme_parser_lock &amp;lock)
{
    auto name=lock->name();
<xsl:for-each select="function">
  <xsl:call-template name="parse-function" />
</xsl:for-each>

    throw EXCEPTION(gettextmsg(_("&lt;%1%&gt;: unknown element"), name));
}

vector&lt;functionref&lt;void (<xsl:for-each select="parameter">
<xsl:call-template name="declare-parameter" /></xsl:for-each>)&gt;&gt;
uicompiler::<xsl:value-of select="name" />_parseconfig(const theme_parser_lock &amp;lock)
{
    auto config=vector&lt;functionref&lt;void (<xsl:for-each select="parameter">
<xsl:call-template name="declare-parameter" /></xsl:for-each>)&gt;&gt;
        ::create();
    config->reserve(lock-&gt;get_child_element_count());

    if (lock-&gt;get_first_element_child())
        do
	{
	    config->push_back(<xsl:value-of select="name" />_parser(lock));
	} while (lock->get_next_element_sibling());
    return config;
}

</xsl:template>


  <!-- Top level element -->

  <xsl:template match="/api">
    <xsl:for-each select="parser">
      <xsl:call-template name="make-parser" />
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="*|/">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="text()" />

</xsl:stylesheet>
