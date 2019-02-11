/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/new_layoutmanager.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/layoutmanager.H"

LIBCXXW_NAMESPACE_START

new_layoutmanager::new_layoutmanager()=default;

new_layoutmanager::~new_layoutmanager()=default;

container new_layoutmanager
::create(const container_impl &parent,
	 const function<void(const container &)> &creator) const
{
	auto impl=ref<container_elementObj<child_elementObj>>
		::create(parent,child_element_init_params{"background@libcxx.com"});

	auto c=container::create(impl, create(impl));
	creator(c);
	return c;
}

LIBCXXW_NAMESPACE_END
