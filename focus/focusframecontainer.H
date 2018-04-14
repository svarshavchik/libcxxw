/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainer_h
#define focusframecontainer_h

#include "focus/focusframecontainerfwd.H"
#include "focus/focusframecontainerobj.H"
#include "x/w/factory.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

focusframecontainer
create_focusframecontainer(const ref<focusframecontainerObj::implObj> &impl,
			   const element &e,
			   const focusable_impl &element_focusable_impl)
	LIBCXX_HIDDEN;

//! Install an element into the focus frame layout manager.

//! The newly-constructed container uses focusframelayoutimplObj as the
//! layout manager. Install an element into this container.

void install_focusframe_element(const container &c,
				const element &e) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
