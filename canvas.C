/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "canvas.H"
#include "x/w/factory.H"

LIBCXXW_NAMESPACE_START

canvasObj::canvasObj(const ref<implObj> &impl) : elementObj(impl),
						 impl(impl)
{
}

canvasObj::~canvasObj()=default;

canvas factoryObj::create_canvas()
{
	return create_canvas([]
			     (const auto &ignore)
			     {
			     },
			     {0, 0},
			     {0, 0});
}

canvas factoryObj::do_create_canvas(const function<void (const canvas &)>
				    &creator,
				    const metrics::mmaxis &horiz,
				    const metrics::mmaxis &vert)
{
	auto canvas_impl=ref<canvasObj::implObj>::create(get_container_impl(),
							 horiz,
							 vert);

	auto c=canvas::create(canvas_impl);

	creator(c);
	created(c);

	return c;
}

LIBCXXW_NAMESPACE_END
