/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedim_axis_element_H
#define x_w_impl_themedim_axis_element_H

#include <x/w/impl/themedim_axis_elementfwd.H>
#include <x/w/impl/themedim_element.H>
#include <x/w/impl/themedimfwd.H>

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
	: superclass_t(dims.minimum, themedimaxis::width,
		       dims.preferred, themedimaxis::width,
		       dims.maximum, themedimaxis::width,
		       std::forward<Args>(args)...)
{
}

//! Destructor

template<typename base>
themedim_axis_widthObj<base>::~themedim_axis_widthObj()=default;

//! Return the current dimensions, as a metrics::axis

template<typename base>
metrics::axis themedim_axis_widthObj<base>::get_width_axis(ONLY IN_THREAD)
	const
{
	dim_t min=themedim_element<themedim_width_min>::pixels(IN_THREAD);
	dim_t pref=themedim_element<themedim_width_pref>::pixels(IN_THREAD);
	dim_t max=themedim_element<themedim_width_max>::pixels(IN_THREAD);

	if (pref < min) pref=min;
	if (max < pref) max=pref;
	return {min, pref, max};
}

//! Update the current dimensions

template<typename base>
void themedim_axis_widthObj<base>
::update_width_axis(ONLY IN_THREAD,
		    const dim_axis_arg &arg,
		    const defaulttheme &current_theme)
{
	themedim_element<themedim_width_min>::update(IN_THREAD, arg.minimum,
						     current_theme);
	themedim_element<themedim_width_pref>::update(IN_THREAD, arg.preferred,
						      current_theme);
	themedim_element<themedim_width_max>::update(IN_THREAD, arg.maximum,
						     current_theme);
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
	: superclass_t(dims.minimum, themedimaxis::height,
		       dims.preferred, themedimaxis::height,
		       dims.maximum, themedimaxis::height,
		       std::forward<Args>(args)...)
{
}

//! Destructor

template<typename base>
themedim_axis_heightObj<base>::~themedim_axis_heightObj()=default;

//! Return the current dimensions, as a metrics::axis

template<typename base>
metrics::axis themedim_axis_heightObj<base>::get_height_axis(ONLY IN_THREAD)
	const
{
	dim_t min=themedim_element<themedim_height_min>::pixels(IN_THREAD);
	dim_t pref=themedim_element<themedim_height_pref>::pixels(IN_THREAD);
	dim_t max=themedim_element<themedim_height_max>::pixels(IN_THREAD);

	if (pref < min) pref=min;
	if (max < pref) max=pref;
	return {min, pref, max};
}

//! Update the current dimensions

template<typename base>
void themedim_axis_heightObj<base>
::update_height_axis(ONLY IN_THREAD,
		     const dim_axis_arg &arg,
		     const defaulttheme &current_theme)
{
	themedim_element<themedim_height_min>::update(IN_THREAD, arg.minimum,
						     current_theme);
	themedim_element<themedim_height_pref>::update(IN_THREAD, arg.preferred,
						      current_theme);
	themedim_element<themedim_height_max>::update(IN_THREAD, arg.maximum,
						      current_theme);
}


LIBCXXW_NAMESPACE_END

#endif
