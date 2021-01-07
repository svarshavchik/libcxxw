/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/focusable.H"
#include "panelayoutmanager/pane_slider.H"
#include "panelayoutmanager/pane_slider_impl.H"
#include "x/w/impl/element.H"

LIBCXXW_NAMESPACE_START

pane_sliderObj::pane_sliderObj(const ref<implObj> &impl)
	: elementObj{ref(&impl->slider_element_impl())},
	  impl{impl}
{
}

pane_sliderObj::~pane_sliderObj()=default;

focusable_impl pane_sliderObj::get_impl() const
{
	return ref(&impl->slider_focusable_impl());
}

LIBCXXW_NAMESPACE_END
