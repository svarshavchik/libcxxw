/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole_layoutmanager_impl.H"
#include "catch_exceptions.H"
#include "container.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::peepholeObj::layoutmanager_implObj)
LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_START

peepholeObj::layoutmanager_implObj
::layoutmanager_implObj(const ref<containerObj::implObj> &container_impl,
			const element &peephole_element)
	: layoutmanagerObj::implObj(container_impl),
	peephole_element(peephole_element)
{
}

peepholeObj::layoutmanager_implObj::~layoutmanager_implObj()=default;

void peepholeObj::layoutmanager_implObj
::child_metrics_updated(IN_THREAD_ONLY)
{
	recalculate(IN_THREAD);
}

void peepholeObj::layoutmanager_implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &e)> &callback)
{
	callback(peephole_element);
}

layoutmanager peepholeObj::layoutmanager_implObj::create_public_object()
{
	return layoutmanager::create(ref<layoutmanagerObj::implObj>(this));
}

void peepholeObj::layoutmanager_implObj
::ensure_visibility(IN_THREAD_ONLY,
		    elementObj::implObj &e,
		    const rectangle &r)
{
	requested_visibility=r;
	recalculate(IN_THREAD);
}

// Called from recalculate to make sure that the proposed element_pos
// will have (offset_x, offset_y) coordinate visible, given the peephole's
// own current_position.

static void adjust_for_visibility(rectangle &element_pos,
				  const rectangle &current_position,
				  coord_squared_t x,
				  coord_squared_t y)
{
	auto targeted_x=element_pos.x + coord_t::truncate(x);
	auto targeted_y=element_pos.y + coord_t::truncate(y);

	// If the computed coordinates is beyond the given
	// margin, shift element_pos accordingly.

	if (targeted_x > coord_t::truncate(current_position.width))
	{
		element_pos.x=coord_squared_t::truncate(current_position.width)
			-coord_t::truncate(x);
	}
	else if (targeted_x < 0)
	{
		element_pos.x=coord_t::truncate(-x);
	}

	if (targeted_y > coord_t::truncate(current_position.height))
	{
		element_pos.y=coord_squared_t::truncate(current_position.height)
			-coord_t::truncate(y);
	}
	else if (targeted_y < 0)
	{
		element_pos.y=coord_t::truncate(-y);
	}
}

void peepholeObj::layoutmanager_implObj::recalculate(IN_THREAD_ONLY)
{
	try {
		peephole_element->impl->initialize_if_needed(IN_THREAD);
	} CATCH_EXCEPTIONS;

	auto &current_position=container_impl->get_element_impl()
		.data(IN_THREAD).current_position;

	auto &element_current_position=
		peephole_element->impl->data(IN_THREAD).current_position;
	auto element_horizvert=
		peephole_element->impl->get_horizvert(IN_THREAD);

	rectangle element_pos{element_current_position.x,
			element_current_position.y,
			element_horizvert->horiz.preferred(),
			element_horizvert->vert.preferred()};

	LOG_DEBUG("Peephole " << this
		  << ": current position is " << current_position
		  << std::endl
		  << "   element position is " << element_pos);

	coord_t min_scroll_x=0;
	coord_t min_scroll_y=0;

	if (element_pos.width > current_position.width)
	{
		min_scroll_x=-coord_t::truncate(element_pos.width-
						current_position.width);
	}

	if (element_pos.height > current_position.height)
	{
		min_scroll_y=-coord_t::truncate(element_pos.height-
						current_position.height);
	}

	LOG_DEBUG("Minimum X position is " << min_scroll_x);
	LOG_DEBUG("Minimum Y position is " << min_scroll_y);

	// Call for adjust_visibility. Notably we scroll bottom/right
	// requested_visibility corner into view first. This makes sure that
	// the bottom/right is scrolled into view first, then top/left.

	adjust_for_visibility(element_pos, current_position,
			      requested_visibility.x+requested_visibility.width,
			      requested_visibility.y+requested_visibility.height
			      );

	adjust_for_visibility(element_pos, current_position,
			      requested_visibility.x+dim_t{0},
			      requested_visibility.y+requested_visibility.height
			      );

	adjust_for_visibility(element_pos, current_position,
			      requested_visibility.x+requested_visibility.width,
			      requested_visibility.y+dim_t{0});

	adjust_for_visibility(element_pos, current_position,
			      requested_visibility.x+dim_t{0},
			      requested_visibility.y+dim_t{0});

	LOG_DEBUG("Element's requested visibility: " << requested_visibility
		  << std::endl
		  << "   adjusted element position to: "
		  << element_pos);

	// Final set of sanity checks.

	if (element_pos.x > 0)
		element_pos.x=0;

	if (element_pos.y > 0)
		element_pos.y=0;

	if (element_pos.x < min_scroll_x)
		element_pos.x=min_scroll_x;

	if (element_pos.y < min_scroll_y)
		element_pos.y=min_scroll_y;

	LOG_DEBUG("Positioning element to: " << element_pos);
	peephole_element->impl->update_current_position(IN_THREAD,
							element_pos);
}

void peepholeObj::layoutmanager_implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	recalculate(IN_THREAD);
}


LIBCXXW_NAMESPACE_END
