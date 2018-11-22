/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainer_impl_element_h
#define peepholed_listcontainer_impl_element_h

#include "peepholed_listcontainer_impl_elementfwd.H"
#include "listlayoutmanager/listcontainer_dim_element.H"
#include "x/w/impl/themedim.H"
#include "x/w/impl/themedim_axis_element.H"
#include "x/w/impl/metrics_horizvert.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START


//! Constructor

template<typename element_impl>
template<typename ...Args>
peepholed_listcontainer_impl_elementObj<element_impl>
::peepholed_listcontainer_impl_elementObj(const new_listlayoutmanager &style,
					  Args && ...args)
	: superclass_t{style, std::forward<Args>(args)...}
{
}

//! Destructor

template<typename element_impl>
peepholed_listcontainer_impl_elementObj<element_impl>
::~peepholed_listcontainer_impl_elementObj()=default;

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

LIBCXXW_NAMESPACE_END

#endif
