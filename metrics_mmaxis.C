/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/metrics/mmaxis.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START


double screenObj::implObj::compute_widthmm(const current_theme_t::lock &lock,
					   dim_t pixels)
{
	return dim_t::value_type(pixels)
		/ (*lock)->themescale
		* xcb_screen->width_in_millimeters
		/ xcb_screen->width_in_pixels;
}

metrics::axis screenObj::implObj::compute_width(const metrics::mmaxis &mm)
{
	current_theme_t::lock lock(current_theme);

	return {
		(*lock)->compute_width(mm.minimum()),
		(*lock)->compute_width(mm.preferred()),
		(*lock)->compute_width(mm.maximum())
	       };
}

metrics::axis screenObj::implObj::compute_height(const metrics::mmaxis &mm)
{
	current_theme_t::lock lock(current_theme);

	return {
		(*lock)->compute_height(mm.minimum()),
		(*lock)->compute_height(mm.preferred()),
		(*lock)->compute_height(mm.maximum())
	       };
}

LIBCXXW_NAMESPACE_END
