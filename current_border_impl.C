/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/pictformat.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/border_impl.H"
#include "screen.H"
#include "defaulttheme.H"
#include "x/w/impl/background_color.H"
#include <variant>
#include <functional>
#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

//! Convert a border_arg to a border_infomm.

static inline const border_infomm &
get_border_infomm(const border_arg &arg,
		  const const_defaulttheme &theme)
{
	return std::visit(visitor{
			[&]
			(const border_infomm &info) -> const border_infomm &
			{
				return info; // It already is.
			},
			[&]
			(const std::string &n) -> const border_infomm &
			{
				// Theme reference.

				return theme->get_theme_border(n);
			}}, arg);
}

//! Now take a border_infomm and convert it to a border_info

static inline border_info
convert_to_border_info(const screen &s,
		       const const_pictformat &pf,
		       const const_defaulttheme &theme,
		       const border_infomm &mm)
{
	border_info info{create_new_background_color(s, pf, mm.color1)};

	if (mm.color2)
		info.color2=create_new_background_color(s, pf, *mm.color2);

	auto w=theme->get_theme_dim_t(mm.width, themedimaxis::width);
	auto h=theme->get_theme_dim_t(mm.height, themedimaxis::height);

	info.width=dim_t::truncate(w * mm.width_scale);
	info.height=dim_t::truncate(h * mm.height_scale);

	// Sanity check
	if (info.width == dim_t::infinite() || info.height == dim_t::infinite())
		info.width=info.height=0;

	auto radius_w=theme->get_theme_dim_t(mm.hradius, themedimaxis::width);
	auto radius_h=theme->get_theme_dim_t(mm.vradius, themedimaxis::height);

	// Sanity check
	if (radius_w == dim_t::infinite() ||
	    radius_h == dim_t::infinite())
		radius_w=radius_h=0;

	// A scaled radius can be 0 only if it's specified as 0 in millimeters,
	// otherwise it's at least one pixel.
	if (radius_w == 0 && mm.rounded)
		radius_w=1;

	if (radius_h == 0 && mm.rounded)
		radius_h=1;

	info.hradius=dim_t::truncate(radius_w * mm.hradius_scale);
	info.vradius=dim_t::truncate(radius_h * mm.vradius_scale);

	// If a radius was specified, the radius must be at least two pixels.

	if (radius_w <= 1 &&
	    std::visit(visitor
		       {[](double v)
			{
				return v > 0;
			},
			[](const std::string &s)
			{
				return !s.empty();
			}}, mm.hradius))
	{
		radius_w=2;
	}

	if (radius_h <= 1 &&
	    std::visit(visitor
		       {[](double v)
			{
				return v > 0;
			},
			[](const std::string &s)
			{
				return !s.empty();
			}}, mm.vradius))
	{
		radius_h=2;
	}

	// Aspect ratio is expected to be the same horizontally and vertically.
	//
	// But, to compute one value for dash length, with the dashes being
	// used for horizontal and vertical pixels, we convert the millimeter
	// based dash value to both horizontal and vertical pixel counts, then
	// take their average.
	info.dashes.reserve(mm.dashes.size());

	for (const auto &orig_dash:mm.dashes)
	{
		auto dash_w=theme->compute_width(orig_dash);
		auto dash_h=theme->compute_height(orig_dash);

		if (dash_w == dim_t::infinite() ||
		    dash_h == dim_t::infinite())
			dash_w=dash_h=0;

		uint8_t dash=number<uint8_t, uint8_t>::truncate
			((dash_w+dash_h)/2);

		// Each dash must be at least one pixel in length.
		if (dash == 0)
			dash=1;

		info.dashes.push_back(dash);
	}

	return info;
}


// Convert a border_arg to a border_impl object.

static inline const_border_impl
border_impl_from_arg(const screen &s,
		     const const_pictformat &pf,
		     const border_arg &arg,
		     const const_defaulttheme &theme)
{
	auto b=border_impl::create(convert_to_border_info
				   (s, pf, theme,
				    get_border_infomm(arg, theme)));
	b->calculate();
	return b;
}

current_border_implObj
::current_border_implObj(const screen &s,
			 const const_pictformat &pf,
			 const border_arg &arg,
			 const current_theme_t::lock &lock)
	: s{s},
	  pf{pf},
	  arg{arg},
	  border_thread_only{border_impl_from_arg(s, pf, arg, *lock)}
{
}

current_border_implObj
::~current_border_implObj()=default;

void current_border_implObj
::current_theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	border(IN_THREAD)=border_impl_from_arg(s, pf, arg, new_theme);
}

bool current_border_implObj::no_border(ONLY IN_THREAD) const
{
	return border(IN_THREAD)->no_border();
}

LIBCXXW_NAMESPACE_END
