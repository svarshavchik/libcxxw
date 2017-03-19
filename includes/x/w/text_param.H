/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_param_H
#define x_w_text_param_H

#include <string>
#include <x/w/font.H>
#include <x/w/rgb.H>
#include <x/w/theme_font.H>
#include <x/w/theme_color.H>
#include <x/w/screenfwd.H>
#include <string>
#include <unordered_map>
#include <utility>

LIBCXXW_NAMESPACE_START

//! A text parameter for a new display element.

//! This parameter specifies a text string when creating
//! a new display element using the \ref factory "element factory"; such
//! as create_label(), for example.
//!
//! The text is more than just text, it may include colors and markups.
//!
//! The first way to create a \c text_param is by using uniform initialization
//! syntax:
//!
//! \code
//! factory->create_label({INSERT_LIBX_NAMESPACE::w::rgb(0,0,0),
//!                        "Hello world"});
//! \endcode
//!
//! This invokes \c text_param's constructor, that takes a variadic parameter
//! pack.
//!
//! A \c text_param can also be created using a default constructor, and
//! then initialized one piece at a time.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p(INSERT_LIBX_NAMESPACE::w::rgb(0, 0, 0));
//! p("Hello world");
//!
//! factory->create_label(p);
//! \endcode
//!
//! or
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p(INSERT_LIBX_NAMESPACE::w::rgb(0, 0, 0), "Hello world");
//!
//! factory->create_label(p);
//! \endcode
//!
//! The available parameters to the constructor, and to the \c () operator
//! are:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p("Hello world");
//! \endcode
//!
//! A \c std::string specifies a UTF-8 string.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p(U"Hello world");
//! \endcode
//!
//! A \c std::u32string specifies a unicode string.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p(INSERT_LIBX_NAMESPACE::w::font{"liberation sans", 12.0});
//! p("Hello world");
//!
//! // ... or ...
//!
//! p(INSERT_LIBX_NAMESPACE::w::font{"liberation sans", 12.0}, "Hello world");
//! \endcode
//!
//! A \ref font "INSERT_LIBX_NAMESPACE::w::font" specifies a new font.
//! This must be followed by at least one additional string parameter (either
//! UTF-8 or unicode), otherwise an exception gets thrown.
//! The specified font applies to all subsequent strings passed to the
//! constructor or the \c () operator, until the next font, if there is one.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p(INSERT_LIBX_NAMESPACE::w::theme_font{"label"};
//! p("Hello world");
//!
//! // ... or ...
//!
//! #include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>
//!
//! p("label"_font, "Hello world!");
//! \endcode
//!
//! A \ref theme_font "INSERT_LIBX_NAMESPACE::w::theme_font" specifies a new
//! theme-specified font.
//! This must be followed by at least one additional string parameter (either
//! UTF-8 or unicode), otherwise an exception gets thrown.
//! The specified theme font applies to all subsequent strings passed to the
//! constructor or the \c () operator, until the next font, if there is one.
//!
//! Including \c text_param_literals.H allows a theme font to be specified
//! using a \c "name"_font string literal.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p(INSERT_LIBX_NAMESPACE::w::rgb{INSERT_LIBX_NAMESPACE::w::rgb::maximum,
//!           INSERT_LIBX_NAMESPACE::w::rgb::maximum,
//!           INSERT_LIBX_NAMESPACE::w::rgb::maximum});
//! p("Hello world");
//!
//! \endcode
//!
//! A \ref rgb "INSERT_LIBX_NAMESPACE::w::rgb" specifies a new color.
//! This must be followed by at least one additional string parameter (either
//! UTF-8 or unicode), otherwise an exception gets thrown.
//! The specified color applies to all subsequent strings passed to the
//! constructor or the \c () operator, until the next font, if there is one.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::text_param p;
//!
//! p(INSERT_LIBX_NAMESPACE::w::theme_color{"highlight"};
//! p("Hello world");
//!
//! // ... or ...
//!
//! #include <INSERT_LIBX_NAMESPACE/w/text_param_literals.H>
//!
//! p("hightlight"_color, "Hello world!");
//! \endcode
//!
//! A \ref theme_color "INSERT_LIBX_NAMESPACE::w::theme_color" specifies a new
//! theme-specified color.
//! This must be followed by at least one additional string parameter (either
//! UTF-8 or unicode), otherwise an exception gets thrown.
//! The specified theme color applies to all subsequent strings passed to the
//! constructor or the \c () operator, until the next color, if there is one.
//!
//! Including \c text_param_literals.H allows a theme color to be specified
//! using a \c "name"_color string literal.
//!
//! \code
//! p("hightlight_fg"_color, "hightlight_fg"_color, "Hello world!");
//! \endcode
//!
//! Specifying two colors in a row, without an intervening text string,
//! has the effect of setting both the foreground and the background color
//! for the following text. Either the foreground or the background color
//! can be an explicit \ref rgb "rgb color", or a theme color.
//!
//! Both the foreground and background color apply to all subsequent strings
//! passed to the constructor or the \c () operator, until the next color, if
//! there is one.

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
	std::unordered_map<size_t, font> fonts;

	//! Fonts for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.
	std::unordered_map<size_t, std::string> theme_fonts;

	//! Colors for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.

	std::unordered_map<size_t, rgb> colors;
	//! Colors for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.
	std::unordered_map<size_t, rgb> background_colors;

	//! Colors for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.
	std::unordered_map<size_t, std::string> theme_colors;
	//! Colors for the text string being built.

	//! \internal
	//!
	//! Do not touch, use \c operator() exclusively.
	std::unordered_map<size_t, std::string> theme_background_colors;

	//! Default constructor
	text_param();

	//! Destructor
	~text_param();

	//! Copy constructor
	text_param(const text_param &);

	//! Assignment operator
	text_param &operator=(const text_param &);

	//! Variadic constructor

	//! Forward to the default constructor, then forward to \c operator().

	template<typename first_arg, typename ...args>
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
	text_param &operator()(const std::experimental::string_view &s);

	//! Add more text.
	text_param &operator()(const std::experimental::u32string_view &s);

	//! Use this font for all text going forward.
	text_param &operator()(const font &f);

	//! Use this font for all text going forward.
	text_param &operator()(const theme_font &f);

	//! Use this color for all text going forward.
	text_param &operator()(const rgb &);

	//! Use this color for all text going forward.
	text_param &operator()(const theme_color &);

	//! Variadic list of parameters to the \c () operator.

	//! Invokes \c operator() one parameter at a time.

	template<typename first_arg, typename second_arg, typename ...args>
	inline text_param &operator()(first_arg &&arg1,
				      second_arg &&arg2,
				      args && ...remaining_args)
	{
		return operator()(std::forward<first_arg>(arg1))
			(std::forward<second_arg>(arg2),
			 std::forward<args>(remaining_args)...);
	}
};

LIBCXXW_NAMESPACE_END

#endif
