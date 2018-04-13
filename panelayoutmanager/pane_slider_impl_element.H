/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_slider_impl_element_h
#define pane_slider_impl_element_h

#include "panelayoutmanager/pane_slider_impl_elementfwd.H"
#include "x/w/impl/focus/focusable_element.H"
#include "x/w/impl/always_visible_element.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename straight_borderobj_t>
template<typename ...Args>
inline pane_slider_impl_elementObj<straight_borderobj_t>
::pane_slider_impl_elementObj(Args && ...args)
	: superclass_t{std::forward<Args>(args)...}
{
}

//! Destructor
template<typename straight_borderobj_t>
pane_slider_impl_elementObj<straight_borderobj_t>
::~pane_slider_impl_elementObj()=default;

//! Implement slider_element_impl()

template<typename straight_borderobj_t>
elementObj::implObj &pane_slider_impl_elementObj<straight_borderobj_t>
::slider_element_impl()
{
	return *this;
}

//! Implement slider_focusalbe_impl()
template<typename straight_borderobj_t>
focusableObj::implObj &pane_slider_impl_elementObj<straight_borderobj_t>
::slider_focusable_impl()
{
	return *this;
}

LIBCXXW_NAMESPACE_END

#endif
