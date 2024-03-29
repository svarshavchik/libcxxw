/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_font_H
#define x_w_font_H

#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include <x/w/fontfwd.H>

LIBCXXW_NAMESPACE_START

//! A font specification.

//! \code
//! INSERT_LIBX_NAMESPACE::w::font f{"arial"};
//! \endcode
//!
//! Construct a font specification for the "Arial" font family.

//! \code
//! INSERT_LIBX_NAMESPACE::w::font f{"arial", 10.0};
//! \endcode
//!
//! Construct a font specification for the "Arial" font family, 10 points.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::font f{"liberation sans; point_size=10, slant=italic"};
//!
//! INSERT_LIBX_NAMESPACE::w::font f{"family=liberation sans, point_size=10, slant=italic"};
//! \endcode
//!
//! The constructor that takes a single text parameter parses its string
//! further in order to extract a more detailed font specification as per
//! the examples above:
//!
//! - either commas or semicolons (together with surrounding namespaces
//! gets recognized as delimiter for a list of setting
//!
//! - each setting takes the form of a "parameter=value"
//!
//! - a setting without an equals sign is interpreted as value for the
//!  "family" setting.
//!
//! Otherwise the parameters and values are:
//!
//! - "foundry=<n>" - font foundry
//! - "family=<n>" - font family
//! - "weight=n" - same as calling set_weight()
//! - "slant=n" - same as calling set_slant()
//! - "width=n" - same as calling set_width()
//! - "style=n" - same as calling set_style()
//! - "scale=n" - same as calling scale()
//! - "point_size=n" - same as calling set_point_size()
//! - "pixel_size=n" - same as calling pixel_size()

struct font {

	//! Font foundry

	std::string foundry;

	//! Font family

	std::string family;

	//! Predefined weight
	int weight= -1;

	//! Font slant
	int slant=-1;

	//! Predefined font width

	int width=-1;

	//! Font style

	std::string style;

	//! Point size

	double point_size=0;

	//! Scaled size

	//! This is currently used by SXG files.
	double scaled_size=0;

	//! Pixel size

	double pixel_size=0;

	//! Font spacing

	int spacing=-1;

	//! Comparison operator
	bool operator==(const font &o) const;

	//! Constructor.

	font();

	//! Constructor.

	font(const std::string &family);

	//! Constructor.

	font(const std::string &family, double point_size);

	//! Destructor

	~font();

	//! A predefined int value, its label, and localized name.

	struct value_t {
		int value;
		const char *label;
		const char *description;
	};
	//! Map of predefined int values and the corresponding localized string.

	//! Container returned by several standard_() methods. The int
	//! values are in sorted order.
	typedef std::vector<value_t> values_t;

	/////////////////////////////////////////////////////////////////////

	//! Predefined font weight()
	static const int weight_thin;
	//! Predefined font weight()
	static const int weight_light;
	//! Predefined font weight()
	static const int weight_normal;
	//! Predefined font weight()
	static const int weight_medium;
	//! Predefined font weight()
	static const int weight_demibold;
	//! Predefined font weight()
	static const int weight_bold;
	//! Predefined font weight()
	static const int weight_heavy;
	//! Predefined font weight()
	static const int weight_extrablack;

	//! Request a specific font

	//! Specifies FC_WEIGHT
	inline font &set_weight(int value)
	{
		weight=value;
		return *this;
	}

	//! Specifies weight as a character string
	font &set_weight(const std::string_view &value);

	//! Return a list of all predefined weights
	static values_t standard_weights();

	//////////////////////////////////////////////////////////////////////

	//! Predefined font slant()
	static const int slant_roman;
	//! Predefined font slant()
	static const int slant_italic;
	//! Predefined font slant()
	static const int slant_oblique;

	//! Request a specific font

	//! Specifies FC_SLANT
	inline font &set_slant(int value)
	{
		slant=value;
		return *this;
	}

	//! Specify slant as a character string
	font &set_slant(const std::string_view &value);

	//! Return a list of all predefined slants
	static values_t standard_slants();

	//////////////////////////////////////////////////////////////////////


	//! Predefined font width()
	static const int width_condensed;
	//! Predefined font width()
	static const int width_normal;
	//! Predefined font width()
	static const int width_expanded;

	//! Request a specific font

	//! Specifies FC_WIDTH

	inline font &set_width(int value)
	{
		width=value;
		return *this;
	}

	//! Specify width as a character string
	font &set_width(const std::string_view &value);

	//! Return a list of all predefined widths
	static values_t standard_widths();

	///////////////////////////////////////////////////////////////////////

	//! Request a specific font

	//! Specifies FC_STYLE

	inline font &set_style(const std::string &arg)
	{
		style=arg;
		return *this;
	}

	///////////////////////////////////////////////////////////////////////

	//! Request a specific font

	//! Specifies FC_FAMILY

	inline font &set_family(const std::string &arg)
	{
		family=arg;
		return *this;
	}

	///////////////////////////////////////////////////////////////////////

	//! Request a specific font

	//! Specifies FC_STYLE

	inline font &set_foundry(const std::string &arg)
	{
		foundry=arg;
		return *this;
	}

	///////////////////////////////////////////////////////////////////////

	//! Predefined font spacing()
	static const int spacing_proportional;
	//! Predefined font spacing()
	static const int spacing_dual_width;
	//! Predefined font spacing()
	static const int spacing_monospace;
	//! Predefined font spacing()
	static const int spacing_charcell;

	//! Request a specific font

	//! Specifies FC_STYLE
	inline font &set_spacing(int value)
	{
		spacing=value;
		return *this;
	}

	//! Specify spacing as a character string
	font &set_spacing(const std::string_view &value);

	//! Return a list of all predefined spacings
	static values_t standard_spacings();

	/////////////////////////////////////////////////////////////////////

	//! Request a specific font

	//! Specifies the size of the font in points. Uses the drawable's
	//! resolution to compute the requested pixel size, that gets rounded
	//! off.

	inline font &set_point_size(double arg)
	{
		point_size=arg;

		return *this;
	}

	//! Return standard point sizes

	//! The returned vector is in sorted order
	static std::vector<unsigned> standard_point_sizes();

	//! Request a specific font

	//! Specifies the size of the font on some arbitrary scale.
	//! This is currently used only by SXG files.

	inline font &set_scaled_size(double arg)
	{
		scaled_size=arg;

		return *this;
	}

	//! Request a specific font

	//! Specifies the size of the font in pixels.
	//! This is currently used by SXG files.

	inline font &set_pixel_size(double arg)
	{
		pixel_size=arg;

		return *this;
	}

	//! Scale pixel size.

	//! Multiplies point_size and scaled_size by numerator/denominator.

	font &scale(unsigned numerator, unsigned denominator);

	//! Scale the pixel size.

	//! Multiplies point_size and scaled_size by the ratio.
	font &scale(double ratio);

	//! Provide a descriptive string for this font.

	//! Does include scaled_size, which is for internal use only.

	operator std::string() const;

	//! Create a new font by modifying this font.

	//! The string is in the same format as the constructor's parameter,
	//! and the additional properties from the string get applied to
	//! this font specification.

	font operator+(const std::string_view &s) const;

	//! Modify this font

	//! \see operator+
	font &operator+=(const std::string_view &);
};

//! Overload << operator

std::ostream &operator<<(std::ostream &, const font &);

LIBCXXW_NAMESPACE_END

#endif
