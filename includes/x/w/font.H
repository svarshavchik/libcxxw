/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_font_H
#define x_w_font_H

#include <string>
#include <experimental/string_view>
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
	font &set_weight(const std::experimental::string_view &value);

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
	font &set_slant(const std::experimental::string_view &value);

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
	font &set_width(const std::experimental::string_view &value);

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
	font &set_spacing(const std::experimental::string_view &value);

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

	//! Scale pixel size.

	//! Multiplies pixel size by numerator/denominator.
	//! Rounds off to the nearest pixel size.
	font &scale(unsigned numerator, unsigned denominator);

	//! Scale the pixel size.

	//! Multiplies pixel size by the ratio.
	//! Rounds off to the nearest pixel size.
	font &scale(double ratio);
};

LIBCXXW_NAMESPACE_END

#endif
