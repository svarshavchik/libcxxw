/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/canvas.H"
#include "x/w/impl/canvas.H"
#include "x/w/factory.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

canvasObj::canvasObj(const ref<implObj> &impl) : elementObj(impl),
						 impl(impl)
{
}

canvasObj::~canvasObj()=default;

void canvasObj::update(const dim_axis_arg &new_width,
		       const dim_axis_arg &new_height)
{
	in_thread([new_width, new_height, impl=this->impl]
		  (ONLY IN_THREAD)
		  {
			  impl->update(IN_THREAD, new_width, new_height);
		  });
}

canvas factoryObj::create_canvas()
{
	return create_canvas({});
}

canvas factoryObj::create_canvas(const canvas_config &config)
{
	auto canvas_impl=ref<canvasObj::implObj>
		::create(get_container_impl(),
			 canvas_init_params{config.width, config.height, {},
						    config.background_color});

	auto c=canvas::create(canvas_impl);

	created(c);

	return c;
}

LIBCXXW_NAMESPACE_END
