/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/chrcasecmp.H>
#include <x/exception.H>
#include <x/strtok.H>
#include <x/visitor.H>
#include <x/imbue.H>
#include "x/w/font.H"
#include "messages.H"
#include <fontconfig/fontconfig.h>

#include <algorithm>
#include <iostream>
#include <cstring>
#include <charconv>

LIBCXXW_NAMESPACE_START

#ifndef DEFAULT_FONT
#define DEFAULT_FONT "liberation sans"
#endif

#ifndef DEFAULT_FONT_POINT_SIZE
#define DEFAULT_FONT_POINT_SIZE 12
#endif

font::font() : font{DEFAULT_FONT, DEFAULT_FONT_POINT_SIZE}
{
}

font::font(const std::string &family_arg)
	: font(DEFAULT_FONT, DEFAULT_FONT_POINT_SIZE)
{
	size_t p=family_arg.find(';');

	if (p == family_arg.npos)
		family=family_arg;
	else
		operator+=(family_arg);
}

font::font(const std::string &family, double point_size)
	: family{family}, point_size{point_size}
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

#define DECLARE_INT(ln,un,lv,uv,s)		\
	const int font:: ln ## _ ## lv=FC_ ## un ## _ ## uv;

#define DECLARE_STR(ln,un,lv,uv,s)		\
	{ # lv, &font:: ln ## _ ## lv },

#define DECLARE_MAP(ln,un,lv,uv,s)		\
	{ ln ## _ ## lv, # lv,  s},

#define WEIGHTS							\
	DO(weight,WEIGHT,thin,THIN,_("Thin"))			\
	DO(weight,WEIGHT,light,LIGHT,_("Light"))		\
	DO(weight,WEIGHT,normal,NORMAL,_("Normal"))		\
	DO(weight,WEIGHT,medium,MEDIUM,_("Medium"))		\
	DO(weight,WEIGHT,demibold,DEMIBOLD,_("Demibold"))	\
	DO(weight,WEIGHT,bold,BOLD,_("Bold"))			\
	DO(weight,WEIGHT,heavy,HEAVY,_("Heavy"))		\
	DO(weight,WEIGHT,extrablack,EXTRABLACK,_("Extra Black"))

#define DO DECLARE_INT

WEIGHTS

#undef DO

struct LIBCXX_HIDDEN CHARMAP { const char *n; const int *ptr; };

static int lookup(const struct CHARMAP *cm, const std::string_view &s)
{
	int v;

	// We will accept an integer value literal, here.

	auto b=s.data();
	auto e=s.data()+s.size();

	auto ret=std::from_chars(b, e, v);

	if (ret.ec == std::errc{} && ret.ptr == e && v >= 0)
		return v;

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

font &font::set_weight(const std::string_view &value)
{
	LOOKUP(weights_str, value, set_weight);
}

#define DO DECLARE_MAP

font::values_t font::standard_weights()
{
	return { WEIGHTS };
}

#undef DO

#define SLANTS \
	DO(slant,SLANT,roman,ROMAN,_("Roman"))		\
	DO(slant,SLANT,italic,ITALIC,_("Italic"))	\
	DO(slant,SLANT,oblique,OBLIQUE,_("Oblique"))

#define DO DECLARE_INT

SLANTS

#undef DO

static const struct CHARMAP slants_str[] = {
#define DO DECLARE_STR

	SLANTS

	{nullptr, nullptr}
};

#undef DO

font &font::set_slant(const std::string_view &value)
{
	LOOKUP(slants_str, value, set_slant);
}

#define DO DECLARE_MAP

font::values_t font::standard_slants()
{
	return { SLANTS };
}

#undef DO

#define WIDTHS \
	DO(width,WIDTH,condensed,CONDENSED,_("Condensed"))	\
	DO(width,WIDTH,normal,NORMAL,_("Normal"))		\
	DO(width,WIDTH,expanded,EXPANDED,_("Expanded"))

#define DO DECLARE_INT

WIDTHS

#undef DO

static const struct CHARMAP widths_str[] = {
#define DO DECLARE_STR

	WIDTHS

	{nullptr, nullptr}
};

#undef DO

font &font::set_width(const std::string_view &value)
{
	LOOKUP(widths_str, value, set_width);
}

#define DO DECLARE_MAP

font::values_t font::standard_widths()
{
	return { WIDTHS };
}

#undef DO

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

font &font::set_spacing(const std::string_view &value)
{
	LOOKUP(spacings_str, value, set_spacing);
}

font::values_t font::standard_spacings()
{
	return {
		{ spacing_proportional, "proportional", _("Proportional")},
		{ spacing_dual_width, "dual", _("Dual Width")},
		{ spacing_monospace, "monospace", _("Monospaced")},
		{ spacing_charcell, "charcell", _("Character Cell")},
			};
}

std::vector<unsigned> font::standard_point_sizes()
{
	return {8, 10, 12, 14, 18, 24};

}

font &font::scale(unsigned numerator,
		  unsigned denominator)
{
	if (point_size <= 0 && scaled_size <= 0)
		throw EXCEPTION(_("Font size to scale() has not been set."));

	if (point_size > 0)
		point_size=point_size*numerator/denominator;

	if (scaled_size > 0)
		point_size=scaled_size*numerator/denominator;

	return *this;
}

font &font::scale(double ratio)
{
	if (ratio < 0)
		throw EXCEPTION("Invalid scale() ratio.");

	if (point_size <= 0 && scaled_size <= 0)
		throw EXCEPTION(_("Font size to scale() has not been set."));

	if (point_size > 0)
		point_size *= ratio;

	if (scaled_size > 0)
		scaled_size *= ratio;

	return *this;
}

/////////////////////////////////////////////////////////////////////////

font operator"" _font(const char *str, size_t s)
{
	font f;

	f += std::string_view{str, s};

	return f;
}

font font::operator+(const std::string_view &s) const
{
	font f{*this};

	f += s;

	return f;
}

static const char seps[] = ";,";

font &font::operator+=(const std::string_view &s)
{
	const char *beg=s.begin();
	const char *end=s.end();

	chrcasecmp::str_equal_to cmpi;

	while (beg != end)
	{
		if (strchr(seps, *beg))
		{
			++beg;
			continue;
		}

		auto p=beg;
		beg=std::find_if(beg, end,
				 []
				 (char c)
				 {
					 return strchr(seps, c) != 0;
				 });

		std::string setting{p, beg};

		if (setting.empty()) continue;

		auto equals=setting.find('=');

		if (equals == setting.npos)
		{
			auto s=trim(setting);

			if (!s.empty())
				family=s;
			continue;
		}

		std::string name=trim(setting.substr(0, equals));
		std::string value=trim(setting.substr(equals+1));

		if (value.empty())
			continue;

		if (cmpi(name, "point_size"))
		{
			double v;

			auto ret=std::from_chars(&value[0],
						 &value[0]+value.size(), v);

			if (ret.ec == std::errc{} &&
			    *ret.ptr == 0)
				set_point_size(v);
		}

		if (cmpi(name, "scale"))
		{
			double v;

			auto ret=std::from_chars(&value[0],
						 &value[0]+value.size(), v);

			if (ret.ec == std::errc{} &&
			    *ret.ptr == 0)
				scale(v);
		}

		if (cmpi(name, "weight"))
			set_weight(value);

		if (cmpi(name, "slant"))
			set_slant(value);

		if (cmpi(name, "width"))
			set_width(value);

		if (cmpi(name, "style"))
			set_style(value);

		if (cmpi(name, "family"))
			family=value;

		if (cmpi(name, "foundry"))
			set_foundry(value);

		if (cmpi(name, "spacing"))
			set_spacing(value);
	}

	return *this;
}

font::operator std::string() const
{
	std::ostringstream o;

	// TODO -- when to_chars implements double.

	imbue im{locale::base::c(), o};

	const char *sep="";
	const char *string_sep="; ";
	auto v=visitor{
		[&](const char *name, const std::string &v)
		{
			if (v.empty())
				return;

			o << sep << name << "=" << v;
			sep=string_sep;
		},
		[&](const char *name, int v, const struct CHARMAP *map)
		{
			if (v < 0)
				return;

			o << sep << name << "=";

			while (map->n)
			{
				if (*map->ptr == v)
					break;
				++map;
			}

			// If there's a predefined label, use it.
			if (map->n)
			{
				o << map->n;
			}
			else
			{
				char buf[40];

				auto ret=std::to_chars(buf, buf+sizeof(buf)-1,
						       v);
				if (ret.ec == std::errc{})
				{
					*ret.ptr=0;
				}

				o << buf;
			}
			sep=", ";
		},
		[&](const char *name, double v)
		{
			if (v <= 0)
				return;

			o << sep << name << "=" << v;
			sep=", ";
		}};

	v("family", family);
	string_sep=", ";
	v("foundry", foundry);
	v("weight", weight, weights_str);
	v("slant", slant, slants_str);
	v("width", width, widths_str);
	v("spacing", spacing, spacings_str);
	v("style", style);
	v("point_size", point_size);
	v("pixel_size", pixel_size);

	return o.str();
}

//! Overload << operator

std::ostream &operator<<(std::ostream &o, const font &f)
{
	return o << (std::string)f;
}

LIBCXXW_NAMESPACE_END
