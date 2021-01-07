/*
** Copyright 2017-2021 Double Precision, Inc.
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

image factoryObj::create_image(const std::string &name)
{
	return create_image(name, 0, 0);
}

image factoryObj::create_image(const std::string &name,
			       const dim_arg &width,
			       const dim_arg &height)
{
	auto container_impl=get_container_impl();

	auto icon=container_impl->get_window_handler()
		.create_icon({name, render_repeat::none, width, height,
			      icon_scale::nearest});

	auto impl=ref<imageObj::implObj>::create(image_impl_init_params
						 {container_impl, icon});

	auto i=image::create(impl);
	created(i);
	return i;
}

LIBCXXW_NAMESPACE_END
