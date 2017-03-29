/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_font_literals_H
#define x_w_font_literals_H

#include <x/w/font.H>

LIBCXXW_NAMESPACE_START

/*!

Construct a \ref font "INSERT_LIBX_NAMESPACE::w::font" from a string.

\c "_font" literal suffix constructs a \ref font "font" from a string.
The string, at a minimum consists of a font family name. A
semicolon-delimited list of attributes follows the font family name,
for example:

\code
"liberation mono; point_size=10; weight=bold"_font
\endcode

The available attributes are:

- foundry=name

Sets the font's foundry name.

- point_size=n

Sets the font's size to \c n, which may be fractional. The font's size
gets automatically scaled by the current theme.

- weight=value

Sets the font's weight. "value" is one of: "thin", "light", "normal", "medium",
"demibold", "bold", "heavy", or "extrablack".

- slant=value

Sets the font's slant. "value" is one of: "roman", "italic", or "oblique".

- width=value

Sets the font's width. "value" is one of: "condensed", "normal", or "expanded".

- style=label

Sets the font's style. This is a font-specific property, that depends on the
font.

- spacing=value

Sets the font's spacing. "value" is one of: "proportional", "dual",
"monospace", or "charcell".

*/

font operator"" _font(const char *, size_t);

LIBCXXW_NAMESPACE_END

using LIBCXXW_NAMESPACE::operator "" _font;

#endif
