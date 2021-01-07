/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pagelayoutmanager_impl.H"
#include "container_impl.H"
#include "x/w/impl/metrics_horizvert.H"
#include "x/w/metrics/derivedaxis.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

new_pagelayoutmanager::new_pagelayoutmanager()=default;

new_pagelayoutmanager::~new_pagelayoutmanager()=default;

layout_impl new_pagelayoutmanager::create(const container_impl &c) const
{
	return ref<pagelayoutmanagerObj::implObj>::create(c);
}

pagelayoutmanagerObj::implObj::implObj(const container_impl &c)
	: layoutmanagerObj::implObj(c)
{
}

pagelayoutmanagerObj::implObj::~implObj()=default;

size_t pagelayoutmanagerObj::implObj::pages()
{
	page_layout_info_t::lock lock{info};

	return lock->elements.size();
}

void pagelayoutmanagerObj::implObj::append(const switch_element_info &e)
{
	page_layout_info_t::lock lock{info};

	if (!lock->element_index.insert({e.the_element, 0}).second)
		throw EXCEPTION("Internal error: inconsistent element_index");
	lock->elements.push_back(e);
	rebuild_index(lock);
}

void pagelayoutmanagerObj::implObj::insert(size_t p,
					     const switch_element_info &e)
{
	page_layout_info_t::lock lock{info};

	if (p > lock->elements.size())
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));

	if (!lock->element_index.insert({e.the_element, 0}).second)
		throw EXCEPTION("Internal error: inconsistent element_index");

	lock->elements.insert(lock->elements.begin()+p, e);
	rebuild_index(lock);

	// Update current_element, if needed.

	if (lock->current_element)
	{
		auto n=*lock->current_element;

		if (n >= p)
		{
			++n;
			lock->current_element=n;
		}
	}
}

void pagelayoutmanagerObj::implObj::remove(size_t n)
{
	page_layout_info_t::lock lock{info};

	if (n >= lock->elements.size())
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));

	auto iter=lock->elements.begin()+n;

	lock->element_index.erase(iter->the_element);

	lock->elements.erase(iter);
	rebuild_index(lock);

	// Update current_element, intelligently.

	if (lock->elements.empty())
	{
		lock->current_element.reset();
		return;
	}

	if (!lock->current_element)
		return;

	if (*lock->current_element > n ||

	    // Removed element was the last one
	    *lock->current_element >= lock->elements.size())
		--*lock->current_element;
}

void pagelayoutmanagerObj::implObj
::rebuild_index(page_layout_info_t::lock &lock)
{
	size_t i=0;

	for (const auto &info:lock->elements)
		lock->element_index.at(info.the_element)=i++;
}

void pagelayoutmanagerObj::implObj
::do_for_each_child(ONLY IN_THREAD,
		    const function<void(const element &)> &callback)
{
	page_layout_info_t::lock lock{info};

	for (const auto &e:lock->elements)
		callback(e.the_container);
}

size_t pagelayoutmanagerObj::implObj::num_children(ONLY IN_THREAD)
{
	page_layout_info_t::lock lock{info};

	return lock->elements.size();
}

layoutmanager pagelayoutmanagerObj::implObj::create_public_object()
{
	return create_pagelayoutmanager();
}

pagelayoutmanager pagelayoutmanagerObj::implObj::create_pagelayoutmanager()
{
	return pagelayoutmanager::create(ref{this});
}

void pagelayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	process_updated_position(IN_THREAD,
				 layout_container_impl->container_element_impl()
				 .data(IN_THREAD).current_position);
}

void pagelayoutmanagerObj::implObj
::process_updated_position(ONLY IN_THREAD,
			   const rectangle &position)
{
	page_layout_info_t::lock lock{info};

	// Sanity check.

	if (lock->current_element &&
	    *lock->current_element >= lock->elements.size())
		lock->current_element.reset();

	// Compute the derived metrics for ourselves based on the switched
	// elements' metrics.

	metrics::derivedaxis mhoriz, mvert;

	for (const auto &e:lock->elements)
	{
		auto impl=e.the_container->elementObj::impl;

		// Take this opportunity to welcome the new elements.
		impl->initialize_if_needed(IN_THREAD);

		auto hv=impl->get_horizvert(IN_THREAD);

		mhoriz(hv->horiz);
		mvert(hv->vert);
	}

	auto my_metrics=layout_container_impl->container_element_impl()
		.get_horizvert(IN_THREAD);

	// We don't want maximum=infinite if we have no elements.
	if (lock->elements.empty())
		my_metrics->set_element_metrics(IN_THREAD,
						{0, 0, 0},
						{0, 0, 0});
	else
		my_metrics->set_element_metrics(IN_THREAD,
						mhoriz, mvert);


	// Go through all elements, and reset their position.

	size_t i=0;

	for (const auto &e:lock->elements)
	{
		// The open bit for the element's size is this container's
		// current size.

		auto w=position.width;
		auto h=position.height;

		if (!lock->current_element ||
		    *lock->current_element != i)
		{
			// This page is not visible. Set the shim container's
			// size to 0x0 pixels, effectively hiding everything
			// on the page.
			//
			// Sizing it to 0 width and height alone will not be
			// sufficient. If the page contains focusable fields
			// they'll still be logically visible and tabbable,
			// so we must do both, size the sim container to 0
			// width and height, and mark it as invisible.
			e.the_container->elementObj::impl
				->request_visibility(IN_THREAD, false);
			e.the_container->elementObj::impl
				->update_current_position
				(IN_THREAD, {0, 0, 0, 0});
		}
		else
		{
			// The the_container to be the size of the visible
			// page, we will compute the page widget's actual
			// size and position below.
			e.the_container->elementObj::impl
				->update_current_position
				(IN_THREAD, {0, 0, w, h});
			e.the_container->elementObj::impl
				->request_visibility(IN_THREAD, true);
		}

		// If the element's minimum size is larger, increase
		// the proposed element size. This should be
		// propagated into our own metrics, the parent
		// container probably hasn't had the chance to resize
		// us yet. But do not size the element smaller than
		// its minimum size.

		auto m=e.the_container->elementObj::impl
			->get_horizvert(IN_THREAD);

		if (w < m->horiz.minimum())
			w=m->horiz.minimum();
		if (h < m->vert.minimum())
			h=m->vert.minimum();

		// If our size is larger than the element's maximum,
		// adjust our expectations.

		if (w > m->horiz.maximum())
			w=m->horiz.maximum();

		if (h > m->vert.maximum())
			h=m->vert.maximum();

		// We're ready to use the bog standard alignment logic,
		// and position it accordingly.

		auto aligned=metrics::align(position.width,
					    position.height,
					    w, h,
					    e.horizontal_alignment,
					    e.vertical_alignment);

		// Position the page widget.

		e.the_element->impl
			->update_current_position(IN_THREAD, aligned);
		++i;
	}
}

LIBCXXW_NAMESPACE_END
