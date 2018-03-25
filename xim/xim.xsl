<?xml version='1.0'?>

<!--

Copyright 2015 Double Precision, Inc.
See COPYING for distribution information.

-->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="text" />

  <xsl:param name="mode" select="'all'" />

  <xsl:template name="badmessagename">
    <xsl:choose>
      <xsl:when test="../name">
	<xsl:value-of select="../name" />
      </xsl:when>
      <xsl:when test="../../name">
	<xsl:value-of select="../../name" />
      </xsl:when>
      <xsl:otherwise>
	<xsl:message terminate="yes">Cannot derive element name for badmessage()</xsl:message>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!--
      Generate function prototype. Function already has a parameter, so
      a comma goes before every parameter added here.
  -->

  <xsl:template name="extra-prototypes">
    <xsl:for-each select="parameter">

      <xsl:text>, </xsl:text>

      <xsl:if test="count(class) &gt; 0">
	<xsl:text>const </xsl:text>
      </xsl:if>
      <xsl:value-of select="type" />
      <xsl:text> </xsl:text>
      <xsl:if test="count(class) &gt; 0">
	<xsl:text>&amp;</xsl:text>
      </xsl:if>
      <xsl:value-of select="name" />
    </xsl:for-each>
  </xsl:template>

  <!--
      Generate function prototype. Function has no parameters, so a comma
      between parameters only.
  -->

  <xsl:template name="only-prototypes">
    <xsl:for-each select="parameter">

      <xsl:if test="position() &gt; 1">
	<xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:if test="count(class) &gt; 0">
	<xsl:text>const </xsl:text>
      </xsl:if>
      <xsl:value-of select="type" />
      <xsl:text> </xsl:text>
      <xsl:if test="count(class) &gt; 0">
	<xsl:text>&amp;</xsl:text>
      </xsl:if>
      <xsl:value-of select="name" />
    </xsl:for-each>
  </xsl:template>

  <!--
      Declare <datatype>_generate() <datatype>_sizeof() functions,
      to calculate the size of a datatype, and to actually generate it. Declare
      <datatype>_received()
  -->

  <xsl:template match="datatype" mode="request-prototypes">
    <xsl:text>bool </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_received(ONLY IN_THREAD, const uint8_t * &amp;data, size_t &amp;data_size</xsl:text>
    <xsl:for-each select="parameter">
      <xsl:text>, </xsl:text>
      <xsl:value-of select="type" />
      <xsl:text> &amp;</xsl:text>
      <xsl:value-of select="name" />
    </xsl:for-each>
    <xsl:text>);&#10;void </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_generate(uint8_t * &amp;p</xsl:text>
    <xsl:call-template name="extra-prototypes" />
    <xsl:text>);&#10;</xsl:text>
    <xsl:text>size_t </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_sizeof(</xsl:text>
    <xsl:call-template name="only-prototypes" />
    <xsl:text>);&#10;&#10;</xsl:text>
  </xsl:template>

  <!--
      Declare <datatype>_send(), that creates a request and calls send()
      to send it.
  -->

  <xsl:template match="request" mode="request-prototypes">
    <xsl:text>void </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_send(ONLY IN_THREAD</xsl:text>
    <xsl:if test="count(parameter) &gt; 0">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:call-template name="only-prototypes" />
    <xsl:text>);&#10;&#10;</xsl:text>
  </xsl:template>

  <!-- Declare received_<name> for replies -->
  <xsl:template match="reply" mode="request-prototypes">
    <xsl:text>void received_</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>(ONLY IN_THREAD</xsl:text>
    <xsl:if test="count(parameter) &gt; 0">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:call-template name="only-prototypes" />
    <xsl:text>);&#10;&#10;</xsl:text>
  </xsl:template>

  <!--
      Generate code to either calculate the size of a datatype, or
      to actually generate it.

