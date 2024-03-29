<?xml version='1.0'?>

<!--

Copyright 2017-2020 Double Precision, Inc.
See COPYING for distribution information.

Stylesheet for transforming the XML in uicompiler.xml

-->

<xsl:stylesheet
    xmlns:exsl="http://exslt.org/common"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    extension-element-prefixes="exsl"
>

  <xsl:output method="text" />

  <!-- <parameter>

Create an initialization for each parameter's value in the tuple
that gets returned from get_<mumble>()

  -->

  <xsl:template name="parse-parameter-tuple-value">

    <xsl:choose>
      <!-- An instance of a class, default-constructed -->
      <xsl:when test="object">
	<xsl:value-of select="object" />
	<xsl:text>{</xsl:text>
	<xsl:value-of select="default_constructor_params" />
	<xsl:text>}</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="parse-parameter-value" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <!--

After the tuple declaration, initialize the corresponding value.

Our caller already used a structured binding to bind <name>_value to our
tuple value.
  -->

  <xsl:template name="parse-parameter-tuple-initialize">
    <xsl:param name="parameter" />
    <xsl:choose>

      <!--
	  "object" introduces a helper object that gets passed as an
	  additional parameter to a factory method call, such as
	  input_field_config. For example. We generate code for initializing
	  individual fields of this object.
      -->

      <xsl:when test="object">
	<xsl:text>&#10;        if (single_value_exists(lock, "</xsl:text>
	<xsl:value-of select="member_name" />
	<xsl:text>"))&#10;        {&#10;</xsl:text>
	<xsl:choose>

	  <!--
	      An <object> that's marked with <initialize_self /> indicates
	      an additional parameter that's not a class with individual
	      members, but a discrete datatype.
	      The std::tuple return value default-constructed it, and
	      if specified, we generate an assignment operator to set its
	      new value.
	  -->
	  <xsl:when test="count(initialize_self) &gt; 0">

	    <xsl:text>            </xsl:text>
	    <xsl:value-of select="$parameter" />
	    <xsl:text>=</xsl:text>
	    <xsl:call-template name="parse-parameter-value" />
	    <xsl:text>;&#10;</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>            auto cloned_lock=lock->clone();

            auto xpath=cloned_lock->get_xpath("</xsl:text>

	    <xsl:value-of select="member_name" />
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
		    <xsl:choose>
		      <!--
			  <method_call /> indicates that instead of assigning
			  a value, the parameter's method gets called.

		      -->
		      <xsl:when test='count(method_call) &gt; 0'>

			<!--
			    "field" gives the name of the method to call,
			    and <method_call> specifies any additional
			    parameters
			-->

			<xsl:choose>
			  <xsl:when test="count(field) &gt; 0">
			    <xsl:value-of select="$parameter" />
			    <xsl:text>.</xsl:text>
			    <xsl:value-of select="field" />

			    <xsl:text>()</xsl:text>
			  </xsl:when>

			  <xsl:otherwise>

			    <!--
				If there's no <field>,
				we're calling something else.
			    -->

			    <xsl:value-of select="method_call/name" />
			    <xsl:text>(</xsl:text>

			    <!-- Pass $parameter as the first parameter. -->
			    <xsl:value-of select="$parameter" />

			    <!-- And any additional parameters -->

			    <xsl:for-each select="method_call/parameter">
			      <xsl:text>,&#10;                            </xsl:text>
			      <xsl:value-of select="node()" />
			    </xsl:for-each>
			    <xsl:text>)</xsl:text>
			  </xsl:otherwise>
			</xsl:choose>
		      </xsl:when>
		      <xsl:otherwise>
			<xsl:value-of select="$parameter" />
			<xsl:text>.</xsl:text>
			<xsl:value-of select="field" />

			<xsl:text>=</xsl:text>

			<xsl:call-template name="parse-parameter-value">
			  <xsl:with-param name="prepend-parameter">
			    <xsl:if test="count(lookup/modify) &gt; 0">
			      <xsl:value-of select="$parameter" />
			      <xsl:text>.</xsl:text>
			      <xsl:value-of select="field" />
			    </xsl:if>
			  </xsl:with-param>
			</xsl:call-template>
		      </xsl:otherwise>
		    </xsl:choose>
		    <xsl:text>;&#10;                }&#10;</xsl:text>
	  </xsl:for-each>
	  <xsl:text>                </xsl:text>
	  <xsl:if test="count(member) &gt; 0">
	    <xsl:text>else </xsl:text>
	  </xsl:if>
	  <xsl:text>throw EXCEPTION(gettextmsg(_("&lt;%1%&gt;: unknown element"), name));
            }&#10;</xsl:text>
          </xsl:otherwise>
	</xsl:choose>
	<xsl:text>        }&#10;</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <!-- Parse parameter value

