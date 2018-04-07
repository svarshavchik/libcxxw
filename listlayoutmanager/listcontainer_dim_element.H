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
			       style.left_padding,
			       themedimaxis::width,
			       style.inner_padding,
			       themedimaxis::width,
			       style.right_padding,
			       themedimaxis::width,

			       std::forward<Args>(args)...}
{
}

//! Destructor

template<typename element_impl>
listcontainer_dim_elementObj<element_impl>::~listcontainer_dim_elementObj()
=default;

//! Return the vertical padding themedim

template<typename element_impl>
const_themedim listcontainer_dim_elementObj<element_impl>::list_v_padding()
	const
{
	return themedim_element<listcontainer_dim_v>::getref();
}

//! Return the left padding themedim

template<typename element_impl>
const_themedim listcontainer_dim_elementObj<element_impl>::list_left_padding()
	const
{
	return themedim_element<listcontainer_dim_left>::getref();
}

//! Return the right padding themedim

template<typename element_impl>
const_themedim listcontainer_dim_elementObj<element_impl>::list_right_padding()
	const
{
	return themedim_element<listcontainer_dim_right>::getref();
}

//! Return the inner padding themedim

template<typename element_impl>
const_themedim listcontainer_dim_elementObj<element_impl>::list_inner_padding()
	const
{
	return themedim_element<listcontainer_dim_inner>::getref();
}

LIBCXXW_NAMESPACE_END

#endif
