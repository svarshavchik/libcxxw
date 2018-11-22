/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainer_impl_elementfwd_h
#define peepholed_listcontainer_impl_elementfwd_h

#include "peepholed_listcontainer_impl.H"
#include "listlayoutmanager/listcontainer_dim_elementfwd.H"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "x/w/impl/themedim_axis_elementfwd.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
class LIBCXX_HIDDEN peepholed_listcontainer_impl_elementObj;

//! Combine the peepholed_listcontainerObj::implObj with an element.

//! The template parameter is a superclass. This template multiply-inherits
//! from the template parameter and peepholed_listcontainerObj::implObj.
//!
//! The template parameter is a subclass of
//!
//! The first template parameter must be \c new_listlayoutmanager, the
//! remaining template parameters get passed to the superclass specified
//! by the template parameter.

template<typename element_impl>
class peepholed_listcontainer_impl_elementObj :
	public listcontainer_dim_elementObj<element_impl>,
	public peepholed_listcontainerObj::implObj {

public:
	typedef listcontainer_dim_elementObj<element_impl> superclass_t;

	template<typename ...Args>
		peepholed_listcontainer_impl_elementObj
		(const new_listlayoutmanager &style,
		 Args && ...args);

	~peepholed_listcontainer_impl_elementObj();

	listcontainer_pseudo_implObj &get_pseudo_impl() override;
	const listcontainer_pseudo_implObj &get_pseudo_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