Used by parse-parameter to generates the code for the value of a parameter.

  -->

  <xsl:template name="parse-parameter-value">

    <xsl:param name="prepend-parameter" />

    <!--
	If <lookup> is specified, massage the argument
	accordingly.

<lookup>
<parameter>extra_parameter</parameter>
<function>function_name</function>
</lookup>

This adds "{function_name}(" before the value, and
", <extra_parameter>"
after the value, effectively invoking a lookup function, first.

If there's a <default_params>, we also append
", allowthemerefs, <elementname>" to the parameters.

    -->
    <xsl:if test="lookup">
      <xsl:value-of select="lookup/function" />
      <xsl:text>(</xsl:text>

      <xsl:if test="$prepend-parameter != ''">
	<xsl:value-of select="$prepend-parameter" />
	<xsl:text>, </xsl:text>
      </xsl:if>

      <xsl:if test="lookup/prepend-parameter">
	<xsl:value-of select="lookup/prepend-parameter" />
	<xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:if>

    <xsl:value-of select="type" />
    <xsl:text>(lock, "</xsl:text>
    <xsl:choose>
      <xsl:when test="xpath">
	<xsl:value-of select="xpath" />
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="name" />
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>", "</xsl:text>
    <xsl:value-of select="../name" />
    <xsl:text>"</xsl:text>
    <xsl:text>)</xsl:text>
    <xsl:if test="lookup">
      <xsl:for-each select="lookup/parameter">
	<xsl:text>,&#10;                     </xsl:text>
	<xsl:value-of select="node()" />
      </xsl:for-each>
      <xsl:if test="count(lookup/default_params) &gt; 0">
	<xsl:text>,&#10;                     compiler.allowthemerefs, &#34;</xsl:text>
	<xsl:value-of select="../name" />
	<xsl:text>&#34;</xsl:text>
      </xsl:if>
      <xsl:text>)</xsl:text>
    </xsl:if>
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


  <xsl:template name="function-parameters-parser-name">
    <xsl:text>get_</xsl:text>
    <xsl:choose>
      <xsl:when test="parameter_parser_name">
	<xsl:value-of select="parameter_parser_name" />
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="name" />
	<xsl:text>_</xsl:text>
	<xsl:value-of select="position()" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="parse-function-parameters">
    <xsl:param name="parameter_parser_name" />

    <xsl:text>static auto </xsl:text>
    <xsl:value-of select="$parameter_parser_name" />
    <xsl:text>(uicompiler &amp;compiler, const ui::parser_lock &amp;orig_lock)&#10;{&#10;</xsl:text>
    <xsl:if test="count(parameter[count(scalar)=0]) &gt; 0">
      <xsl:text>    auto &amp;lock=orig_lock;&#10;</xsl:text>
    </xsl:if>

    <xsl:text>    std::tuple return_value{</xsl:text>

    <xsl:for-each select="parameter[count(scalar)=0]">
      <xsl:if test="position() &gt; 1">
	<xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:text>&#10;</xsl:text>

      <xsl:call-template name="parse-parameter-tuple-value" />
    </xsl:for-each>

    <xsl:text>};&#10;&#10;</xsl:text>

    <xsl:for-each select="parameter[count(scalar)=0]">
      <xsl:call-template name="parse-parameter-tuple-initialize">
	<xsl:with-param name="parameter">
	  <xsl:text>std::get&lt;</xsl:text>
	  <xsl:value-of select="position()-1" />
	  <xsl:text>&gt;(return_value)</xsl:text>
	</xsl:with-param>
      </xsl:call-template>
    </xsl:for-each>

    <xsl:text>        return return_value;&#10;}&#10;</xsl:text>
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
    <xsl:param name="parameter_parser_name" />
    <xsl:text>&#10;    if (name == "</xsl:text><xsl:value-of select="name"/><xsl:text>"</xsl:text>
    <xsl:for-each select="condition">

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
    {&#10;&#10;        return [=, params=</xsl:text>
    <xsl:value-of select="$parameter_parser_name" />
    <xsl:text>(*this, lock)</xsl:text>
<xsl:if test="new_element">
  <xsl:text>,&#10;                   id=lock-&gt;get_any_attribute("id"),&#10;                   optional_tooltip=compiler_functions::get_optional_tooltip(*this, lock),&#10;                   optional_elements=compiler_functions::get_optional_elements(*this, lock),&#10;                   optional_contextpopup=compiler_functions::get_optional_contextpopup(*this, lock)</xsl:text></xsl:if>
<xsl:if test="new_container">
  <xsl:text>,&#10;                   optional_tooltip=compiler_functions::get_optional_tooltip(*this, lock),&#10;optional_elements=compiler_functions::get_optional_elements(*this, lock),&#10;                   optional_contextpopup=compiler_functions::get_optional_contextpopup(*this, lock)</xsl:text></xsl:if>
<xsl:text>]
            (</xsl:text>
	    <xsl:for-each select="../parameter">
	      <xsl:call-template name="declare-parameter" />
	    </xsl:for-each><xsl:text>)
	    {&#10;</xsl:text>
		<xsl:for-each select="parameter[count(scalar) = 0]">
		  <xsl:text>                const auto &amp;</xsl:text>
		  <xsl:value-of select="name" />
		  <xsl:text>_value=std::get&lt;</xsl:text>
		  <xsl:value-of select="position()-1" />
		  <xsl:text>&gt;(params);&#10;</xsl:text>
		</xsl:for-each>

		<xsl:text>                </xsl:text>
		<xsl:if test="new_element or new_container">
		  <xsl:text>element new_element=</xsl:text>
		</xsl:if>
		<xsl:value-of select="before_invocation" />
		<xsl:if test="object">
	            <xsl:value-of select="object"/>-&gt;</xsl:if>
		<xsl:value-of select="invoke" />(<xsl:for-each select="parameter">
		  <xsl:if test="position() &gt; 1">
		    <xsl:text>, </xsl:text>
		  </xsl:if>

		  <xsl:value-of select="before-passing-parameter" />
		  <!--

Maybe we should use a <scalar>?

		  -->

		  <xsl:choose>
		    <xsl:when test="scalar">
		      <xsl:value-of select="scalar" />
		    </xsl:when>
		    <xsl:when test="factory_wrapper">


		      <!--

The parameter is created by factory_parseconfig, a functionref that takes
a factory and a uilements parameter, and forwards the uielements to a compiled
list of element generators.
method that takes a creator as a parameter.

The actual parameter we generate is

[&]
(const auto &<wrapper>)    // The factory parameter
{
    [values](forwarded parameter);
}

		      -->
		      <xsl:text>&#10;                    [&amp;]&#10;                    (const auto &amp;</xsl:text>
		      <xsl:value-of select="factory_wrapper" />
		      <xsl:text>)
                    {
                        </xsl:text>
		        <xsl:value-of select="name" /><xsl:text>_value(</xsl:text>
			<xsl:for-each select="../../parameter">
			  <xsl:if test="position() &gt; 1"><xsl:text>, </xsl:text></xsl:if>
			  <xsl:value-of select="name" />
			  </xsl:for-each><xsl:text>);
                    }</xsl:text>
		    </xsl:when>
		    <xsl:otherwise>
		      <xsl:value-of select="name" /><xsl:text>_value</xsl:text>
		    </xsl:otherwise>
		  </xsl:choose>
		  <xsl:value-of select="after-passing-parameter" />
		  </xsl:for-each><xsl:text>)</xsl:text>
		  <xsl:value-of select="after_invocation" />
		  <xsl:text>;&#10;</xsl:text>
		  <xsl:if test="new_element">
		    <xsl:text>&#10;                elements.new_elements.insert_or_assign(id, new_element);&#10;</xsl:text>
		    </xsl:if>
		    <xsl:if test="new_element or new_container">
		    <xsl:text>                if (optional_elements)&#10;                    elements.generate_factory(*optional_elements);&#10;                if (optional_contextpopup)&#10;                    compiler_functions::install_contextpopup(elements, new_element, *optional_contextpopup);&#10;                compiler_functions::install_tooltip(new_element, elements, optional_tooltip);&#10;</xsl:text>
		  </xsl:if>

		  <xsl:text>            };
    }&#10;</xsl:text></xsl:template>

