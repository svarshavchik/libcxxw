/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedim_axis_element_H
#define themedim_axis_element_H

#include "themedim_axis_elementfwd.H"
#include "themedim_element.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

//! Constructor

//! The first template parameter initialized the themedim_elementObj
//! mixin for the three dimensions. The remaining parameters get forwarded
//! to the superclass template parameter.

template<typename base>
template<typename ...Args>
inline themedim_axis_widthObj<base>
::themedim_axis_widthObj(const dim_axis_arg &dims,
			 Args && ...args)
	: superclass_t(dims.minimum, &defaultthemeObj::get_theme_width_dim_t,
		       dims.preferred, &defaultthemeObj::get_theme_width_dim_t,
		       dims.maximum, &defaultthemeObj::get_theme_width_dim_t,
		       std::forward<Args>(args)...)
{
}

//! Destructor

template<typename base>
themedim_axis_widthObj<base>::~themedim_axis_widthObj()=default;

//! Return the current dimensions, as a metrics::axis

template<typename base>
metrics::axis themedim_axis_widthObj<base>::get_width_axis(IN_THREAD_ONLY)
	const
{
	dim_t min=themedim_element<themedim_width_min>::pixels(IN_THREAD);
	dim_t pref=themedim_element<themedim_width_pref>::pixels(IN_THREAD);
	dim_t max=themedim_element<themedim_width_max>::pixels(IN_THREAD);

	if (pref < min) pref=min;
	if (max < pref) max=pref;
	return {min, pref, max};
}

//! Constructor

//! The first template parameter initialized the themedim_elementObj
//! mixin for the three dimensions. The remaining parameters get forwarded
//! to the superclass template parameter.

template<typename base>
template<typename ...Args>
inline themedim_axis_heightObj<base>
::themedim_axis_heightObj(const dim_axis_arg &dims,
			  Args && ...args)
	: superclass_t(dims.minimum, &defaultthemeObj::get_theme_height_dim_t,
		       dims.preferred, &defaultthemeObj::get_theme_height_dim_t,
		       dims.maximum, &defaultthemeObj::get_theme_height_dim_t,
		       std::forward<Args>(args)...)
{
}

//! Destructor

template<typename base>
themedim_axis_heightObj<base>::~themedim_axis_heightObj()=default;

//! Return the current dimensions, as a metrics::axis

template<typename base>
metrics::axis themedim_axis_heightObj<base>::get_height_axis(IN_THREAD_ONLY)
	const
{
	dim_t min=themedim_element<themedim_height_min>::pixels(IN_THREAD);
	dim_t pref=themedim_element<themedim_height_pref>::pixels(IN_THREAD);
	dim_t max=themedim_element<themedim_height_max>::pixels(IN_THREAD);

	if (pref < min) pref=min;
	if (max < pref) max=pref;
	return {min, pref, max};
}


LIBCXXW_NAMESPACE_END

#endif
