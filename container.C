/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "x/w/container.H"
#include "new_layoutmanager.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

containerObj::containerObj(const ref<implObj> &impl,
			   const new_layoutmanager &layout_factory)
	: elementObj(impl),
	  impl(impl),
	  layout_impl(layout_factory.create({impl}).layout_manager_impl)
{
	impl->install_layoutmanager(layout_impl);
}

containerObj::~containerObj()
{
	impl->uninstall_layoutmanager();
}

layoutmanager containerObj::get_layoutmanager()
{
	return layout_impl->create_public_object();
}

const_layoutmanager containerObj::get_layoutmanager() const
{
	return layout_impl->create_public_object();
}

LIBCXXW_NAMESPACE_END
