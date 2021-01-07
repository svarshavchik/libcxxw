/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_slider_impl_elementfwd_h
#define pane_slider_impl_elementfwd_h

#include "panelayoutmanager/pane_slider_impl.H"
#include "x/w/impl/focus/focusable_elementfwd.H"
#include "x/w/impl/always_visible_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Combine the pane_slider_implObj mixin with the border element object.

//! The template is either horizontal_straight_borderObj or
//! vertical_straight_borderObj. Multiply-inherits from the template parameter
//! and pane_sliderObj::implObj.

template<typename straight_borderobj_t>
class LIBCXX_HIDDEN pane_slider_impl_elementObj
	: public focusable_elementObj<always_visible_elementObj
				      <straight_borderobj_t>>,
	  public pane_sliderObj::implObj {

	//! Alias.

	typedef focusable_elementObj<
		always_visible_elementObj<straight_borderobj_t>> superclass_t;

 public:

	template<typename ...Args> pane_slider_impl_elementObj(Args && ...args);

	~pane_slider_impl_elementObj();

	elementObj::implObj &slider_element_impl() override;

	focusableObj::implObj &slider_focusable_impl() override;
};

LIBCXXW_NAMESPACE_END

#endif