This used in the separate _sizeof() and _generate() methods.
The _send() method for a request includes both the size and the generate
code path.

  -->

  <xsl:template name="generate_or_sizeof">

    <xsl:param name="which" />

    <xsl:for-each select="element">
      <xsl:text>    </xsl:text>

      <!-- Figure out what to do for each element -->

      <xsl:choose>
	<xsl:when test="count(length) &gt; 0">

	  <!--
	      This element specifies the size of some list, elsewhere in the
	      request. Invoke the _generate() or _sizeof() method for
	      "arg.size()"
	  -->

	  <xsl:if test="$which = 'generate'">
	    <xsl:choose>
	      <xsl:when test="count(lengthinbytes) &gt; 0">
		<!-- The length will be in bytes, for now generate a dummy entry -->

		<xsl:text>auto </xsl:text>
		<xsl:value-of select="length" />
		<xsl:text>_lengthpos=p; </xsl:text>
		<xsl:value-of select="type" />
		<xsl:text>_generate(p, 0);</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="type" />
		<xsl:text>_generate(p, </xsl:text>
		<xsl:value-of select="length" />
		<xsl:text>.size());</xsl:text>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:if>

	  <xsl:if test="$which = 'sizeof'">
	    <xsl:text>n += </xsl:text>
	    <xsl:value-of select="type" />
	    <xsl:text>_sizeof(</xsl:text>
	    <xsl:value-of select="length" />
	    <xsl:text>.size());</xsl:text>
	  </xsl:if>
	</xsl:when>

	<xsl:when test="count(constant) &gt; 0">

	  <!--
	      This is a constant value. Invoke the _generate() or _sizeof()
	      method for the constant value.
	  -->

	  <xsl:if test="$which = 'generate'">
	    <xsl:value-of select="type" />
	    <xsl:text>_generate(p, </xsl:text>
	    <xsl:value-of select="constant" />
	    <xsl:text>);</xsl:text>
	  </xsl:if>

	  <xsl:if test="$which = 'sizeof'">
	    <xsl:text>n += </xsl:text>
	    <xsl:value-of select="type" />
	    <xsl:text>_sizeof(</xsl:text>
	    <xsl:value-of select="constant" />
	    <xsl:text>);</xsl:text>
	  </xsl:if>

	</xsl:when>

	<xsl:when test="count(list) &gt; 0">

	  <!--
	      This is a list. Iterate over the container, and invoke the
	      _generate() or _sizeof() method for each element in the list.
	  -->

	  <xsl:if test="$which = 'generate'">
	    <xsl:if test="count(lengthinbytes) &gt; 0">
	      <xsl:text>auto </xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text>_start=p; </xsl:text>
	    </xsl:if>
	  </xsl:if>

	  <xsl:text>for (auto &amp;v:</xsl:text>
	  <xsl:value-of select="list" />
	  <xsl:text>) { </xsl:text>

	  <xsl:if test="$which = 'generate'">
	    <xsl:value-of select="type" />
	    <xsl:text>_generate(p, v);</xsl:text>
	  </xsl:if>

	  <xsl:if test="$which = 'sizeof'">
	    <xsl:text>n += </xsl:text>
	    <xsl:value-of select="type" />
	    <xsl:text>_sizeof(v);</xsl:text>
	  </xsl:if>

	  <xsl:text> }</xsl:text>

	  <xsl:if test="$which = 'generate'">
	    <xsl:if test="count(lengthinbytes) &gt; 0">
	      <xsl:text> </xsl:text>
	      <xsl:value-of select="lengthinbytes" />
	      <xsl:text>_generate(</xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text>_lengthpos, p-</xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text>_start);</xsl:text>
	    </xsl:if>
	  </xsl:if>

	</xsl:when>

	<xsl:when test="count(listpad) &gt; 0">
	  <!--
	      Generate padding for a list. Calculate the padded value, and
	      invoke CARD8_generate() for the requisite number of bytes.
	  -->

	  <xsl:text>{auto pad=(4-((</xsl:text>
	  <xsl:value-of select="listpad" />
	  <xsl:text>.size()+</xsl:text><xsl:value-of select="extra" />
	  <xsl:text>) % 4)) % 4; </xsl:text>

	  <xsl:if test="$which = 'generate'">
	    <xsl:text>while (pad) {*p++=0; --pad; }}</xsl:text>
	  </xsl:if>

	  <xsl:if test="$which = 'sizeof'">
	    <xsl:text> n += pad; }</xsl:text>
	  </xsl:if>
	</xsl:when>

	<xsl:otherwise>

	  <!--
	      An explicitly given parameter. Invoke the _generate() or
	      _sizeof() method accordingly.
	  -->

	  <xsl:if test="$which = 'generate'">
	    <xsl:value-of select="type" />
	    <xsl:text>_generate(p, </xsl:text>
	    <xsl:value-of select="name" />
	    <xsl:text>);</xsl:text>
	  </xsl:if>

	  <xsl:if test="$which = 'sizeof'">
	    <xsl:text>n += </xsl:text>
	    <xsl:value-of select="type" />
	    <xsl:text>_sizeof(</xsl:text>
	    <xsl:value-of select="name" />
	    <xsl:text>);</xsl:text>
	  </xsl:if>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:text>&#10;</xsl:text>
    </xsl:for-each>
  </xsl:template>

  <!-- _generate(), _sizeof(), and received_() methods for datatypes -->

  <xsl:template match="datatype" mode="request">
    <xsl:text>void ximserverObj::</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_generate(uint8_t * &amp;p</xsl:text>
    <xsl:call-template name="extra-prototypes" />
    <xsl:text>)&#10;{&#10;</xsl:text>

    <xsl:call-template name="generate_or_sizeof">
      <xsl:with-param name="which"><xsl:text>generate</xsl:text></xsl:with-param>
    </xsl:call-template>

    <xsl:text>}&#10;&#10;size_t ximserverObj::</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_sizeof(</xsl:text>
    <xsl:call-template name="only-prototypes" />
    <xsl:text>)&#10;{&#10;    size_t n=0;&#10;&#10;</xsl:text>

    <xsl:call-template name="generate_or_sizeof">
      <xsl:with-param name="which"><xsl:text>sizeof</xsl:text></xsl:with-param>
    </xsl:call-template>
    <xsl:text>&#10;    return n;&#10;}&#10;&#10;</xsl:text>

    <xsl:text>bool ximserverObj::</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_received(ONLY IN_THREAD, const uint8_t * &amp;data, size_t &amp;data_size</xsl:text>
    <xsl:for-each select="parameter">
      <xsl:text>, </xsl:text>
      <xsl:value-of select="type" />
      <xsl:text> &amp;</xsl:text>
      <xsl:value-of select="name" />
    </xsl:for-each>
    <xsl:text>)&#10;{&#10;</xsl:text>

    <xsl:call-template name="parse" />

    <xsl:text>    return true;&#10;}&#10;&#10;</xsl:text>
  </xsl:template>

  <!-- _send() methods for requests -->

  <xsl:template match="request" mode="request">

    <xsl:text>void ximserverObj::</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>_send(ONLY IN_THREAD</xsl:text>
    <xsl:if test="count(parameter) &gt; 0">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:call-template name="only-prototypes" />
    <xsl:text>)&#10;{&#10;    LOG_DEBUG("Sending </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text> message");&#10;    size_t n=4;&#10;&#10;</xsl:text>

    <xsl:call-template name="generate_or_sizeof">
      <xsl:with-param name="which"><xsl:text>sizeof</xsl:text></xsl:with-param>
    </xsl:call-template>

    <xsl:text>    n += (4-(n%4)) % 4;&#10;&#10;    uint8_t buffer[n];&#10;    uint8_t *p=buffer+4;&#10;&#10;</xsl:text>

    <xsl:call-template name="generate_or_sizeof">
      <xsl:with-param name="which"><xsl:text>generate</xsl:text></xsl:with-param>
    </xsl:call-template>

    <xsl:text>&#10;    while (p &lt; buffer+n) *p++=0;&#10;    buffer[0]=</xsl:text>
    <xsl:value-of select="major" />
    <xsl:text>; buffer[1]=0;&#10;    auto s=(n - 4)/4;&#10;    buffer[2]=s >> 8; buffer[3]=s;&#10;    send(IN_THREAD, buffer, n);&#10;}&#10;</xsl:text>
  </xsl:template>

  <!-- ****************************************************************** -->

  <!-- Parse datatype's or reply's elements -->

  <xsl:template name="parse">

    <xsl:for-each select="unused">
      <xsl:text>    </xsl:text>
      <xsl:value-of select="type" />
      <xsl:text> </xsl:text>
      <xsl:value-of select="name" />
      <xsl:text>;&#10;</xsl:text>
    </xsl:for-each>

    <xsl:for-each select="element|if">
      <xsl:choose>

	<xsl:when test="name()='if'">
	  <xsl:text>if (</xsl:text>
	  <xsl:value-of select="@test" />
	  <xsl:text>) {&#10;</xsl:text>
	  <xsl:call-template name="parse" />
	  <xsl:text>}&#10;</xsl:text>
	</xsl:when>

	<!-- Receive length of array -->

	<xsl:when test="count(length) &gt; 0">
	  <xsl:text>    </xsl:text>
	  <xsl:value-of select="type" />
	  <xsl:text> length_</xsl:text>
	  <xsl:value-of select="length" />
	  <xsl:text>{};&#10;</xsl:text>

	  <xsl:text>    if (!</xsl:text>
	  <xsl:value-of select="type" />
	  <xsl:text>_received(IN_THREAD, data, data_size, length_</xsl:text>
	  <xsl:value-of select="length" />
	  <xsl:text>)) badmessage(IN_THREAD, "</xsl:text>
	  <xsl:call-template name="badmessagename" />
	  <xsl:text>");&#10;</xsl:text>
	</xsl:when>

	<!-- And now the list itself -->
	<xsl:when test="count(list) &gt; 0">

	  <xsl:choose>
	    <xsl:when test="count(lengthinbytes) &gt; 0">
	      <!-- length specified in bytes -->

	      <xsl:text>    {&#10;        auto list_data=data;&#10;        size_t list_size=length_</xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text>;&#10;        if (data_size &lt; list_size) badmessage(IN_THREAD, "</xsl:text>
	      <xsl:call-template name="badmessagename" />
	      <xsl:text>");&#10;        data += list_size;&#10;        data_size -= list_size;&#10;        while (list_size)&#10;        {&#10;            </xsl:text>
	      <xsl:value-of select="recvtype" />
	      <xsl:text> v;&#10;            if (!</xsl:text>
	      <xsl:value-of select="type" />
	      <xsl:text>_received(IN_THREAD, list_data, list_size, v)) badmessage(IN_THREAD, "</xsl:text>
	      <xsl:call-template name="badmessagename" />
	      <xsl:text>");&#10;            </xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text>.push_back(v);&#10;        }&#10;    }&#10;&#10;</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <!-- length specified as number of elements -->

	      <xsl:text>    while (length_</xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text> &gt; 0) { </xsl:text>
	      <xsl:value-of select="recvtype" />
	      <xsl:text> v{}; if (!</xsl:text>
	      <xsl:value-of select="type" />
	      <xsl:text>_received(IN_THREAD, data, data_size, v)) badmessage(IN_THREAD, "</xsl:text>
	      <xsl:call-template name="badmessagename" />
	      <xsl:text>"); </xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text>.push_back(v); --length_</xsl:text>
	      <xsl:value-of select="list" />
	      <xsl:text>; }&#10;</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>

	<!-- Skip padding -->
	<xsl:when test="count(listpad) &gt; 0">

	  <xsl:text>    {auto pad=(4-((</xsl:text>
	  <xsl:value-of select="listpad" />
	  <xsl:text>.size()+</xsl:text><xsl:value-of select="extra" />
	  <xsl:text>) % 4)) % 4; ; uint8_t v; while (pad) { if (!CARD8_received(IN_THREAD, data, data_size, v)) badmessage(IN_THREAD, "padding"); --pad; }}&#10;</xsl:text>
	</xsl:when>

	<xsl:otherwise>
	  <xsl:text>    if (!</xsl:text>
	  <xsl:value-of select="type" />
	  <xsl:text>_received(IN_THREAD, data, data_size, </xsl:text>
	  <xsl:value-of select="name" />
	  <xsl:text>)) badmessage(IN_THREAD, "</xsl:text>
	  <xsl:call-template name="badmessagename" />
	  <xsl:text>");&#10;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>

  <!-- The big switch that goes into received() -->

  <xsl:template match="reply" mode="received-switch">
    <xsl:text>case </xsl:text><xsl:value-of select="major" /><xsl:text>:&#10;{&#10;    LOG_DEBUG("Received </xsl:text>
    <xsl:value-of select="name" />
    <xsl:text> message");&#10;</xsl:text>

    <xsl:for-each select="parameter">
      <xsl:text>    </xsl:text>
      <xsl:value-of select="type" />
      <xsl:text> </xsl:text>
      <xsl:value-of select="name" />
      <xsl:choose>
	<xsl:when test="default">
	  <xsl:text>=</xsl:text>
	  <xsl:value-of select="default" />
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>{}</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:text>;&#10;</xsl:text>
    </xsl:for-each>

    <xsl:call-template name="parse" />

    <xsl:text>&#10;    received_</xsl:text>
    <xsl:value-of select="name" />
    <xsl:text>(IN_THREAD</xsl:text>

    <xsl:for-each select="parameter">
      <xsl:text>, </xsl:text>
      <xsl:value-of select="name" />
    </xsl:for-each>
    <xsl:text>);&#10;    break;&#10;}&#10;</xsl:text>
  </xsl:template>

  <!-- Top level element -->

  <xsl:template match="protocol">
    <xsl:if test="$mode = 'request'">
      <xsl:apply-templates select="@*|node()" mode='request' />
    </xsl:if>

    <xsl:if test="$mode = 'request-prototypes'">
      <xsl:apply-templates select="@*|node()" mode='request-prototypes' />
    </xsl:if>

    <xsl:if test="$mode = 'received-switch'">
      <xsl:apply-templates select="@*|node()" mode='received-switch' />
    </xsl:if>
  </xsl:template>

  <xsl:template match="*|/">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="*|/" mode="request-prototypes">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="*|/" mode="request">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="*|/" mode="received-switch">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="text()" />
  <xsl:template match="text()" mode="request" />
  <xsl:template match="text()" mode="request-prototypes" />
  <xsl:template match="text()" mode="received-switch" />

</xsl:stylesheet>
