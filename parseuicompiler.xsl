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
      <xsl:when test="scalar">

	<!-- If <scalar> has an <object>, we declare it -->

	<xsl:if test="object">
	  <xsl:text>        </xsl:text>
	  <xsl:value-of select="object" />
	  <xsl:text> </xsl:text>
	  <xsl:value-of select="scalar" />
	  <xsl:text>;

        if (single_value_exists(lock, "</xsl:text>
	<xsl:value-of select="name" />
	<xsl:text>"))
        {
            auto cloned_lock=lock->clone();

            auto xpath=cloned_lock->get_xpath("</xsl:text>

	    <xsl:value-of select="name" />
	    <xsl:text>");

	    xpath->to_node();

	    auto lock=cloned_lock->clone();

	    xpath=cloned_lock->get_xpath("*");

            size_t n=xpath-&gt;count();
            for (size_t i=1; i &lt;= n; ++i)
            {
                xpath->to_node(i);

                auto name=cloned_lock-&gt;name();&#10;&#10;</xsl:text>
	  <xsl:for-each select="member">

	    <xsl:text>                </xsl:text>
	    <xsl:if test="position() &gt; 1">
	      <xsl:text>else </xsl:text>
	    </xsl:if>

	    <xsl:text>if (name == "</xsl:text>
	    <xsl:value-of select="name" />
	    <xsl:text>")
                {
                    </xsl:text>
		    <xsl:value-of select="../scalar" />
		    <xsl:text>.</xsl:text>
		    <xsl:value-of select="field" />
		    <xsl:text>=</xsl:text>

		    <xsl:call-template name="parse-parameter-value" />

		    <xsl:text>                }&#10;</xsl:text>
	  </xsl:for-each>
	  <xsl:text>                else throw EXCEPTION(gettextmsg(_("&lt;%1%&gt;: unknown element"), name));
            }
        }&#10;</xsl:text>


	</xsl:if>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>        auto </xsl:text>
	<xsl:value-of select="name" />
	<xsl:text>_value=&#10;            </xsl:text>

	<xsl:call-template name="parse-parameter-value" />

      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Parse parameter value

Used by parse-parameter to generates the code for the value of a parameter.

  -->

  <xsl:template name="parse-parameter-value">

    <!--
	If <lookup> is specified, massage the argument
	accordingly.

<lookup>
<parameter>extra_parameter</parameter>
<function>function_name</function>
</lookup>

This adds "{function_name}(" before the value, and
", <extra_parameter>, allowthemerefs, <elementname>)"
after the value, effectively invoking a lookup function, first.
    -->
    <xsl:if test="lookup">
      <xsl:value-of select="lookup/function" />
      <xsl:text>(</xsl:text>
    </xsl:if>

    <xsl:value-of select="type" />
    <xsl:text>(lock, "</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>", "</xsl:text>
    <xsl:value-of select="../name" />
    <xsl:text>"</xsl:text>
    <xsl:text>)</xsl:text>
    <xsl:if test="lookup">
      <xsl:for-each select="lookup/parameter">
	<xsl:text>,&#10;                     </xsl:text>
	<xsl:value-of select="node()" />
      </xsl:for-each>
      <xsl:text>,&#10;                     allowthemerefs, &#34;</xsl:text>
      <xsl:value-of select="../name" />
      <xsl:text>&#34;)</xsl:text>
    </xsl:if>
    <xsl:text>;&#10;</xsl:text>
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
      </xsl:choose></xsl:for-each>
      <xsl:text>)
    {&#10;</xsl:text>
    <xsl:for-each select="parameter">
  <xsl:call-template name="parse-parameter"/>
</xsl:for-each><xsl:text>&#10;        return [=</xsl:text>
<xsl:if test="new_element">
  <xsl:text>, id=lock-&gt;get_any_attribute("id")</xsl:text>
</xsl:if>
<xsl:text>]
            (</xsl:text>
	    <xsl:for-each select="../parameter">
	      <xsl:call-template name="declare-parameter" />
	    </xsl:for-each><xsl:text>)
	    {
                </xsl:text>
		<xsl:if test="new_element">
		  <xsl:text>auto new_element=</xsl:text>
		</xsl:if>
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
		  </xsl:for-each><xsl:text>);&#10;</xsl:text>
		  <xsl:if test="new_element">
		    <xsl:text>&#10;                if (!id.empty())
                    elements.new_elements.emplace(id, new_element);&#10;</xsl:text>
		  </xsl:if>

		  <xsl:text>            };
    }&#10;</xsl:text></xsl:template>

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