<!-- <parser> generates:

functionref<void, ...parameters> {name}_parser()
{
    auto name=lock->name();

Loops over each <function>

Throws an exception
}

And

vector<functionref<void, ...parameters>> {name}_parseconfig()
{

    // Loop over each child element

    // Call _parser()
}

If <common> exists, _parseconfig() doe not get generated.

This is used for generating code that's shared by all factory objects,
to generate a single element in the factory.

-->

  <xsl:template name="make-parser">
    <exsl:document
	href="uicompiler.inc.H/{name}_parser.H"
	method="text">
    <xsl:text>&#10;functionref&lt;void (</xsl:text><xsl:for-each select="parameter">
<xsl:call-template name="declare-parameter" /></xsl:for-each>)&gt;&#10;      uicompiler::<xsl:value-of select="name" />_parser(const ui::parser_lock &amp;lock)
{
    auto name=lock->name();
<exsl:document
    href="{name}_parse_parameters.H"
    method="text">
  <xsl:for-each select="function">
    <xsl:call-template name="parse-function-parameters">
      <xsl:with-param name="parameter_parser_name">
	<xsl:call-template name="function-parameters-parser-name" />
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text>&#10;</xsl:text>
  </xsl:for-each>
</exsl:document>
<xsl:for-each select="function">
  <xsl:call-template name="parse-function">
    <xsl:with-param name="parameter_parser_name">
      <xsl:call-template name="function-parameters-parser-name" />
    </xsl:with-param>
  </xsl:call-template>
</xsl:for-each>
<xsl:choose>
  <xsl:when test="use_common">
    return [common=<xsl:value-of select="use_common" />_parser(lock)]
        (<xsl:for-each select="parameter">
	      <xsl:call-template name="declare-parameter" />
	    </xsl:for-each>)
        {
            return common(<xsl:for-each select="parameter">
                <xsl:if test="position() &gt; 1">, </xsl:if>
	      <xsl:value-of select="name" />
	    </xsl:for-each>);
        };
}
</xsl:when>
  <xsl:otherwise>
    throw EXCEPTION(gettextmsg(_("&lt;%1%&gt;: unknown element"), name));
}
</xsl:otherwise>
</xsl:choose>
vector&lt;functionref&lt;void (<xsl:for-each select="parameter">
<xsl:call-template name="declare-parameter" /></xsl:for-each>)&gt;&gt;
uicompiler::<xsl:value-of select="name" />_parseconfig(const ui::parser_lock &amp;lock)
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
</exsl:document>
</xsl:template>


  <!-- Top level element -->

  <xsl:template match="/api">
    <xsl:for-each select="parser">
      <xsl:call-template name="make-parser" />
    </xsl:for-each>
    <exsl:document
	href="uicompiler.inc.H/uicompiler_configparser.H"
	method="text">
      <xsl:for-each select="parser">
	<xsl:if test="config">
	  <xsl:text>static const_vector&lt;</xsl:text>
	  <xsl:value-of select="config" />
	  <xsl:text>manager_generator&gt;
