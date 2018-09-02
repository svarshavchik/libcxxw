/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_dim_element_h
#define listcontainer_dim_element_h

#include "x/w/impl/themedim_element.H"
#include "x/w/impl/themedimfwd.H"
#include "listlayoutmanager/listcontainer_dim_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
inline listcontainer_dim_elementObj<element_impl>
::listcontainer_dim_elementObj(const new_listlayoutmanager &style,
			       Args && ...args)
	: superclass_t{style.v_padding,
		       themedimaxis::height,
		       style.h_padding,
		       themedimaxis::width,
		       std::forward<Args>(args)...},
	  listcontainer_cell_paddingsObj{style}
{
}

//! Destructor
template<typename element_impl>
listcontainer_dim_elementObj<element_impl>::~listcontainer_dim_elementObj()
=default;

//! Calculate the size in pixels.
template<typename element_impl>
void listcontainer_dim_elementObj<element_impl>
::initialize(ONLY IN_THREAD)
{
	listcontainer_cell_paddingsObj::initialize(IN_THREAD, *this);
	superclass_t::initialize(IN_THREAD);
}

//! Recalculate the size in pixels.
template<typename element_impl>
void listcontainer_dim_elementObj<element_impl>
::theme_updated(ONLY IN_THREAD,
		const defaulttheme &new_theme)
{
	listcontainer_cell_paddingsObj::theme_updated(IN_THREAD, new_theme);
	superclass_t::theme_updated(IN_THREAD, new_theme);
}


//! Return the vertical padding themedim

template<typename element_impl>
dim_t listcontainer_dim_elementObj<element_impl>::list_v_padding(ONLY IN_THREAD)
	const
{
	return themedim_element<listcontainer_dim_v>::pixels(IN_THREAD);
}

//! Return the left & right padding for a cell.

template<typename element_impl>
std::tuple<dim_t, dim_t> listcontainer_dim_elementObj<element_impl>
::get_paddings(ONLY IN_THREAD, size_t n) const
{
	return listcontainer_cell_paddingsObj
		::get_paddings(IN_THREAD, n,
			       themedim_element<listcontainer_dim_h>
			       ::pixels(IN_THREAD));
}

LIBCXXW_NAMESPACE_END

#endif
