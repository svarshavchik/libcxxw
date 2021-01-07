/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/focus/focusframecontainer_impl.H"
#include "x/w/impl/container.H"
#include "x/w/impl/current_border_impl.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

focusframelayoutimplObj
::focusframelayoutimplObj(const container_impl &parent_container,
			  const ref<focusframecontainer_implObj>
			  &focusframe_container_impl,
			  const element &initial_element)
	: borderlayoutmanagerObj
	  ::implObj{parent_container,
		    ref(&focusframe_container_impl
			->focusframe_bordercontainer_impl()),
		    initial_element,
		    halign::fill, valign::fill},
	  focusframe_container_impl{focusframe_container_impl}
{
}

focusframelayoutimplObj::~focusframelayoutimplObj()=default;

LIBCXXW_NAMESPACE_END
