/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image.H"
#include "x/w/factory.H"
#include "generic_window_handler.H"
#include "icon_cache.H"

LIBCXXW_NAMESPACE_START

imageObj::imageObj(const ref<implObj> &impl)
	: elementObj(impl)
{
}

imageObj::~imageObj()=default;


image factoryObj::create_image(const std::string_view &name,
			       const dim_arg &width,
			       const dim_arg &height,
			       render_repeat repeat)
{
	auto container_impl=get_container_impl();

	auto icon=container_impl->get_window_handler()
		.create_icon(name, repeat, width, height);

	auto impl=ref<imageObj::implObj>::create(container_impl, icon);

	auto i=image::create(impl);
	created(i);
	return i;
}

LIBCXXW_NAMESPACE_END
