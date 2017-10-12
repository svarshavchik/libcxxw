/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "singletonlayoutmanager_impl.H"
#include "singletonlayoutmanager.H"
#include "catch_exceptions.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

singletonlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl,
	  const elementptr &initial_element)
	: layoutmanagerObj::implObj(container_impl),
	current_element(&initial_element,
			(initial_element ? &initial_element+1:&initial_element))
{
}

singletonlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager singletonlayoutmanagerObj::implObj::create_public_object()
{
	return singletonlayoutmanager::create(ref<implObj>(this));
}

void singletonlayoutmanagerObj::implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &e)> &callback)
{
	auto c=({
			current_element_t::lock lock{current_element};

			if (lock->empty())
				return;

			lock->at(0);
		});

	c->impl->initialize_if_needed(IN_THREAD);

	callback(c);
}

dim_t singletonlayoutmanagerObj::implObj::get_left_padding(IN_THREAD_ONLY)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::implObj::get_right_padding(IN_THREAD_ONLY)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::implObj::get_top_padding(IN_THREAD_ONLY)
{
	return 0;
}

dim_t singletonlayoutmanagerObj::implObj::get_bottom_padding(IN_THREAD_ONLY)
{
	return 0;
}

void singletonlayoutmanagerObj::implObj::created(const element &e)
{
	current_element_t::lock lock{current_element};

	lock->push_back(e);
}

elementptr singletonlayoutmanagerObj::implObj::get()
{
	current_element_t::lock lock{current_element};

	if (!lock->empty())
		return lock->at(0);

	return {};
}

elementimplptr singletonlayoutmanagerObj::implObj
::get_list_element_impl(IN_THREAD_ONLY)
{
	std::vector<element> all_elements;

	// Grab the all_elements vector. If it has more than one element,
	// the current display element in the singleton is being replaced.
	// While we're holding the lock, leave the last element in
	// current_element. We'll clean everything up after releasing the
	// lock.

	{
		current_element_t::lock lock{current_element};

		all_elements=*lock;

		if (lock->size() > 1)
			lock->erase(lock->begin(), --lock->end());
	}

	if (all_elements.empty())
		return elementimplptr();

	// The last element in the vector is the official, remaining
	// element in the singleton. Make sure that everything is done
	// by the book.

	auto e=--all_elements.end();

	for (auto b=all_elements.begin(); b != e; ++b)
	{
		(*b)->impl->initialize_if_needed(IN_THREAD);
		(*b)->impl->removed_from_container(IN_THREAD);
	}

	(*e)->impl->initialize_if_needed(IN_THREAD);

	return (*e)->impl;
}

void singletonlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
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

void singletonlayoutmanagerObj::implObj
::update_metrics(IN_THREAD_ONLY,
		 const metrics::axis &horiz,
		 const metrics::axis &vert)
{
	auto &element=container_impl->get_element_impl();

	element.get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      horiz, vert);
}

void singletonlayoutmanagerObj::implObj
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
