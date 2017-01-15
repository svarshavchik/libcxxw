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
#include "x/w/element_state.H"
#include "x/callback_list.H"

LIBCXXW_NAMESPACE_START

#define THREAD get_window_handler().screenref->impl->thread

elementObj::implObj::implObj(size_t nesting_level,
			     const rectangle &initial_position)
	: data_thread_only
	  ({
	      initial_position,
	  }),
	  nesting_level(nesting_level)
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

	// Notify handlers that we're about to show or hide this element.

	invoke_element_state_updates(IN_THREAD,
				     data(IN_THREAD).requested_visibility
				     ? element_state::before_showing
				     : element_state::before_hiding);
	visibility_updated(IN_THREAD,
			   (
			    data(IN_THREAD).actual_visibility=
			    data(IN_THREAD).requested_visibility));

	// Notify handlers that we just shown or hidden this element.

	invoke_element_state_updates(IN_THREAD,
				     data(IN_THREAD).requested_visibility
				     ? element_state::after_showing
				     : element_state::after_hiding);
}

void elementObj::implObj::visibility_updated(IN_THREAD_ONLY, bool flag)
{
}

ref<obj> elementObj::implObj
::do_on_state_update(const element_state_update_handler_t &handler)
{
	// It's ok, create_mcguffin() can be safely used by any thread.
	auto mcguffin=data_thread_only.update_handlers->create_mcguffin();

	THREAD->run_as(RUN_AS,
		       [mcguffin, handler,
			me=ref<elementObj::implObj>(this)]
		       (IN_THREAD_ONLY)
		       {
			       // Install the new handler in the connection
			       // thread, then send it the current_state
			       mcguffin->install(handler);

			       handler->invoke(me->create_element_state
					       (IN_THREAD,
						element_state::current_state));
		       });

	return mcguffin;
}

void elementObj::implObj::current_position_updated(IN_THREAD_ONLY,
						   const rectangle &r)
{
	auto &current_data=data(IN_THREAD);

	if (r == current_data.current_position)
		return;

	current_data.current_position=r;
	invoke_element_state_updates(IN_THREAD,
				     element_state::current_state);
}

element_state elementObj::implObj
::create_element_state(IN_THREAD_ONLY,
		       element_state::state_update_t element_state_for)
{
	auto &current_data=data(IN_THREAD);

	return element_state{
		element_state_for,
		current_data.actual_visibility,
		current_data.current_position
			};
}

void elementObj::implObj
::invoke_element_state_updates(IN_THREAD_ONLY,
			       element_state::state_update_t reason)
{
	data(IN_THREAD).update_handlers->invoke(create_element_state
						(IN_THREAD, reason));
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
