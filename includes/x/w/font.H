/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_font_H
#define x_w_font_H

#include <string>
#include <string_view>
#include <map>
#include <x/w/fontfwd.H>

LIBCXXW_NAMESPACE_START

//! A font specification.

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

	// Map of predefined int values and the corresponding localized string.

	typedef std::map<int, std::string> values_t;

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

	//! Return a list of all predefined weights
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
};

//! Overload << operator

std::ostream &operator<<(std::ostream &, const font &);

LIBCXXW_NAMESPACE_END

#endif
