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
class LIBCXX_HIDDEN peepholed_listcontainer_impl_element_baseObj;

template<typename element_impl>
class LIBCXX_HIDDEN peepholed_listcontainer_impl_elementObj;

//! Combine the peepholed_listcontainerObj::implObj with an element.

//! For internal use by peepholed_listcontainer_impl_elementObj, which
//! uses this template, pulls in the font and dimension mixins that
//! peepholed_listcontainerObj::implObj uses; and pulls in the
//! themedim_axis_elementObj mixin to handle the list height when it is
//! specified as a dim_axis_arg. This makes these mixins to
//! be the subclass of both peepholed_listcontainer_impl_elementObj and
//! the template parameter, which must be a subclass of
//! listcontainer_pseudo_implObj.
//!
//! One of the combining elements is listcontainerObj::implObj which also
//! uses the same font and dimension mixin.
//!
//! The first constructor parameter is a new_listlayoutmanager that gets
//! passed to peepholed_listcontainer_impl_elementObj's constructor.
//! The remaining constructor parameters get passed to the superclass
//! template parameter.

template<typename element_impl>
class peepholed_listcontainer_impl_element_baseObj
	: public element_impl, public peepholed_listcontainerObj::implObj {

 public:
	using element_impl::element_impl;

	~peepholed_listcontainer_impl_element_baseObj();

	void initialize(ONLY IN_THREAD) override;

	void horizvert_updated(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
		override;

};

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
	public listcontainer_dim_elementObj<
	peepholed_listcontainer_impl_element_baseObj<element_impl>> {

public:
	typedef listcontainer_dim_elementObj<
	peepholed_listcontainer_impl_element_baseObj<
		element_impl>> superclass_t;

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
