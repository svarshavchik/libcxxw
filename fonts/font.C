/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/chrcasecmp.H>
#include <x/exception.H>
#include "x/w/font.H"
#include "messages.H"
#include <fontconfig/fontconfig.h>

#include <algorithm>

LIBCXXW_NAMESPACE_START

#ifndef DEFAULT_FONT
#define DEFAULT_FONT "liberation sans"
#endif

#ifndef DEFAULT_FONT_POINT_SIZE
#define DEFAULT_FONT_POINT_SIZE 12
#endif

font::font() : font(DEFAULT_FONT, DEFAULT_FONT_POINT_SIZE)
{
}

font::font(const std::string &family)
	: family(family)
{
}

font::font(const std::string &family, double point_size)
	: family(family), point_size(point_size)
{
}
font::~font()=default;

bool font::operator==(const font &o) const
{
	return foundry == o.foundry &&
		family == o.family &&
		weight == o.weight && slant == o.slant &&
		width == o.width &&
		style == o.style &&
		point_size == o.point_size &&
		spacing == o.spacing;
}

#define DECLARE_INT(ln,un,lv,uv) \
	const int font:: ln ## _ ## lv=FC_ ## un ## _ ## uv;

#define DECLARE_STR(ln,un,lv,uv) \
	{ # lv, &font:: ln ## _ ## lv },

#define WEIGHTS \
	DO(weight,WEIGHT,thin,THIN)		\
	DO(weight,WEIGHT,light,LIGHT)		\
	DO(weight,WEIGHT,normal,NORMAL)		\
	DO(weight,WEIGHT,medium,MEDIUM)		\
	DO(weight,WEIGHT,demibold,DEMIBOLD)	\
	DO(weight,WEIGHT,bold,BOLD)		\
	DO(weight,WEIGHT,heavy,HEAVY)		\
	DO(weight,WEIGHT,extrablack,EXTRABLACK)

#define DO DECLARE_INT

WEIGHTS

#undef DO

struct LIBCXX_HIDDEN CHARMAP { const char *n; const int *ptr; };

static int lookup(const struct CHARMAP *cm, const std::experimental::string_view &s)
{
	while (cm->n)
	{
		if (chrcasecmp::str_equal_to()(cm->n, s))
			return *cm->ptr;
		++cm;
	}
	return -1;
}

#define LOOKUP(cm,s,f) do {			\
	int v=lookup(cm, (s));			\
						\
	if (v >= 0)				\
		return f(v);			\
						\
	return *this;				\
	} while (0)

static const struct CHARMAP weights_str[] = {
#define DO DECLARE_STR

	WEIGHTS

	{nullptr, nullptr}
};

#undef DO

font &font::set_weight(const std::experimental::string_view &value)
{
	LOOKUP(weights_str, value, set_weight);
}

#define SLANTS \
	DO(slant,SLANT,roman,ROMAN)		\
	DO(slant,SLANT,italic,ITALIC)		\
	DO(slant,SLANT,oblique,OBLIQUE)

#define DO DECLARE_INT

SLANTS

#undef DO

static const struct CHARMAP slants_str[] = {
#define DO DECLARE_STR

	SLANTS

	{nullptr, nullptr}
};

#undef DO

font &font::set_slant(const std::experimental::string_view &value)
{
	LOOKUP(slants_str, value, set_slant);
}

#define WIDTHS \
	DO(width,WIDTH,condensed,CONDENSED)	\
	DO(width,WIDTH,normal,NORMAL)		\
	DO(width,WIDTH,expanded,EXPANDED)

#define DO DECLARE_INT

WIDTHS

#undef DO

static const struct CHARMAP widths_str[] = {
#define DO DECLARE_STR

	WIDTHS

	{nullptr, nullptr}
};

#undef DO

font &font::set_width(const std::experimental::string_view &value)
{
	LOOKUP(widths_str, value, set_width);
}

const int font::spacing_proportional=FC_PROPORTIONAL;
const int font::spacing_dual_width=FC_DUAL;
const int font::spacing_monospace=FC_MONO;
const int font::spacing_charcell=FC_CHARCELL;

static const struct CHARMAP spacings_str[] = {

	{"proportional", &font::spacing_proportional},
	{"dual",         &font::spacing_dual_width},
	{"monospace",    &font::spacing_monospace},
	{"charcell",     &font::spacing_charcell},

	{nullptr, nullptr}
};

#undef DO

font &font::set_spacing(const std::experimental::string_view &value)
{
	LOOKUP(spacings_str, value, set_spacing);
}

font &font::scale(unsigned numerator,
					unsigned denominator)
{
	if (point_size <= 0)
		throw EXCEPTION(_("Font size to scale() has not been set."));

	point_size=point_size*numerator/denominator;

	return *this;
}

font &font::scale(double ratio)
{
	if (point_size <= 0)
		throw EXCEPTION(_("Font size to scale() has not been set."));

	point_size *= ratio;

	return *this;
}

/////////////////////////////////////////////////////////////////////////

font operator"" _font(const char *str, size_t s)
{
	font f;

	const char *end=str+s;

	const char *beg=std::find(str, end, ';');

	if (beg != str)
		f.family=std::string(str, beg);

	chrcasecmp::str_equal_to cmpi;

	const auto eof=std::istringstream::traits_type::eof();

	while (beg != end)
	{
		auto p=++beg;
		beg=std::find(beg, end, ';');

		std::string setting{p, beg};

		if (setting.empty()) continue;

		auto equals=setting.find('=');

		if (equals == setting.npos)
			throw EXCEPTION(_("Invalid font specification"));

		std::string name=setting.substr(0, equals);
		std::string value=setting.substr(equals+1);

		if (cmpi(name, "point_size"))
		{
			double v;

			std::istringstream i{value};

			i >> v;

			if (!i.fail() || i.get() == eof)
				f.set_point_size(v);
		}

		if (cmpi(name, "scale"))
		{
			double v;

			std::istringstream i{value};

			i >> v;

			if (!i.fail() || i.get() == eof)
				f.scale(v);
		}

		if (cmpi(name, "weight"))
			f.set_weight(value);

		if (cmpi(name, "slant"))
			f.set_slant(value);

		if (cmpi(name, "width"))
			f.set_width(value);

		if (cmpi(name, "style"))
			f.set_style(value);

		if (cmpi(name, "foundry"))
			f.set_foundry(value);

		if (cmpi(name, "spacing"))
			f.set_spacing(value);
	}

	return f;
}
LIBCXXW_NAMESPACE_END