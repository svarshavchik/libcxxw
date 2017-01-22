/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_thread.H"
#include "element.H"
#include "container.H"
#include "layoutmanager.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

void connection_threadObj::insert_element_set(element_set_t &s,
					      const elementimpl &i)
{
	s[i->nesting_level].insert(i);
}

// Remove the lowest element from the non-empty element_set_t.
//
// Returns the next element with the largest nesting_level. This is used
// for optimally processing visibility changes, for example, so that the
// so that the topmost element's visibility is updated last.

elementimpl connection_threadObj::next_lowest_element(element_set_t &s)
{
	auto iter=--s.end();

	auto &last_set=iter->second;

	auto element_iter=last_set.begin();

	auto element=*element_iter;

	last_set.erase(element_iter);

	if (last_set.empty())
		s.erase(iter);

	return element;
}

// Remove the highest element from the non-empty element_set_t.
//
// Returns the next element with the smallest nesting_level. This is used
// for optimally processing sizing changes, for example, so that the
// so that the consequences of the topmost element's size change is processed
// last, since the topmost element may choose to recalculate the sizes of its
// child elements.

elementimpl connection_threadObj::next_highest_element(element_set_t &s)
{
	auto iter=s.begin();

	auto &first_set=iter->second;

	auto element_iter=first_set.begin();

	auto element=*element_iter;

	first_set.erase(element_iter);

	if (first_set.empty())
		s.erase(iter);

	return element;
}

// Some display element changed their visibility. Invoke
// their update_visibility() methods.

bool connection_threadObj::process_visibility_updated(IN_THREAD_ONLY)
{
	if (visibility_updated_thread_only->empty())
		return false;

	while (!visibility_updated_thread_only->empty())
	{
		try {
			next_lowest_element(*visibility_updated_thread_only)
				->update_visibility(IN_THREAD);
		} CATCH_EXCEPTIONS;
	}
	return true;
}

bool connection_threadObj::recalculate_containers(IN_THREAD_ONLY)
{
	if (containers_2_recalculate_thread_only->empty())
		return false;

	// Get the next container to be recalculated, with the
	// highest nesting level.

	while (!containers_2_recalculate_thread_only->empty())
	{
		auto p=--containers_2_recalculate_thread_only->end();

		auto last=--p->second.end();

		auto container=*last;

		// Now that we have it, remove it from
		// containers_2_recalculate

		p->second.erase(last);

		if (p->second.empty())
			containers_2_recalculate_thread_only->erase(p);

		// And invoke it

		try {
			container->invoke_layoutmanager
				([&]
				 (const auto &l)
				 {
					 l->check_if_recalculate_needed
						 (IN_THREAD);
				 });
		} CATCH_EXCEPTIONS;
	}
	return true;
}

bool connection_threadObj::redraw_elements(IN_THREAD_ONLY)
{
	if (elements_to_redraw_thread_only->empty())
		return false;

	while (!elements_to_redraw_thread_only->empty())
	{
		auto p=elements_to_redraw_thread_only->begin();

		auto elem=*p;
		elements_to_redraw_thread_only->erase(p);

		elem->explicit_redraw(IN_THREAD);
	}

	return true;
}

LIBCXXW_NAMESPACE_END
