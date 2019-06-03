/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_param_H
#define x_w_text_param_H

#include <string>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/font_arg.H>
#include <x/w/theme_font.H>
#include <x/w/theme_color.H>
#include <x/w/screenfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/text_hotspotfwd.H>
#include <x/w/theme_textfwd.H>

#include <string>
#include <unordered_map>
#include <utility>
#include <type_traits>

LIBCXXW_NAMESPACE_START

//! Optional text decorations for \ref text_param "text parameters".

enum class text_decoration {
	none=0,
	underline=1,
};

template<typename ...> struct text_param_constructible_d;

template<typename ...Args>
using text_param_constructible=
	typename text_param_constructible_d<Args...>::type;

/*! \brief \anchor text_param A text parameter for a new widget.

This parameter specifies a text string when creating
a new widget using the \ref factory "element factory"; such
as create_label(), for example.

The text may include colors and markups, as described here.

The first way to create a \c text_param is with uniform initialization
syntax:

\code
factory->create_label({INSERT_LIBX_NAMESPACE::w::rgb(0,0,0),
                       "Hello world"});
\endcode

This invokes \c text_param's constructor, which takes a variadic parameter
pack.

A \c text_param also has a default constructor. This makes it possible to
construct a \c text_param one piece at a time.

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p(INSERT_LIBX_NAMESPACE::w::rgb(0, 0, 0));
p("Hello world");

factory->create_label(p);
\endcode

or

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p(INSERT_LIBX_NAMESPACE::w::rgb(0, 0, 0), "Hello world");

factory->create_label(p);
\endcode

The \c () operator takes a variadic parameter pack, and like the constructor
each parameter gets processed individually.

The available parameters to the constructor, and to the \c () operator
are:

\par Text

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p("Hello world");
\endcode

A \c std::string specifies a string in the current locale charset.

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p(U"Hello world");
\endcode

A \c std::u32string specifies a unicode string.

\par Fonts

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p(INSERT_LIBX_NAMESPACE::w::font{"liberation sans", 12.0});
p("Hello world");

// ... or ...

p(INSERT_LIBX_NAMESPACE::w::font{"liberation sans", 12.0}, "Hello world");
\endcode

A \ref font "INSERT_LIBX_NAMESPACE::w::font" specifies a new font.
At least one additional string parameter (either string or unicode) must
follow a font specification, otherwise an exception gets thrown; either
in the same variadic parameter pack, or in a subsequent \c () call.

The specified font applies to all subsequent string or strings, passed to the
constructor or the \c () operator, until the next font change, if there is one.

\code
#include <INSERT_LIBX_NAMESPACE/w/font_literals.H>

INSERT_LIBX_NAMESPACE::w::text_param p;

p("liberation mono;point_size=18"_font);
p("Hello world");
\endcode

\c font_literals.H allows using a \c "font"_font string literal to
to specify a font.

\par Theme fonts

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p(INSERT_LIBX_NAMESPACE::w::theme_font{"label"});
p("Hello world");

// ... or ...

#include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>

p("label"_theme_font, "Hello world!");
\endcode

A \ref theme_font "INSERT_LIBX_NAMESPACE::w::theme_font" specifies a new
theme-specified font, instead of a custom font.
The specified theme font applies to all subsequent strings passed to the
constructor or the \c () operator, until the next font, if there is one.

\c text_param_literals.H allows using a \c "name"_theme_font string literal
to specify a theme font.

\par Colors

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p(INSERT_LIBX_NAMESPACE::w::rgb{INSERT_LIBX_NAMESPACE::w::rgb::maximum,
          INSERT_LIBX_NAMESPACE::w::rgb::maximum,
          INSERT_LIBX_NAMESPACE::w::rgb::maximum});
p("Hello world");

\endcode

A \ref rgb "INSERT_LIBX_NAMESPACE::w::rgb" parameter specifies a new color.
This must be followed by at least one additional string parameter (either
string or unicode), otherwise an exception gets thrown.
The specified color applies to all subsequent strings passed to the
constructor or the \c () operator, until the next font, if there is one.

\par Theme colors

\code
INSERT_LIBX_NAMESPACE::w::text_param p;

p(INSERT_LIBX_NAMESPACE::w::theme_color{"highlight"};
p("Hello world");

// ... or ...

#include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>

p("highlight"_color, "Hello world!");
\endcode

A \ref theme_color "INSERT_LIBX_NAMESPACE::w::theme_color" specifies a new
theme-specified color.
At least one additional string parameter (either string or unicode) must
follow a color specification, otherwise an exception gets thrown; either
in the same variadic parameter pack, or in a subsequent \c () call.
\c text_param_literals.H allows using a \c "name"_color string literal
to specify a color.

\par Foreground and background colors

\code
p("highlight_fg"_color, "highlight_bg"_color, "Hello world!");
\endcode

Specifying two colors in a row, without an intervening text string,
has the effect of setting both the foreground and the background color
for the following text. Either the foreground or the background color
can be an explicit \ref rgb "rgb color", or a theme color.

Both the foreground and background color apply to all subsequent strings
passed to the constructor or the \c () operator, until the next color
specification, if there is one.

\par Text decorations

\code
p(INSERT_LIBX_NAMESPACE::w::text_decoration::underline);
\endcode

All following text gets underlined until the next \c none decoration.

\code
p(INSERT_LIBX_NAMESPACE::w::text_decoration::none);
\endcode

Alternatively:

\code
#include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>

p("underline"_decoration,
  "Hello"
  "none"_decoration,
   " world!");
\endcode

\par gettext-compatible markup

It is possible to specify fonts, colors, and markup in a single
string parameter that's amenable to gettext-ization. See
\ref theme_text for more information.

*/

