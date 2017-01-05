/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element.H"
#include "screen.H"
#include "connection_thread.H"
#include "generic_window_handler.H"
#include "draw_info.H"

LIBCXXW_NAMESPACE_START

#define THREAD get_window_handler().screenref->impl->thread

elementObj::implObj::implObj(size_t nesting_level,
			     const rectangle &initial_position)
	: data_thread_only
	  ({
		  nesting_level,
		  initial_position,
	  })
{
}

elementObj::implObj::~implObj()=default;

void elementObj::implObj::request_visibility(bool flag)
{
	// Set requested_visibility, make sure this is done in the connection
	// thread.

	// Sets requested_visibility, then adds the element to the
	// visibility_updated list.

	// The connection thread invokes update_visibility after processing
	// all messages.

	THREAD->run_as(RUN_AS,
		       [flag, me=elementimpl(this)]
		       (IN_THREAD_ONLY)
		       {
			       me->data(IN_THREAD).requested_visibility=flag;
			       IN_THREAD->visibility_updated(IN_THREAD)->insert(me);
		       });
}

void elementObj::implObj::update_visibility(IN_THREAD_ONLY)
{
	if (data(IN_THREAD).actual_visibility ==
	    data(IN_THREAD).requested_visibility)
		return;

	visibility_updated(IN_THREAD,
			   (
			    data(IN_THREAD).actual_visibility=
			    data(IN_THREAD).requested_visibility));
}

void elementObj::implObj::visibility_updated(IN_THREAD_ONLY, bool flag)
{
}

// Before rendering the element, we need to reset the window picture's
// clip region to current_position. Just in case.

class LIBCXX_INTERNAL elementObj::implObj::clip_region_set {

public:
	clip_region_set(IN_THREAD_ONLY,
			implObj &me,
			const draw_info &di)
	{
		// This now clips the subsequent draw operation to this
		// display element's viewport.
		di.window_picture->set_clip_rectangle(di.viewport);
	}
};

void elementObj::implObj::clear_to_color(IN_THREAD_ONLY,
					 const draw_info &di,
					 const std::set<rectangle> &areas)
{
	clear_to_color(IN_THREAD,
		       clip_region_set(IN_THREAD, *this, di),
		       di, areas);
}
void elementObj::implObj::clear_to_color(IN_THREAD_ONLY,
					 const clip_region_set &,
					 const draw_info &di,
					 const std::set<rectangle> &areas)
{
	for (auto area:areas)
	{
		// areas's (0, 0) is the (0, 0) coordinates of the viewport.
		area.x = (coord_squared_t::value_type)(area.x+di.viewport.x);
		area.y = (coord_squared_t::value_type)(area.y+di.viewport.y);

		di.window_picture->composite(di.window_background,
					     (dim_squared_t::value_type)
					     (area.x-di.background_x),
					     (dim_squared_t::value_type)
					     (area.y-di.background_y),
					     area);
	}
}

LIBCXXW_NAMESPACE_END
