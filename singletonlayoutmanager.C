/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "singletonlayoutmanager.H"
#include "catch_exceptions.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

singletonlayoutmanagerObj
::singletonlayoutmanagerObj(const ref<containerObj::implObj>
			    &container_impl,
			    const elementptr &initial_element)
	: layoutmanagerObj::implObj(container_impl),
	current_element_thread_only(initial_element)
{
}

singletonlayoutmanagerObj::~singletonlayoutmanagerObj()=default;

void singletonlayoutmanagerObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &e)> &callback)
{
	if (current_element(IN_THREAD))
		callback(current_element(IN_THREAD));
}

dim_t singletonlayoutmanagerObj::get_left_padding(IN_THREAD_ONLY)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::get_right_padding(IN_THREAD_ONLY)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::get_top_padding(IN_THREAD_ONLY)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::get_bottom_padding(IN_THREAD_ONLY)
{
	return 0;
}

ptr<elementObj::implObj> singletonlayoutmanagerObj
::get_list_element_impl(IN_THREAD_ONLY)
{
	if (!current_element(IN_THREAD))
		return ptr<elementObj::implObj>();

	auto list_impl=current_element(IN_THREAD)->impl;

	try {
		list_impl->initialize_if_needed(IN_THREAD);
	} CATCH_EXCEPTIONS;

	return list_impl;
}

void singletonlayoutmanagerObj::recalculate(IN_THREAD_ONLY)
{
	auto list_impl=get_list_element_impl(IN_THREAD);

	auto h2=dim_t::truncate(get_left_padding(IN_THREAD)
				+get_right_padding(IN_THREAD));

	auto v2=dim_t::truncate(get_top_padding(IN_THREAD)
				+get_bottom_padding(IN_THREAD));

	metrics::axis horiz{h2, h2, h2};
	metrics::axis vert{v2, v2, v2};

	// Take the element's metrics, add the padding, and set our metrics
	// to it.
	if (list_impl)
	{
		auto child_horizvert=list_impl->get_horizvert(IN_THREAD);

		horiz=child_horizvert->horiz + horiz;
		vert=child_horizvert->vert + vert;
	}

	update_metrics(IN_THREAD, horiz, vert);

	// Also update the element's position based on the element's new
	// metrics and our current position.

	process_updated_position(IN_THREAD,
				 container_impl->get_element_impl()
				 .data(IN_THREAD).current_position);
}

void singletonlayoutmanagerObj::update_metrics(IN_THREAD_ONLY,
					       const metrics::axis &horiz,
					       const metrics::axis &vert)
{
	auto &element=container_impl->get_element_impl();

	element.get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      horiz, vert);
}

void singletonlayoutmanagerObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	auto lei=get_list_element_impl(IN_THREAD);

	if (!lei)
		return;

	auto child_horizvert=lei->get_horizvert(IN_THREAD);

	// The usable size for position the element is the container's
	// current size, less the specified padding.

	dim_t w_overhead=dim_t::truncate(get_left_padding(IN_THREAD)+
					 get_right_padding(IN_THREAD));
	dim_t h_overhead=dim_t::truncate(get_top_padding(IN_THREAD)+
					 get_bottom_padding(IN_THREAD));

	dim_t usable_width=position.width > w_overhead
		? position.width-w_overhead:dim_t{0};

	dim_t usable_height=position.height > h_overhead
		? position.height-h_overhead:dim_t{0};

	// Opening bid is that the element's height is its maximum height,
	// reduce if the container is not tlal enough.
	auto h=child_horizvert->vert.maximum();

	if (h > position.height)
		h=usable_height;

	// ... but not below the container's minimum height.
	if (h < child_horizvert->vert.minimum())
		h=child_horizvert->vert.minimum();

	// Similar logic for the width.
	auto w=child_horizvert->horiz.maximum();

	if (w > usable_width)
		w=usable_width;

	if (w < child_horizvert->horiz.minimum())
		w=child_horizvert->horiz.minimum();

	// Vertically center the element if the usable height is more than
	// the element needs.
	coord_t y=0;

	if (h < usable_height)
		y=coord_t::truncate( (usable_height-h)/2 );

	lei->update_current_position(IN_THREAD, {
			coord_t::truncate(get_left_padding(IN_THREAD)),
				coord_t::truncate(y+get_top_padding(IN_THREAD)),
				w, h});
}

LIBCXXW_NAMESPACE_END
