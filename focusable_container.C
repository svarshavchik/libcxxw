/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable_container.H"
#include "x/w/new_focusable_layoutmanagerfwd.H"
#include "x/w/factory.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

new_focusable_layoutmanager::new_focusable_layoutmanager()=default;

new_focusable_layoutmanager::~new_focusable_layoutmanager()=default;

focusable_container
factoryObj::do_create_focusable_container(const function<void
					  (const focusable_container
					   &)> &creator,
					  const new_focusable_layoutmanager
					  &layout_manager)
{
	auto c=layout_manager.create(get_container_impl());
	creator(c);
	created(c);
	return c;
}

LIBCXXW_NAMESPACE_END
