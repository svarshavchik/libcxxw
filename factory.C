/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/factory.H"
#include "container.H"
#include "canvas.H"
#include "messages.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

factoryObj::factoryObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

factoryObj::~factoryObj()=default;

canvas factoryObj::do_create_canvas(const function<void (const canvas &)>
				    &creator,
				    const metrics::mmaxis &horiz,
				    const metrics::mmaxis &vert,
				    metrics::halign h,
				    metrics::valign v)
{
	auto canvas_impl=ref<canvasObj::implObj>::create(container_impl,
							 horiz,
							 vert,
							 h,
							 v);

	auto c=canvas::create(canvas_impl);

	creator(c);
	created(c);

	return c;
}

LIBCXXW_NAMESPACE_END
