/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_background_color_element_H
#define x_w_impl_background_color_element_H

#include <x/w/impl/background_color_elementfwd.H>
#include <x/w/impl/background_color.H>
#include <x/w/impl/element.H>

LIBCXXW_NAMESPACE_START

inline const background_color background_color_element_implObj
::get(ONLY IN_THREAD)
{
	return color;
}

//! Destructor
template<typename tag>
background_color_element<tag>::~background_color_element()=default;

//! If the widget is visible, invoke do_update().

template<typename tag>
void background_color_element<tag>
::update(ONLY IN_THREAD,
	 const background_color &new_color)
{
	this->do_update(IN_THREAD, new_color,
			this->background_color_element_impl());
}

template<typename tag>
void background_color_element<tag>
::set_background_color_for_element(ONLY IN_THREAD)
{
	auto &e=this->background_color_element_impl();

	if (!e.data(IN_THREAD).logical_inherited_visibility)
		return;

	background_color_element_implObj
		::set_background_color_for_element(IN_THREAD, e);
}

//! Constructor

//! The first parameter is the initial background_color, the remaining
//! parameters get forwarded to the parent class's constructor.

template<typename element_impl, typename tag>
template<typename ...Args>
inline background_color_1elementObj<element_impl, tag>
::background_color_1elementObj(const background_color &color,
			       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  background_color_element<tag>(color)
{
}

//! Constructor

//! The first parameter is the initial background_color, the remaining
//! parameters get forwarded to the parent class's constructor.

template<typename element_impl, typename tag>
template<typename ...Args>
inline background_color_1elementObj<element_impl, tag>
::background_color_1elementObj(const color_arg &color,
			       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  background_color_element<tag>(this->create_background_color(color))
{
}

template<typename element_impl, typename tag>
background_color_1elementObj<element_impl, tag>::~background_color_1elementObj()
=default;

//! Widget has been initialized

//! Invoke the parent class's initialize(), then invoke theme_updated() for
//! the background_color, in case the theme has changed in the meantime.
template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>::initialize(ONLY IN_THREAD)
{
	background_color_element<tag>::set_background_color_for_element
		(IN_THREAD);

	element_impl::initialize(IN_THREAD);
}

//! New theme

//! Invoke the parent class's theme_updated(), then invoke theme_updated() for
//! the background_color.

template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	background_color_element<tag>::theme_updated(IN_THREAD, new_theme,
						     *this);
	element_impl::theme_updated(IN_THREAD, new_theme);
}


//! Override process_updated_position.

//! If the widget is visible, call background_color_size().

template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>
::process_updated_position(ONLY IN_THREAD)
{
	background_color_element<tag>::set_background_color_for_element
		(IN_THREAD);
	element_impl::process_updated_position(IN_THREAD);
}

//! Override invoke_element_state_updates

//! If becoming visible, call set_background_color_for_element().
//! This makes sure we do this work before the display window
//! becomes visible.

template<typename element_impl, typename tag>
void background_color_1elementObj<element_impl, tag>
::invoke_element_state_updates(ONLY IN_THREAD,
			       element_state::state_update_t s)
{
	if (s == element_state::before_showing)
	{
		background_color_element<tag> &base=*this;

		base.background_color_element_implObj
			::set_background_color_for_element(IN_THREAD, *this);
	}

	element_impl::invoke_element_state_updates(IN_THREAD, s);
}

template<typename element_impl, typename tag>
elementObj::implObj &background_color_1elementObj<element_impl, tag>
::background_color_element_impl()
{
	return *this;
}

LIBCXXW_NAMESPACE_END

#endif
