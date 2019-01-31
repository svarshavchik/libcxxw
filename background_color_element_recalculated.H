/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_color_element_recalculated_H
#define background_color_element_recalculated_H

#include "background_color_element_recalculatedfwd.H"

LIBCXXW_NAMESPACE_START


template<typename element_impl>
background_color_element_recalculatedObj<element_impl>
::~background_color_element_recalculatedObj()=default;

template<typename element_impl>
void background_color_element_recalculatedObj<element_impl>
::initialize(ONLY IN_THREAD)
{
	recalculated_flag=false;
	element_impl::initialize(IN_THREAD);
	if (recalculated_flag)
		all_background_colors_were_recalculated(IN_THREAD);
}

template<typename element_impl>
void background_color_element_recalculatedObj<element_impl>
::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	recalculated_flag=false;
	element_impl::theme_updated(IN_THREAD, new_theme);
	if (recalculated_flag)
		all_background_colors_were_recalculated(IN_THREAD);
}

template<typename element_impl>
void background_color_element_recalculatedObj<element_impl>
::process_updated_position(ONLY IN_THREAD)
{
	recalculated_flag=false;
	element_impl::process_updated_position(IN_THREAD);
	if (recalculated_flag)
		all_background_colors_were_recalculated(IN_THREAD);
}

template<typename element_impl>
void background_color_element_recalculatedObj<element_impl>
::invoke_element_state_updates(ONLY IN_THREAD,
			       element_state::state_update_t e)
{
	recalculated_flag=false;
	element_impl::invoke_element_state_updates(IN_THREAD, e);
	if (recalculated_flag)
		all_background_colors_were_recalculated(IN_THREAD);
}

//! Register the fact that a background color was recalculated.

template<typename element_impl>
void background_color_element_recalculatedObj<element_impl>
::background_color_was_recalculated(ONLY IN_THREAD)
{
	recalculated_flag=true;
}

LIBCXXW_NAMESPACE_END

#endif
