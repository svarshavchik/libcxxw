/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_layoutmanager_impl.H"
#include "peephole/peepholed_element.H"
#include "catch_exceptions.H"
#include "container.H"
#include "run_as.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::peepholeObj::layoutmanager_implObj)
LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_START

void peepholeObj::layoutmanager_implObj
::update_scrollbars(IN_THREAD_ONLY,
		    const rectangle &element_pos,
		    const rectangle &current_position)
{
}

bool peepholeObj::layoutmanager_implObj
::process_button_event(IN_THREAD_ONLY,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	return false;
}

peepholeObj::layoutmanager_implObj
::layoutmanager_implObj(const ref<containerObj::implObj> &container_impl,
			peephole_style style,
			const peepholed &element_in_peephole)
	: layoutmanagerObj::implObj(container_impl),
	style(style),
	element_in_peephole(element_in_peephole)
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
	callback(element_in_peephole->get_peepholed_element());
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
	recalculate_with_requested_visibility(IN_THREAD, true);
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
	recalculate_with_requested_visibility(IN_THREAD, false);
}

void peepholeObj::layoutmanager_implObj
::recalculate_with_requested_visibility(IN_THREAD_ONLY, bool flag)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	peephole_element_impl->initialize_if_needed(IN_THREAD);

	auto &current_position=container_impl->get_element_impl()
		.data(IN_THREAD).current_position;

	// This is the peepholed element's current position
	auto &element_current_position=
		peephole_element_impl->data(IN_THREAD).current_position;

	// This is its advertised metrics
	auto element_horizvert=
		peephole_element_impl->get_horizvert(IN_THREAD);

	// Compute the peepholed element's new position. Start with its
	// current x/y coordinates, and tentatively size it at its maximum
	// requested size.
	rectangle element_pos{element_current_position.x,
			element_current_position.y,
			element_horizvert->horiz.maximum(),
			element_horizvert->vert.maximum()};

	// Stretch the combo-box's peephole to fill its alloted width.
	if (element_pos.width < current_position.width)
	{
		element_pos.width=current_position.width;
	}
	// If the maximum requested size exceeds the peephole's size,
	// truncate it down (this will also chop off the infinite() requested
	// size.

	if (element_pos.width > current_position.width)
		element_pos.width=current_position.width;
	if (element_pos.height > current_position.height)
		element_pos.height=current_position.height;

	// But don't go below the element's minimum size. This is what the
	// peephole is, after all.
	if (element_pos.width < element_horizvert->horiz.minimum())
		element_pos.width=element_horizvert->horiz.minimum();
	if (element_pos.height < element_horizvert->vert.minimum())
		element_pos.height=element_horizvert->vert.minimum();

	LOG_DEBUG("Peephole " << this
		  << ": current position is " << current_position
		  << std::endl
		  << "   element position is " << element_pos);

	// The "minimum" scroll position is technically accurate, but
	// misleading. The element in the peephole gets scrolled into view
	// by setting its starting coordinates to negative values (thus
	// bringing the bottom/right-most portions of the element into the
	// peepholed view). As such, min_scroll-x is actually the farthest
	// the element can be scrolled, at which point its bottom-right
	// corner is going to be aligned with the peephole's bottom-right
	// corner.
	//
	// But that's getting a bit ahead of the game. Firstly, unless the
	// element's size exceeds the peephole's size, it's a moot point, and
	// the "minimum" scroll position is 0 -- it won't scroll, it fits
	// entirely into the peephole.

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

	if (flag)
	{
		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+
				      requested_visibility.width,
				      requested_visibility.y+
				      requested_visibility.height
				      );

		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+dim_t{0},
				      requested_visibility.y+
				      requested_visibility.height
				      );

		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+
				      requested_visibility.width,
				      requested_visibility.y+dim_t{0});

		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+dim_t{0},
				      requested_visibility.y+dim_t{0});
	}

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
	peephole_element_impl->update_current_position(IN_THREAD,
						       element_pos);
	update_scrollbars(IN_THREAD, element_pos, current_position);
}

void peepholeObj::layoutmanager_implObj
::update_horizontal_scroll(IN_THREAD_ONLY, dim_t offset)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t x=coord_t::truncate(offset);

	cur_pos.x= -x;
	peephole_element_impl->update_current_position(IN_THREAD, cur_pos);
}

void peepholeObj::layoutmanager_implObj
::update_vertical_scroll(IN_THREAD_ONLY, dim_t offset)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t y=coord_t::truncate(offset);

	cur_pos.y= -y;
	peephole_element_impl->update_current_position(IN_THREAD, cur_pos);
}

void peepholeObj::layoutmanager_implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	recalculate(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
