/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/border_impl.H"
#include "screen.H"
#include "defaulttheme.H"
#include "x/w/impl/background_color.H"
#include <variant>
#include <functional>
#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

static inline border_info
convert_to_border_info(const ref<screenObj::implObj> &s,
		       const defaulttheme &theme,
		       const border_infomm &mm)
{
	border_info info{s->create_background_color(mm.color1)};

	if (mm.color2)
		info.color2=s->create_background_color(*mm.color2);

	info.width=theme->compute_width(mm.width);
	info.height=theme->compute_height(mm.height);

	// Sanity check
	if (info.width == dim_t::infinite() || info.height == dim_t::infinite())
		info.width=info.height=0;

	auto radius_w=theme->compute_width(mm.radius);
	auto radius_h=theme->compute_height(mm.radius);

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

	// If a radius was specified, the radius must be at least two pixels.

	if (radius_w <= 1 && mm.radius)
		radius_w=2;

	if (radius_h <= 1 && mm.radius)
		radius_h=2;

	info.hradius=radius_w;
	info.vradius=radius_h;

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

static const_border_impl
border_impl_from_arg(const ref<screenObj::implObj> &screen,
		     const border_arg &arg,
		     const defaulttheme &theme)
{
	return std::visit(visitor{
			[&]
			(const border_infomm &info)
			{
				auto b=border_impl
					::create(convert_to_border_info
						 (screen, theme, info));
				b->calculate();
				return const_border_impl(b);
			},
			[&]
			(const std::string &n)
			{
				return theme->get_theme_border(n);
			}}, arg);
}

current_border_implObj
::current_border_implObj(const ref<screenObj::implObj> &screen,
			 const border_arg &arg)
	: current_border_implObj(screen, arg,
				 current_theme_t::lock(screen->current_theme))
{
}

current_border_implObj
::current_border_implObj(const ref<screenObj::implObj> &screen,
			 const border_arg &arg,
			 const current_theme_t::lock &lock)
	: screen(screen),
	  arg(arg),
	  current_theme_thread_only(*lock),
	  border_thread_only(border_impl_from_arg(screen, arg, *lock))
{
}

current_border_implObj
::~current_border_implObj()=default;

void current_border_implObj
::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	// This custom border object can be attached to multiple
	// border display elements. Go through the motions of
	// creating a new border object only the first time we're
	// called, here.

	auto &t=current_theme(IN_THREAD);

	if (new_theme == t)
		return;

	t=new_theme;

	border(IN_THREAD)=border_impl_from_arg(screen, arg, new_theme);

	initialized=false;
	initialize(IN_THREAD);
}

void current_border_implObj::initialize(ONLY IN_THREAD)
{
	if (initialized)
		return;
	initialized=true;

	border(IN_THREAD)->color1->initialize(IN_THREAD);

	if (border(IN_THREAD)->color2)
		border(IN_THREAD)->color2->initialize(IN_THREAD);
}

bool current_border_implObj::no_border(ONLY IN_THREAD) const
{
	return border(IN_THREAD)->no_border();
}

LIBCXXW_NAMESPACE_END
