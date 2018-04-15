/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainer_h
#define focusframecontainer_h

#include "x/w/impl/focus/focusframecontainer_impl.H"
#include "x/w/focusable_container_owner.H"
#include "x/w/element.H"
#include "x/w/focusableobj.H"

LIBCXXW_NAMESPACE_START

focusable_container_owner
create_focusframe_container_owner(const ref<focusframecontainer_implObj> &impl,
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
