/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_theme_text_H
#define x_w_theme_text_H

#include <x/w/theme_textfwd.H>
#include <x/w/uigeneratorsfwd.H>
#include <string>

LIBCXXW_NAMESPACE_START

/*! Helper for creating a \ref text_param "text parameter" that uses fonts and colors, in gettext-compatible way

\code
#include &lt;INSERT_LIBX_NAMESPACE/locale.H&gt;
#include &lt;INSERT_LIBX_NAMESPACE/messages.H&gt;

auto catalog=INSERT_LIBX_NAMESPACE::w::messages::create("app", INSERT_LIBX_NAMESPACE::locale::base::utf8());

auto generators=INSERT_LIBX_NAMESPACE::w::uigenerators::create("template.xml", catalog);

INSERT_LIBX_NAMESPACE text_param t{
    INSERT_LIBX_NAMESPACE::theme_text{ _("${color:black}Ok"), generators }
};

factory->create_label(t);

\endcode

The \c theme_text object gets constructed and passed as a
\ref text_param "text_param"
parameter, or, alternatively, gets used to construct an explicit
\ref text_param "text_param".

The first parameter to the constructor is a string that's optionally amenable to
internationalization, using the gettext-visible _() macro.

The second parameter is an optional \ref uigenerators "parsed theme file" that
provides optional colors and fonts.

Colors, fonts, and other \c text_param markup gets specified by placing
${macro}s in the text string. "$$" results in a single "$" character in the
resulting string. Effectively, ${macro} executed \c text_param's appropriate
\c () overloaded operator, as follows:

- ${color:name} - adds color "name" to the \c text_param,
referencing the theme-defined color. "name" is a
\ref rgb "standard HTML 3.2 color";
are always available; other "name"s must be specified in the theme file.

- ${font:name} - adds font "name" from the theme file to the \c text_param.

- ${decoration:underline} and ${decoration:none} - turns on/off the
corresponding text decoration.

- ${context:id} - this does nothing, and is skipped over. This is used to
provide distinct strings for translated strings, used with gettext().
Identical theme strings differing only in the "id" values result in
distinct internationalizable strings:

- $# - introduces a "comment". The \c $# and all following text up until the
next newline gets ignored. If there are any spaces or tabs that immediately
precede the \c $# they get replaced by a single space character. No replacement
occurs if the only whitespace precedes the \c $#. This is mostly for use
in theme files:

\code
   <label>                 $#
Lorem Ipsum                $#

Dolor Sit Amet</label>
\endcode

This produces the text string "Lorem Ipsum \nDolor Sit Amet". Except for the
whitespace removed by the \c $#, all whitespace and newlines in the theme
text get preserved. In the above example, the rich text starts immediately after the \c &lt;label&gt;, but all whitespace on the first line gets removed by the
first \c $# together with the following newline. All whitespace and the newline
on the 2nd line gets replaced by a single space, then the newline from the
blank line; and finally the last line of the theme text.

\code
INSERT_LIBX_NAMESPACE::theme_text{ _("${context:file}Menu"), generators }

// ...

INSERT_LIBX_NAMESPACE::theme_text{ _("${context:tools}Menu"), generators }
\endcode

This results in two distinct translatable strings. Both strings appear as
"Menu" in the base English version, but they are used in different contexts.
Here, "Menu" can be translated differently, depending on its context
(in another language, it could be translated differently for the "File"
menu or the "Tools" menu, as a simple example).

\par Internationalization and message catalogs

The 2nd parameter to theme_text's constructor is a
\ref messages "message catalog" object. For best results the message catalog
should get constructed with a UTF-8 \ref locale "locale", so that the
translated text string gets converted directly into the internal Unicode
string.

*/

struct theme_text {

	//! Constructor
	theme_text(const std::string &);

	//! Constructor with a custom generator
	theme_text(const std::string &,
		   const const_uigenerators &);

	//! Destructor
	~theme_text();

	//! Copy constructor
	theme_text(const theme_text &);
	//! Move constructor
	theme_text(theme_text &&);

	//! Assignment operator
	theme_text &operator=(const theme_text &);

	//! Move operator
	theme_text &operator=(theme_text &&);

	//! The string
	std::string text;

	//! The generator
	const_uigenerators generators;

	//! Return the plain Unicode string.

	//! This provides a convenient shortcut for embedding gettext-ized
	//! strings with context indicators:
	//!
	//! theme_text(_("${context:menu}Open")).get_unicode();

	std::u32string get_unicode() const;
};

LIBCXXW_NAMESPACE_END

#endif
