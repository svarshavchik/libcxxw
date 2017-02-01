/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/metrics/mmaxis.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

dim_t screenObj::implObj::compute_width(double millimeters)
{
	return compute_width(current_theme_t::lock(current_theme),
			     millimeters);
}

dim_t screenObj::implObj::compute_width(const current_theme_t::lock &lock,
					double millimeters)
{
	if (std::isnan(millimeters))
		return dim_t::infinite();

	auto scaled=std::round((*lock)->themescale * millimeters *
			       xcb_screen->width_in_pixels /
			       xcb_screen->width_in_millimeters);

	if (scaled > dim_t::infinite()-1)
		scaled=dim_t::infinite()-1;

	if (millimeters != 0 && scaled < 1)
		scaled=1;

	return dim_t::value_type(scaled);
}


dim_t screenObj::implObj::compute_height(double millimeters)
{
	return compute_height(current_theme_t::lock(current_theme),
			      millimeters);
}

dim_t screenObj::implObj::compute_height(const current_theme_t::lock &lock,
					 double millimeters)
{
	if (std::isnan(millimeters))
		return dim_t::infinite();

	auto scaled=std::round((*lock)->themescale * millimeters *
			       xcb_screen->height_in_pixels /
			       xcb_screen->height_in_millimeters);

	if (scaled > dim_t::infinite()-1)
		scaled=dim_t::infinite()-1;

	if (millimeters != 0 && scaled < 1)
		scaled=1;

	return dim_t::value_type(scaled);
}

metrics::axis screenObj::implObj::compute_width(const metrics::mmaxis &mm)
{
	current_theme_t::lock lock(current_theme);

	return {
		compute_width(lock, mm.minimum()),
		compute_width(lock, mm.preferred()),
		compute_width(lock, mm.maximum())
	       };
}

metrics::axis screenObj::implObj::compute_height(const metrics::mmaxis &mm)
{
	current_theme_t::lock lock(current_theme);

	return {
		compute_height(lock, mm.minimum()),
		compute_height(lock, mm.preferred()),
		compute_height(lock, mm.maximum())
	       };
}

LIBCXXW_NAMESPACE_END
