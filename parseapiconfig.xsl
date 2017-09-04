<?xml version='1.0'?>

<!--

Copyright 2017 Double Precision, Inc.
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
	    <xsl:if test="position() &gt; 1">
	      <xsl:text>, </xsl:text>
	    </xsl:if>
	    <xsl:text>const </xsl:text>
	    <xsl:value-of select="type" />
	    <xsl:text> &amp;</xsl:text>
	    <xsl:value-of select="name" />
	  </xsl:for-each>)
	    {
                <xsl:if test="object">
	            <xsl:value-of select="object"/>-&gt;</xsl:if>
		<xsl:value-of select="invoke" />(<xsl:for-each select="../forward">
		  <xsl:if test="position() &gt; 1">
		    <xsl:text>, </xsl:text>
		  </xsl:if>
		  <xsl:value-of select="node()" />
		</xsl:for-each>
		<xsl:for-each select="parameter">
		  <xsl:if test="position() = 1">
		    <xsl:if test="../../forward">
		      <xsl:text>, </xsl:text>
		    </xsl:if>
		  </xsl:if>

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

std::function<void, ...parameters> {name}_parser(const xml::doc::base::readlock &lock)
{
    auto name=lock->name();

Loops over each <function>

Throws an exception

-->

  <xsl:template name="make-parser">
std::function&lt;void (<xsl:for-each select="parameter">
<xsl:if test="position() &gt; 1">,
                </xsl:if><xsl:text>
                const </xsl:text><xsl:value-of select="type" />&#32;&amp;</xsl:for-each>)&gt;&#10;      <xsl:value-of select="objectname" />_parser(const xml::doc::base::readlock &amp;lock<xsl:for-each select="other_parameter">
		<xsl:text>,&#10;                      </xsl:text>
		<xsl:value-of select="type" />
		<xsl:text> </xsl:text>
		<xsl:value-of select="name" />
		</xsl:for-each>)
{
    auto name=lock->name();
<xsl:for-each select="function">
  <xsl:call-template name="parse-function" />
</xsl:for-each>

    throw EXCEPTION(gettextmsg(_("&lt;%1%&gt;: unknown element"), name));
}

void <xsl:value-of select="objectname" />_parseconfig(const xml::doc::base::readlock &amp;lock,
    std::vector&lt;std::function&lt;void (<xsl:for-each select="parameter">
    <xsl:if test="position()&gt; 1">,
                          </xsl:if>const <xsl:value-of select="type" />&#32;&amp;</xsl:for-each>)&gt;&gt; &amp;config<xsl:for-each select="other_parameter">
			  <xsl:text>,&#10;                          </xsl:text>
			  <xsl:value-of select="type" />
			  <xsl:text> </xsl:text>
			  <xsl:value-of select="name" />
			  </xsl:for-each>)
{
    config.reserve(config.size()+lock-&gt;get_child_element_count());

    if (lock-&gt;get_first_element_child())
        do
	{
	    config.push_back(<xsl:value-of select="objectname" />_parser(lock<xsl:for-each select="other_parameter">
		<xsl:text>, </xsl:text>
		<xsl:value-of select="name" />
		</xsl:for-each>));
	} while (lock->get_next_element_sibling());
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
