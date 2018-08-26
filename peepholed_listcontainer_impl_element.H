/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainer_impl_element_h
#define peepholed_listcontainer_impl_element_h

#include "peepholed_listcontainer_impl_elementfwd.H"
#include "listlayoutmanager/listcontainer_dim_element.H"
#include "x/w/impl/theme_font_element.H"
#include "x/w/impl/themedim.H"
#include "x/w/impl/themedim_axis_element.H"
#include "x/w/impl/metrics_horizvert.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
peepholed_listcontainer_impl_element_baseObj<element_impl>
::peepholed_listcontainer_impl_element_baseObj(const new_listlayoutmanager
					       &style,
					       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  peepholed_listcontainerObj::implObj(style)
{
}

//! Destructor

template<typename element_impl>
peepholed_listcontainer_impl_element_baseObj<element_impl>
::~peepholed_listcontainer_impl_element_baseObj()=default;

//! Override initialize()

//! Calls the superclass's initialize() first, then
//! peepholed_listcontainerObj::implObj::initialize().

template<typename element_impl>
void peepholed_listcontainer_impl_element_baseObj<element_impl>
::initialize(ONLY IN_THREAD)
{
	element_impl::initialize(IN_THREAD);
	peepholed_listcontainerObj::implObj::initialize(IN_THREAD);
}

//! Override horizvert_updated()

//! Calls the superclass's horizvert_updated() first, then
//! peepholed_listcontainerObj::implObj::horizvert_updated().

template<typename element_impl>
void peepholed_listcontainer_impl_element_baseObj<element_impl>
::horizvert_updated(ONLY IN_THREAD)
{
	element_impl::horizvert_updated(IN_THREAD);
	peepholed_listcontainerObj::implObj::horizvert_updated(IN_THREAD);
}

//! Override theme_updated()

//! Calls the superclass's theme_updated() first, then
//! peepholed_listcontainerObj::implObj::theme_updated().

template<typename element_impl>
void peepholed_listcontainer_impl_element_baseObj<element_impl>
::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	element_impl::theme_updated(IN_THREAD, new_theme);
	peepholed_listcontainerObj::implObj::theme_updated(IN_THREAD,
							   new_theme);
}

//! Constructor

//! Constructs the superclass by:
//!
//! - passes the dim_axis_arg, if one gets specified by list style's height,
//!   else provides a default one, to the themedim_axis_elementObj mixin
//! template.
//!
//! - passes the name of the font for the theme_font_elementObj mixin
//! template.
//!
//! - passes the style for the listcontainer_dim_elementObj mixin template.
//!
//! - passes the style again for the peepholed_listcontainer_impl_element_baseObj
//! helper mixin template.
//!
//! - then forwarding the remaining arguments.

template<typename element_impl>
template<typename ...Args>
peepholed_listcontainer_impl_elementObj<element_impl>
::peepholed_listcontainer_impl_elementObj(const new_listlayoutmanager &style,
					  Args && ...args)
	: superclass_t{
	(std::holds_alternative<dim_axis_arg>(style.height_value)
		? std::get<dim_axis_arg>(style.height_value):
	 dim_axis_arg{}),
		style.list_font,
		style, style,
		std::forward<Args>(args)...}
{
}

//! Destructor

template<typename element_impl>
peepholed_listcontainer_impl_elementObj<element_impl>
::~peepholed_listcontainer_impl_elementObj()=default;

//! Implement list_reference_font().

//! Used by peepholed_listcontainerObj::implObj.

template<typename element_impl>
const theme_fontObj &peepholed_listcontainer_impl_elementObj<element_impl>
::list_reference_font() const
{
	return *this;
}

//! Implement get_pseudo_impl().

//! Used by peepholed_listcontainerObj::implObj.

template<typename element_impl>
listcontainer_pseudo_implObj &peepholed_listcontainer_impl_elementObj<element_impl>
::get_pseudo_impl()
{
	return *this;
}

//! Implement get_pseudo_impl().

//! Used by peepholed_listcontainerObj::implObj.

template<typename element_impl>
const listcontainer_pseudo_implObj &peepholed_listcontainer_impl_elementObj<element_impl>
::get_pseudo_impl() const
{
	return *this;
}

//! Implement

template<typename element_impl>
metrics::axis peepholed_listcontainer_impl_elementObj<element_impl>
::get_height_metrics(ONLY IN_THREAD) const
{
	return this->get_height_axis(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