create_new</xsl:text><xsl:value-of select="name" /><xsl:text>manager_vector(uicompiler &amp;compiler,
				   const ui::parser_lock &amp;orig_lock,
                                   const std::string &amp;configname)
{
	auto lock=orig_lock->clone();

	auto xpath=lock->get_xpath(configname);

	if (xpath->count() == 0) // None, return an empty vector.
		return const_vector&lt;</xsl:text>
<xsl:value-of select="config" /><xsl:text>manager_generator&gt;::create();

	xpath->to_node();

	return compiler.</xsl:text>
<xsl:value-of select="config" /><xsl:text>_parseconfig(lock);
}
</xsl:text>


	</xsl:if>
      </xsl:for-each>
    </exsl:document>
    <exsl:document
	href="uielements.inc.C"
	method="text">
      <xsl:for-each select="parser[@type='layoutmanager']">
	<xsl:if test="category">
	  <xsl:text>void </xsl:text>
	  <xsl:value-of select="category" />
	  <xsl:text>layoutmanagerObj::generate(const std::string_view &amp;name,
				    const const_uigenerators &amp;generators,
				    uielements &amp;elements)
{
	generate_sentry sentry{elements,
			       this,
			       generators-></xsl:text>
			       <xsl:value-of select="category" />
	  <xsl:text>layoutmanager_generators,
			       name};
}&#10;</xsl:text>
	</xsl:if>
      </xsl:for-each>

      <xsl:for-each select="parser[@type='factory']">
	<xsl:if test="category">
	  <xsl:text>void </xsl:text>
	  <xsl:call-template name="factory-category" />
	  <xsl:text>factoryObj::generate(const std::string_view &amp;name,
			      const const_uigenerators &amp;generators,
			      uielements &amp;elements)
{
	generate_sentry sentry{elements,
			       this,
			       generators-></xsl:text>
			       <xsl:call-template name="factory-category" />
	  <xsl:text>factory_generators,
			       name};
}&#10;</xsl:text>
	</xsl:if>
      </xsl:for-each>
    </exsl:document>

    <exsl:document
	href="uicompiler_layoutparsers.inc.H"
	method="text">
      <xsl:for-each select="parser[@type='layoutmanager']">
	<xsl:if test="category">
	  <xsl:text>
	//! Generate a single </xsl:text>
	<xsl:value-of select="category" /><xsl:text> layout manager instruction.
	</xsl:text>
	<xsl:value-of select="category" /><xsl:text>layoutmanager_generator
	</xsl:text>
	<xsl:value-of select="category" /><xsl:text>layout_parser(const ui::parser_lock &amp;lock);

        //! Parse the &lt;config&gt; for the </xsl:text>
	<xsl:value-of select="category" />
	<xsl:text> layout manager

	vector&lt;</xsl:text>
	<xsl:value-of select="category" />
	<xsl:text>layoutmanager_generator
		&gt; </xsl:text>
		<xsl:value-of select="category" />
		<xsl:text>layout_parseconfig(const ui::parser_lock &amp;lock);&#10;&#10;</xsl:text>
	</xsl:if>
      </xsl:for-each>
    </exsl:document>

    <exsl:document
	href="uicompiler_factoryparsers.inc.H"
	method="text">
      <xsl:for-each select="parser[@type='factory']">
	<xsl:if test="category">
	  <xsl:text>
	//! Generate a single </xsl:text>
	<xsl:value-of select="category" /><xsl:text> factory instruction.
	</xsl:text>
	<xsl:if test="category != 'factory'">
	  <xsl:value-of select="category" />
	</xsl:if>
	<xsl:text>factory_generator
	</xsl:text>
	<xsl:if test="category != 'factory'">
	  <xsl:value-of select="category" />
	</xsl:if>
	<xsl:text>factory_parser(const ui::parser_lock &amp;lock);&#10;</xsl:text>
	</xsl:if>
      </xsl:for-each>
    </exsl:document>
  </xsl:template>

  <xsl:template name='factory-category'>
    <xsl:choose>
      <xsl:when test="category='factory'">
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="category" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="*|/">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="text()" />

</xsl:stylesheet>
