/*
** Copyright 2019 Double Precision, Inc.
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
INSERT_LIBX_NAMESPACE text_param t{
    INSERT_LIBX_NAMESPACE::theme_text{ _("${color:black}Ok"), generators }
};

factory->create_label(INSERT_LIBX_NAMESPACE::theme_text{ _("${color:red}Ok"), generators });

\endcode

The \c theme_text object gets constructed and passed as a
\ref text_param "text_param"
parameter, or, alternatively, gets used to construct an explicit
\ref text_param "text_param".

The first parameter to the constructor is a string that's amenable to
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
};

LIBCXXW_NAMESPACE_END

#endif
