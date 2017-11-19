/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "switchlayoutmanager_impl.H"
#include "container_impl.H"
#include "metrics_horizvert.H"
#include "x/w/metrics/derivedaxis.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

new_switchlayoutmanager::new_switchlayoutmanager()=default;

new_switchlayoutmanager::~new_switchlayoutmanager()=default;

ref<layoutmanagerObj::implObj>
new_switchlayoutmanager::create(const ref<containerObj::implObj> &c) const
{
	return ref<switchlayoutmanagerObj::implObj>::create(c);
}

switchlayoutmanagerObj::implObj::implObj(const ref<containerObj::implObj> &c)
	: layoutmanagerObj::implObj(c)
{
}

switchlayoutmanagerObj::implObj::~implObj()=default;

void switchlayoutmanagerObj::implObj::append(const switch_element_info &e)
{
	switch_layout_info_t::lock lock{info};

	lock->elements.push_back(e);
}

void switchlayoutmanagerObj::implObj::insert(size_t p,
					     const switch_element_info &e)
{
	switch_layout_info_t::lock lock{info};

	if (p > lock->elements.size())
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));
	lock->elements.insert(lock->elements.begin()+p, e);

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

void switchlayoutmanagerObj::implObj::remove(size_t n)
{
	switch_layout_info_t::lock lock{info};

	if (n >= lock->elements.size())
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));

	lock->elements.erase(lock->elements.begin()+n);

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

void switchlayoutmanagerObj::implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void(const element &)> &callback)
{
	switch_layout_info_t::lock lock{info};

	for (const auto &e:lock->elements)
		callback(e.the_container);
}

layoutmanager switchlayoutmanagerObj::implObj::create_public_object()
{
	return switchlayoutmanager::create(ref(this));
}

void switchlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
{
	process_updated_position(IN_THREAD, container_impl->get_element_impl()
				 .data(IN_THREAD).current_position);
}

void switchlayoutmanagerObj::implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	switch_layout_info_t::lock lock{info};

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

		// If there is no current element, take the opportunity this
		// loops presents to hide everything.
		//
		// Merely turning off visibility is insufficient. The
		// display element still takes up space and its location
		// which logically overlaps all others' is going to get
		// cleared with the container's background color.
		//
		// So in addition to hiding it we must size it to 0 width
		// and height.
		//
		// Sizing it to 0 width and height alone will not be
		// sufficient either. If it contains focusable fields,
		// they would still be logically visible, and tabbable.

		if (!lock->current_element)
		{
			impl->request_visibility(IN_THREAD, false);
			impl->update_current_position
				(IN_THREAD, {0, 0, 0, 0});
		}

		auto hv=impl->get_horizvert(IN_THREAD);

		mhoriz(hv->horiz);
		mvert(hv->vert);
	}

	auto my_metrics=
		container_impl->get_element_impl().get_horizvert(IN_THREAD);

	// We don't want maximum=infinite if we have no elements.
	if (lock->elements.empty())
		my_metrics->set_element_metrics(IN_THREAD,
						{0, 0, 0},
						{0, 0, 0});
	else
		my_metrics->set_element_metrics(IN_THREAD,
						mhoriz, mvert);

	if (!lock->current_element)
		return;

	// A current element is being shown. Go through all elements,
	// and hide all except the one that's shown. And for the one
	// that's shown, figure out how it should be positioned inside me.

	size_t j=*lock->current_element;

	size_t i=0;

	for (const auto &e:lock->elements)
	{
		if (i != j)
		{
			e.the_container->elementObj::impl
				->request_visibility(IN_THREAD, false);
			e.the_container->elementObj::impl
				->update_current_position
				(IN_THREAD, {0, 0, 0, 0});
			++i;
			continue;
		}

		// The open bit for the element's size is this container's
		// current size.

		auto w=position.width;
		auto h=position.height;

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

		e.the_container->elementObj::impl->update_current_position
			(IN_THREAD, aligned);
		e.the_container->elementObj::impl
			->request_visibility(IN_THREAD, true);
		++i;
	}
}

LIBCXXW_NAMESPACE_END
