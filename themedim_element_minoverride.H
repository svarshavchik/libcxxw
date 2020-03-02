/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedim_element_minoverride_H
#define themedim_element_minoverride_H

#include "themedim_element_minoverridefwd.H"
#include "x/w/impl/themedim_element.H"
#include "generic_window_handler.H"
#include "defaulttheme.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
inline themedim_element_minoverrideObj<element_impl>
::themedim_element_minoverrideObj(const dim_arg &width_override,
				  const dim_arg &height_override,
				  Args && ...args)
	: superclass_t(width_override,
		       themedimaxis::width,
		       height_override,
		       themedimaxis::height,
		       std::forward<Args>(args)...)
{
}


//! Destructor

template<typename element_impl>
themedim_element_minoverrideObj<element_impl>
::~themedim_element_minoverrideObj()=default;

//! Override initialize()

//! Invoke compute_minimum_override()
template<typename element_impl>
void themedim_element_minoverrideObj<element_impl>
::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	compute_minimum_override(IN_THREAD);
}

//! Override theme_updated

//! Invoke compute_minimum_override()
template<typename element_impl>
void themedim_element_minoverrideObj<element_impl>
::theme_updated(ONLY IN_THREAD,
		const const_defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	compute_minimum_override(IN_THREAD);
}

//! Update the minimum overrides

template<typename element_impl>
void themedim_element_minoverrideObj<element_impl>
::update_minimum_override(ONLY IN_THREAD,
			  const dim_arg &width_override,
			  const dim_arg &height_override)
{
	elementObj::implObj *me=this;

	auto theme=me->get_window_handler()
		.get_screen()->impl->current_theme.get();

	themedim_element<themedim_element_override_width_tag>
		::update(IN_THREAD, width_override, theme);

	themedim_element<themedim_element_override_height_tag>
		::update(IN_THREAD, height_override, theme);
	compute_minimum_override(IN_THREAD);
}

template<typename element_impl>
void themedim_element_minoverrideObj<element_impl>
::compute_minimum_override(ONLY IN_THREAD)
{
	this->get_horizvert(IN_THREAD)->set_minimum_override
		(IN_THREAD,
		 themedim_element<themedim_element_override_width_tag>
		 ::pixels(IN_THREAD),
		 themedim_element<themedim_element_override_height_tag>
		 ::pixels(IN_THREAD));
}

LIBCXXW_NAMESPACE_END

#endif
