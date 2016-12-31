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
#include <x/weakptr.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

generic_windowObj::implObj::implObj(const screen &screenref,
				    const ref<handlerObj> &handler,
				    xcb_window_t parent,
				    const rectangle &initial_position,
				    const values_and_mask &vm,
				    uint16_t window_class,
				    depth_t depth,
				    xcb_visualid_t visual)
	: elementimplObj(initial_position),
	  handler(handler), screenref(screenref)
{
	auto thread=handler->thread();
	auto conn=handler->conn();

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

	connection_threadObj::shared_data_t::lock lock(thread->shared_data);

	xcb_create_window(conn->conn,
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
			  vm.mask,
			  vm.values.data());

	lock->window_handlers.insert({window_id, handler});
}

generic_windowObj::implObj::~implObj() noexcept
{
	auto thread=handler->thread();
	auto conn=handler->conn();
	auto xid_obj=handler->xid_obj;

	auto window_id=handler->id();
	LOG_DEBUG("Destructor: " << objname() << " xid " << window_id);

	connection_threadObj::shared_data_t::lock lock(thread->shared_data);

	lock->window_handlers.erase(window_id);
	lock->destroyed_xids.insert({window_id, xid_obj});

	handler->ondestroy([info=thread->info, window_id]
			   {
				   LOG_DEBUG("Destroy: xid " << window_id);
				   xcb_destroy_window(info->conn, window_id);
			   });
}

bool generic_windowObj::implObj::get_frame_extents(dim_t &left,
						   dim_t &right,
						   dim_t &top,
						   dim_t &bottom) const
{
	mpobj<ewmh>::lock lock(screenref->conn()->impl->ewmh_info);

	return lock->get_frame_extents(left, right, top, bottom,
				       screenref->impl->screen_number,
				       handler->id());
}

LIBCXXW_NAMESPACE_END
