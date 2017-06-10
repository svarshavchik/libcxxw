/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"
#include "themedim.H"
#include "screen.H"
#include "container.H"
#include "element.H"
#include "child_element.H"
#include "generic_window_handler.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

listitemlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl,
	  const element &initial_element,
	  const themedim &left_padding,
	  const themedim &right_padding,
	  const themedim &v_padding)
	: layoutmanagerObj::implObj(container_impl),
	list_element_thread_only(initial_element),
	left_padding(left_padding),
	right_padding(right_padding),
	v_padding(v_padding)
{
}

listitemlayoutmanagerObj::implObj::~implObj()=default;

ref<elementObj::implObj> listitemlayoutmanagerObj::implObj
::get_list_element_impl(IN_THREAD_ONLY)
{
	auto list_impl=list_element(IN_THREAD)->impl;

	try {
		list_impl->initialize_if_needed(IN_THREAD);
	} CATCH_EXCEPTIONS;

	return list_impl;
}

void listitemlayoutmanagerObj::implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &e)> &callback)
{
	callback(list_element(IN_THREAD));
}

layoutmanager listitemlayoutmanagerObj::implObj::create_public_object()
{
	return listitemlayoutmanager::create(ref<implObj>(this));
}

void listitemlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
{
	auto list_impl=get_list_element_impl(IN_THREAD);

	// Take the element's metrics, add the padding, and set our metrics
	// to it.

	auto child_horizvert=list_impl->get_horizvert(IN_THREAD);

	auto h2=dim_t::truncate(left_padding->pixels(IN_THREAD)
				+right_padding->pixels(IN_THREAD));

	auto v2=dim_t::truncate(v_padding->pixels(IN_THREAD)
				+v_padding->pixels(IN_THREAD));

	auto horiz=child_horizvert->horiz + metrics::axis{h2, h2, h2};
	auto vert=child_horizvert->vert + metrics::axis{v2, v2, v2};

	auto &element=container_impl->get_element_impl();

	element.get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      horiz, vert);

	// Also update the element's position based on the element's new
	// metrics and our current position.

	process_updated_position(IN_THREAD,
				 element.data(IN_THREAD).current_position);
}

void listitemlayoutmanagerObj::implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	auto lei=get_list_element_impl(IN_THREAD);

	auto child_horizvert=lei->get_horizvert(IN_THREAD);

	auto h2=dim_t::truncate(left_padding->pixels(IN_THREAD)
				+right_padding->pixels(IN_THREAD));

	auto v2=dim_t::truncate(v_padding->pixels(IN_THREAD)
				+v_padding->pixels(IN_THREAD));

	// Our opening bid is to take our own width and height, and subtract
	// twice the padding from it. This will be the width and the height
	// of the child element.

	dim_t width=position.width;
	dim_t height=position.height;

	if (width > h2 && height > v2)
	{
		width -= h2;
		height -= v2;
	}

	// Revise the opening bid by making sure that the width and height
	// does not exceed the child element's maximum.

	if (width > child_horizvert->horiz.maximum())
		width=child_horizvert->horiz.maximum();

	if (height > child_horizvert->vert.maximum())
		height=child_horizvert->vert.maximum();

	// Do not size the child element to be smaller than its minimum
	// metrics.

	if (width < child_horizvert->horiz.minimum())
		width=child_horizvert->horiz.minimum();

	if (height < child_horizvert->vert.minimum())
		height=child_horizvert->vert.minimum();


	// It is possible that the list layout manager will size us bigger
	// than our requested padding. We'll vertically-center and
	// left-align the child element.
	//
	// If everything goes according to plan, (x,y) will be the same as
	// the padding.

	coord_t x=coord_t::truncate(left_padding->pixels(IN_THREAD));
	coord_t y=0;

	if (position.height > height)
		y=coord_t::truncate((position.height-height)/2);

	lei->update_current_position(IN_THREAD, {x, y, width, height});
}

void listitemlayoutmanagerObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	// themedims are owned by the parent container, which will be recalced
	// first.

	recalculate(IN_THREAD);
}


LIBCXXW_NAMESPACE_END
