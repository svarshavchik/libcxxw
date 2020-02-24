/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uixmlparser_h
#define x_w_uixmlparser_h

#include <x/w/namespace.H>
#include <x/w/rgb.H>
#include <x/xml/doc.H>
#include <x/locale.H>
#include <x/functional.H>
#include <string>
#include <variant>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

namespace ui {
#if 0
}
#endif

//! Internal lock used when reading the XML file.

//! Temporary workaround until libstdc++ implement floating point
//! from_chars: carry around the "C" locale, in order to parse the
//! numerical values in the XML file. It's convenient to swallow
//! the XML document's read lock, and attach a locale object, in this manner.

class parser_lock : public xml::doc::base::readlock {

public:

	const_locale c_locale;

	//! Constructor
	parser_lock(const xml::doc::base::readlock &l);

	//! Constructor
	parser_lock(const xml::doc::base::readlock &l,
		    const const_locale &c_locale);
	//! Destructor
	~parser_lock();

	//! Clone the lock.
	parser_lock clone() const;
};

//! Parsed \<dim> value in the UI theme file

struct parsed_dim {
	//! Value of the scale attribute, if not empty.
	std::string scale;

	//! \<dim>'s value.
	std::string value;
};

//! Load the values from a \<dim>
parsed_dim parse_dim(const xml::doc::base::readlock &lock);

//! A \<color> with a "scale" attribute

//! Variant value returned by parse_color() for a \<color> entry with a
//! scale= attribute.
//!
//! \see parse_color

struct parsed_scaled_color {

	//! Non-empty "scale" value.

	std::string from_name;
	std::optional<double> r, //!< \<r> value, if present
		g, //! \<g> value, if present
		b, //! \<b> value, if present
		a; //! \<a> value, if present

	//! Comparison operator

	inline bool operator==(const parsed_scaled_color &o) const
	{
		return from_name == o.from_name
			&& r == o.r
			&& g == o.g
			&& b == o.b
			&& a == o.a;
	}

	//! Comparison operator
	inline bool operator!=(const parsed_scaled_color &o) const
	{
		return !operator==(o);
	}
};

//! Common superclass of parse_linear_gradient and parse_radial_gradient

//! \see parse_color

struct parse_gradient {

	void parse_gradient_values(const ui::parser_lock &lock,
				   const function<void (size_t,
							const std::string &)
				   > &cb) const;
};

//! A \<color> that specifies a linear gradient.

//! Variant value returned by parse_color() for a \<color> entry with a
//! type=linear_gradient attribute.
//!
//! \see parse_color

struct parse_linear_gradient : parse_gradient {

	//! Parse the gradient color.

	template<typename F>
	void parse(const ui::parser_lock &lock,
		   linear_gradient_values &values,
		   F &&f) const
	{
		do_parse(lock, values,
			 make_function<void (size_t, const std::string &)>
			 (std::forward<F>(f)));
	}

	//! Type-erased parse().
	void do_parse(const ui::parser_lock &lock,
		      linear_gradient_values &values,
		      const function<void (size_t, const std::string &)> &cb)
		const;
};

//! A \<color> that specifies a radial gradient.

//! Variant value returned by parse_color() for a \<color> entry with a
//! type=radial_gradient attribute.
//!
//! \see parse_color

struct parse_radial_gradient : parse_gradient {

	//! Parse the gradient color.

	template<typename F>
	void parse(const ui::parser_lock &lock,
		   radial_gradient_values &values,
		   F &&f) const
	{
		do_parse(lock, values,
			 make_function<void (size_t, const std::string &)>
			 (std::forward<F>(f)));
	}

	//! Type-erased parse().
	void do_parse(const ui::parser_lock &lock,
		      radial_gradient_values &values,
		      const function<void (size_t, const std::string &)> &cb)
		const;
};

//! Type of a \<color> entry in the theme file

//! Variant returned by parse_color() indicating what kind of a \<color>
//! entry gets parsed here.
//!
//! \see parse_color().

typedef std::variant<rgb, parsed_scaled_color, parse_linear_gradient,
		     parse_radial_gradient> parse_color_t;

//! Parse a \<color> entry in the theme file.

//! parse_color() returns a variant that indicates the type of the \<color>
//! entry at the position given by the lock:
//!
//! - a solid color returns an \ref rgb "solid color RGB value", already
//!   parsed
//!
//! - a \<color> with a "scale" attribute returns a
//! \ref parsed_scaled_color "parsed_scaled_color" object, giving the
//! value of the scale attribute, and whatever r/g/b/a scale value were
//! specified.
//!
//! - a \<color> with a "type" of "linear_gradient" returns a
//! \ref parse_linear_gradient "parse_linear_gradient" object that further
//! parses the color as follows: it's parse() method gets invoked with the
//! same lock, a \ref linear_gradient_values "linear_gradient_values" object,
//! which gets initialized from the \<color>, and a callable object. parse()
//! invokes the callable object for each individual \<gradient>, and
//! receives two parameters: the <value>, a \c size_t, and the \<color>,
//! a \c std::string.
//!
//! - a \<color> with a "type" of "radial_gradient" returns a
//! \ref parse_radial_gradient "parse_radial_gradient" object that further
//! parses the color as follows: it's parse() method gets invoked with the
//! same lock, a \ref radial_gradient_values "radial_gradient_values" object,
//! which gets initialized from the \<color>, and a callable object. parse()
//! invokes the callable object for each individual \<gradient>, and
//! receives two parameters: the <value>, a \c size_t, and the \<color>,
//! a \c std::string.

parse_color_t parse_color(const parser_lock &lock);

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