class text_param {

public:

	//! Text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.
	std::u32string string;

	//! Fonts for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.
	std::unordered_map<size_t, font_arg> fonts;

	//! Colors for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.

	std::unordered_map<size_t, text_color_arg> colors;

	//! Colors for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.
	std::unordered_map<size_t, color_arg> background_colors;

	//! Decorations for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.

	std::unordered_map<size_t, text_decoration> decorations;

	//! Hotspots for the text string being built.

	std::unordered_map<size_t, text_hotspotptr> hotspots;

	//! Undecorated, there were no specified decorations.

	//! All the maps are empty.

	bool undecorated() const;

	//! Default constructor
	text_param();

	//! Destructor
	~text_param();

	//! Copy constructor
	text_param(const text_param &);

	//! Move constructor
	text_param(text_param &&);

	//! Assignment operator
	text_param &operator=(const text_param &);

	//! Move operator
	text_param &operator=(text_param &&);

	//! Variadic constructor

	//! Forward to the default constructor, then forward to \c operator().

	template<typename first_arg, typename ...args,
		 typename=
		 std::void_t<text_param_constructible<first_arg, args...>>>
	inline text_param(first_arg &&arg1, args && ...remaining_args)
		: text_param()
	{
		operator()(std::forward<first_arg>(arg1),
			   std::forward<args>(remaining_args)...);
	}

	//! Empty \c operator(), after unwinding the variadic parameter pack.

	inline text_param &operator()()
	{
		return *this;
	}

	//! Add more text.
	text_param &operator()(const std::string_view &s);

	//! Add more text.
	text_param &operator()(const std::u32string_view &s);

	//! Helper class to avoid ambiguous resolution failures.

	//! A font has a constructor that takes a string. Nothing wrong
	//! with the constructor participating in implicit construction of
	//! a font. However, our operator() accepts both fonts and strings.
	//! This results in ambiguous overloads.
	//!
	//! Have our operator() take an explicit_font as a parameter, which
	//! is constructible from a font. So an explicit font object must
	//! be passed in to our constructor, and a string parameter resolves
	//! to our operator().

	class explicit_font {

	public:
		const font &f;

		inline explicit_font(const font &f) : f(f) {}
	};

	//! Helper class to avoid ambiguous resolution failures.

	//! Allows passing a font_arg object into text_param. The variant's
	//! inherited constructor would could also create an ambiguous overload,
	//! this helps preventing it.

	class explicit_font_arg {

	public:
		const font_arg &f;

		inline explicit_font_arg(const font_arg &f) : f{f} {}
	};

	//! Use this font for all text going forward.
	text_param &operator()(const explicit_font &f);

	//! Use this font for all text going forward.
	text_param &operator()(const theme_font &f);

	//! Use this font for all text going forward.
	text_param &operator()(const explicit_font_arg &f);

	//! Use this color for all text going forward.
	text_param &operator()(const text_color_arg &);

	//! Use this decoration for all text going forward.
	text_param &operator()(const text_decoration);

	//! Hotspot for the following text.
	text_param &operator()(const text_hotspot &);

	//! End of hotspot.
	text_param &operator()(std::nullptr_t);

	//! Add text referencing colors and fonts from a theme file.
	text_param &operator()(const theme_text &);

	//! Variadic list of parameters to the \c () operator.

	//! Invokes \c operator() one parameter at a time.

	template<typename first_arg, typename second_arg, typename ...args,
		 typename=std::void_t<text_param_constructible<first_arg,
							       second_arg,
							       args...>>>
	inline text_param &operator()(first_arg &&arg1,
				      second_arg &&arg2,
				      args && ...remaining_args)
	{
		return operator()(std::forward<first_arg>(arg1))
			(std::forward<second_arg>(arg2),
			 std::forward<args>(remaining_args)...);
	}
};

//! Helper class used by \ref text_param_constructible "text_param_constructible".

template<typename, typename=void> struct text_param_constructible_s {};

template<>
struct text_param_constructible_s<std::tuple<>, void> {

	typedef void type;
};

template<typename T, typename ...Args>
struct text_param_constructible_s<std::tuple<T, Args...>,
				  std::void_t<decltype
					      (std::declval<text_param &>()
					       .operator()(std::declval<
							   std::decay_t<T>
							   &>())
					       )>
				  >
	: public text_param_constructible_s<std::tuple<Args...>>
{
};

template<typename ...Args>
struct text_param_constructible_d :
	text_param_constructible_s<std::tuple<std::decay_t<Args>...>> {
};

LIBCXXW_NAMESPACE_END

#endif
