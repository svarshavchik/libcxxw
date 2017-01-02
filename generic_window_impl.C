/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "connection_info.H"
#include "screen.H"
#include "connection.H"
#include "values_and_mask.H"
#include <x/weakptr.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

generic_windowObj::implObj::implObj(const screen &screenref,
				    const ref<handlerObj> &handler,
				    xcb_window_t parent,
				    size_t nesting_level,
				    const rectangle &initial_position,
				    uint16_t window_class,
				    depth_t depth,
				    xcb_visualid_t visual,
				    xcb_colormap_t colormap,
				    const std::function<void (IN_THREAD_ONLY)> &configure_new_window)
	: elementObj::implObj(screenref, nesting_level, initial_position),
	  handler(handler)
{
	auto thread=handler->thread();

	auto width=initial_position.width;
	auto height=initial_position.height;

	// We can logically attempt to create a window with zero width or
	// height (empty container, for example). X will complain about
	// BadValue, so turn this into a tiny 1x1 window.

	if (width == 0 || height == 0)
		width=height=1;

	if (width == width.infinite() ||
	    height == height.infinite())
		throw EXCEPTION("Internal error, invalid initial display element size");

	auto window_id=handler->id();
	LOG_DEBUG("Constructor: " << objname() << " xid " << window_id);

	thread->run_as(RUN_AS,
		       [=,
			border_pixel=screenref->impl->xcb_screen->black_pixel]
		       (IN_THREAD_ONLY)
		       {
			       values_and_mask vm(XCB_CW_EVENT_MASK,
						  handler->current_events(IN_THREAD),
						  XCB_CW_COLORMAP,
						  colormap,
						  XCB_CW_BORDER_PIXEL,
						  border_pixel);

			       xcb_create_window(thread_->info->conn,
						 (depth_t::value_type)depth,
						 window_id,
						 parent,
						 (coord_t::value_type)initial_position.x,
						 (coord_t::value_type)initial_position.y,
						 (dim_t::value_type)width,
						 (dim_t::value_type)height,
						 2, // Border width
						 window_class,
						 visual,
						 vm.mask(),
						 vm.values().data());

			       thread_->window_handlers(IN_THREAD)
				       ->insert({window_id, handler});

			       handler->create_picture(IN_THREAD);

			       configure_new_window(IN_THREAD);
		       });
}

generic_windowObj::implObj::~implObj() noexcept
{
	auto thread=handler->thread();
	auto xid_obj=handler->xid_obj;

	auto window_id=handler->id();
	LOG_DEBUG("Destructor: " << objname() << " xid " << window_id);

	// Disconnect the handler from the thread's window handlers.

	thread->run_as(RUN_AS,
		       [=]
		       (IN_THREAD_ONLY)
		       {
			       thread_->window_handlers(IN_THREAD)
				       ->erase(window_id);
			       thread->destroyed_xids(IN_THREAD)
				       ->insert({window_id, xid_obj});
		       });

	// Attach a destructor callback to the handler object.

	// Note that there's a handler reference in window_handlers,
	// so the following can't happen until the above run_as() completes,
	// and until all other references to the handler go away. It is now
	// safe to execute xcb_destroy_window().
	//
	// Capturing screenref by value is intentional, so that it won't
	// get destroyed until the handler gets destroyed, taking the
	// colormap dud with it...

	handler->ondestroy
		([thread, window_id, screen=get_screen()]
		 {
			 thread->run_as
				 (RUN_AS,
				  [window_id, screen]
				  (IN_THREAD_ONLY)
				  {
					  LOG_DEBUG("Destroy: xid "
						    << window_id);
					  xcb_destroy_window(thread_->info
							     ->conn,
							     window_id);
				  });
		 });
}

bool generic_windowObj::implObj::get_frame_extents(dim_t &left,
						   dim_t &right,
						   dim_t &top,
						   dim_t &bottom) const
{
	mpobj<ewmh>::lock lock(get_screen()->get_connection()->impl->ewmh_info);

	return lock->get_frame_extents(left, right, top, bottom,
				       get_screen()->impl->screen_number,
				       handler->id());
}

void generic_windowObj::implObj::visibility_updated(IN_THREAD_ONLY, bool flag)
{
	if (flag)
		xcb_map_window(IN_THREAD->info->conn, handler->id());
	else
		xcb_unmap_window(IN_THREAD->info->conn, handler->id());
}

LIBCXXW_NAMESPACE_END
