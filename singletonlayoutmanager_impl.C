/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/singletonlayoutmanager.H"
#include "x/w/singletonlayoutmanager.H"
#include "catch_exceptions.H"
#include "x/w/impl/container.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/metrics_horizvert.H"

LIBCXXW_NAMESPACE_START

singletonlayoutmanagerObj::implObj
::implObj(const container_impl &container_impl,
	  const element &initial_element,
	  halign element_halign,
	  valign element_valign)
	: layoutmanagerObj::implObj{container_impl},
	  current_element{initial_element},
	  element_halign{element_halign},
	  element_valign{element_valign}
{
	if (ref<child_elementObj>(initial_element->impl)->child_container !=
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

	c->impl->initialize_if_needed(IN_THREAD);

	callback(c);
}

void singletonlayoutmanagerObj::implObj::initialize(ONLY IN_THREAD)
{
	auto c=current_element.get();

	c->impl->initialize_if_needed(IN_THREAD);

	layoutmanagerObj::implObj::initialize(IN_THREAD);
	needs_recalculation(IN_THREAD);

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

element singletonlayoutmanagerObj::implObj::get()
{
	return current_element.get();
}

element_impl singletonlayoutmanagerObj::implObj
::get_list_element_impl(ONLY IN_THREAD)
{
	auto e=get();

	return e->impl;
}

void singletonlayoutmanagerObj::implObj
::theme_updated(ONLY IN_THREAD,
		const defaulttheme &new_theme)
{
	layoutmanagerObj::implObj::theme_updated(IN_THREAD, new_theme);
	needs_recalculation(IN_THREAD);
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
	auto child_horizvert=list_impl->get_horizvert(IN_THREAD);

	horiz=child_horizvert->horiz + horiz;
	vert=child_horizvert->vert + vert;

	update_metrics(IN_THREAD, horiz, vert);
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

	auto lp=get_left_padding(IN_THREAD);
	auto tp=get_top_padding(IN_THREAD);

	// The usable size for position the element is the container's
	// current size, less the specified padding.

	dim_t w_overhead=dim_t::truncate(lp+get_right_padding(IN_THREAD));
	dim_t h_overhead=dim_t::truncate(tp+get_bottom_padding(IN_THREAD));

	dim_t usable_width=position.width > w_overhead
		? position.width-w_overhead:dim_t{0};

	dim_t usable_height=position.height > h_overhead
		? position.height-h_overhead:dim_t{0};

	// Position the element within the usable area based on the requested
	// alignment.

	auto child_horizvert=lei->get_horizvert(IN_THREAD);

	// Opening bid are the child element's maximum metrics.

	auto w=child_horizvert->horiz.maximum();
	auto h=child_horizvert->vert.maximum();

	// But they can't exceed the usable size.

	if (w > usable_width)
		w=usable_width;

	if (h > usable_height)
		h=usable_height;

	// And they can't be less.

	if (w < child_horizvert->horiz.minimum())
		w=child_horizvert->horiz.minimum();

	if (h < child_horizvert->vert.minimum())
		h=child_horizvert->vert.minimum();

	auto r=metrics::align(usable_width, usable_height,
			      w, h,
			      element_halign,
			      element_valign);

	if (r.width > usable_width)
		r.width=usable_width;

	if (r.height > usable_height)
		r.height=usable_height;

	r.x=coord_t::truncate(r.x+lp);
	r.y=coord_t::truncate(r.y+tp);

	lei->update_current_position(IN_THREAD, r);
}

LIBCXXW_NAMESPACE_END
