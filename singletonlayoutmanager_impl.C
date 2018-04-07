/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "singletonlayoutmanager_impl.H"
#include "singletonlayoutmanager.H"
#include "catch_exceptions.H"
#include "x/w/impl/container.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

singletonlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl,
	  const elementptr &initial_element)
	: layoutmanagerObj::implObj(container_impl),
	current_element(initial_element)
{
	if (initial_element &&
	    ref<child_elementObj>(initial_element->impl)->child_container !=
	    container_impl)
		throw EXCEPTION("Internal error: initial element for the "
				"singleton layout manager belongs to a "
				"different container");
}

singletonlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager singletonlayoutmanagerObj::implObj::create_public_object()
{
	return singletonlayoutmanager::create(ref<implObj>(this));
}

void singletonlayoutmanagerObj::implObj
::do_for_each_child(ONLY IN_THREAD,
		    const function<void (const element &e)> &callback)
{
	auto c=current_element.get();

	if (!c)
		return;

	c->impl->initialize_if_needed(IN_THREAD);

	callback(c);
}

dim_t singletonlayoutmanagerObj::implObj::get_left_padding(ONLY IN_THREAD)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::implObj::get_right_padding(ONLY IN_THREAD)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::implObj::get_top_padding(ONLY IN_THREAD)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::implObj::get_bottom_padding(ONLY IN_THREAD)
{
	return 0;
}

void singletonlayoutmanagerObj::implObj::created(const element &e)
{
	current_element=e;
}

elementptr singletonlayoutmanagerObj::implObj::get()
{
	return current_element.get();
}

elementimplptr singletonlayoutmanagerObj::implObj
::get_list_element_impl(ONLY IN_THREAD)
{
	auto e=get();

	if (!e)
		return elementimplptr();

	return e->impl;
}

void singletonlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
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
				 get_element_impl()
				 .data(IN_THREAD).current_position);
}

void singletonlayoutmanagerObj::implObj
::update_metrics(ONLY IN_THREAD,
		 const metrics::axis &horiz,
		 const metrics::axis &vert)
{
	auto &element=get_element_impl();

	element.get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      horiz, vert);
}

void singletonlayoutmanagerObj::implObj
::process_updated_position(ONLY IN_THREAD,
			   const rectangle &position)
{
	// If our own width/height is 0, don't bother updating the element's
	// position. This is used by the page layout manager to hide us.
	if (position.width == 0 || position.height == 0)
		return;

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
