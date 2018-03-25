/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedim_element_minoverride_H
#define themedim_element_minoverride_H

#include "themedim_element_minoverridefwd.H"
#include "themedim_element.H"
#include "defaulttheme.H"
LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
inline themedim_element_minoverrideObj<element_impl>
::themedim_element_minoverrideObj(const dim_arg &width_override,
				  const dim_arg &height_override,
				  Args && ...args)
	: superclass_t(width_override,
		       &defaultthemeObj::get_theme_width_dim_t,
		       height_override,
		       &defaultthemeObj::get_theme_height_dim_t,
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
		const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
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
