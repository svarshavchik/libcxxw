/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/container.H"
#include "x/w/container.H"
#include "x/w/factory.H"
#include "x/w/new_layoutmanager.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/child_element.H"
#include "run_as.H"
#include "get_layoutmanagers.inc.H"

LIBCXXW_NAMESPACE_START

containerObj::containerObj(const ref<implObj> &impl,
			   const layout_impl &container_layout_impl)
	: elementObj{ref{&impl->container_element_impl()}},
	  impl{impl},
	  container_layout_impl{container_layout_impl}
{
	impl->install_layoutmanager(container_layout_impl);
}

containerObj::~containerObj()
{
	// We want to call removed_from_container() before
	// uninstall_layoutmanager() and before ~elementObj() gets called,
	// so that the message to the connection thread for this parent
	// container gets sent, and processed first, recursively flagging
	// all elements in the container as being removed.
	//
	// uninstalling layoutmanager is going to destroy all the child
	// elements, stored in the layout manager implementation object,
	// invoking their destructors, sending their messages to
	// remove_from_container() themselves, individually. It is far
	// more efficient for the message from the container to be processed
	// first, recursively setting the removed flag on everything.
	//
	// When the container has focusable elements, this ends up flagging
	// all of them as removed, so the focus ends up being transferred
	// one time, to whatever available focusable element still exists,
	// rather than ping-ponging from one element being removed to the
	// next one, before it gets flagged for removal.

	elementObj::impl->removed_from_container();
}

layout_impl containerObj::get_layout_impl() const
{
	return container_layout_impl;
}

layoutmanager containerObj::get_layoutmanager()
{
	return get_layout_impl()->create_public_object();
}

const_layoutmanager containerObj::get_layoutmanager() const
{
	return get_layout_impl()->create_public_object();
}

container factoryObj
::do_create_container(const function<void (const container &)> &creator,
		      const new_layoutmanager &layout_manager)
{
	auto c=layout_manager.create(get_container_impl(), creator);
	created(c);
	return c;
}

#include "get_layoutmanagers.inc.C"

LIBCXXW_NAMESPACE_END
