/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uixmlparser_h
#define x_w_uixmlparser_h

#include <x/w/namespace.H>
#include <x/w/rgb.H>
#include <x/w/fontfwd.H>
#include <x/xml/doc.H>
#include <x/xml/readlock.H>
#include <x/locale.H>
#include <x/functional.H>
#include <string>
#include <string_view>
#include <variant>
#include <optional>
#include <unordered_map>
#include <optional>

LIBCXXW_NAMESPACE_START

namespace ui {
#if 0
}
#endif

//! Internal lock used when reading the XML file.

//! A wrapper for an XML read lock, implements clone() to return a cloned
//! XML lock.

class parser_lock : public xml::readlock {

public:

	//! Constructor
	parser_lock(const xml::readlock &l);

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
parsed_dim parse_dim(const xml::readlock &lock);

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

//! Parse a \<border> entry in the theme file.

//! Construct this object and invoke parse() to parse the \<border> entry,
//! which invokes the following:
//!
//! - from(): if a "from" attribute was found.
//!
//! - color(): The \<color> entry if it exists, with an empty string for
//! "color2" if one was not specified.
//!
//! - rounded(): Called if a \<rounded> exists.
//!
//! - dashes(): Called if one or more \<dash> exists.
//!
//! - width(), height(), hradius(), vradius(),
//!   width_scale(), height_scale(), hradius_scale(), vradius_scale(): called
//!   if the corresponding elements exist.

struct parse_border {

	//! Parse the \<border> element.
	void parse(const ui::parser_lock &lock);

	//! Invoked by parse()

	//! The implementation can move the string that it gets as a parameter.
	virtual void from(std::string &)=0;

	//! Invoked by parse()

	//! The implementation can move the strings that it gets as parameters.
	virtual void color(std::string &color,
			   std::string &color2)=0;

	//! Invoked by parse()

	//! The implementation can move the string that it gets as a parameter.
	virtual void width(std::string &)=0;
	//! Invoked by parse()
	virtual void width_scale(unsigned)=0;

	//! Invoked by parse()

	//! The implementation can move the string that it gets as a parameter.

	//! The implementation can move the string that it gets as a parameter.
	virtual void height(std::string &)=0;
	//! Invoked by parse()
	virtual void height_scale(unsigned)=0;

	//! Invoked by parse()

	//! The implementation can move the string that it gets as a parameter.
	virtual void hradius(std::string &)=0;
	//! Invoked by parse()
	virtual void hradius_scale(unsigned)=0;

	//! Invoked by parse()

	//! The implementation can move the string that it gets as a parameter.
	virtual void vradius(std::string &)=0;
	//! Invoked by parse()
	virtual void vradius_scale(unsigned)=0;

	//! Invoked by parse()
	virtual void rounded(bool)=0;
	//! Invoked by parse()

	//! The implementation can move the vector that it gets as a parameter.
	virtual void dashes(std::vector<double> &)=0;

private:

	//! Internal helper for parsing colors.
	static std::string parse_color(const ui::parser_lock &lock,
				       const char *xpath_name);

};

//! Interface for parsing \<font> elements

//! Parses the contents of the \<font>. Does not parse the id and from
//! attributes.
//!
//! parse() parses the \<font> element, and invokes the callbacks.

struct parse_font {

	//! Parse contents of \<font>
	void parse(const parser_lock &lock,
		   const std::string &id);

	//! Invoked by parse()
	virtual void set_point_size(double)=0;
	//! Invoked by parse()
	virtual void set_scaled_size(double)=0;
	//! Invoked by parse()
	virtual void scale(double)=0;
	//! Invoked by parse()
	virtual void set_from(const std::string &)=0;
	//! Invoked by parse()
	virtual void set_family(const std::string &)=0;
	//! Invoked by parse()
	virtual void set_foundry(const std::string &)=0;
	//! Invoked by parse()
	virtual void set_style(const std::string &)=0;
	//! Invoked by parse()
	virtual void set_weight(const std::string &)=0;
	//! Invoked by parse()
	virtual void set_spacing(const std::string &)=0;
	//! Invoked by parse()
	virtual void set_slant(const std::string &)=0;
	//! Invoked by parse()
	virtual void set_width(const std::string &)=0;
};


#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
