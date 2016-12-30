#include <x/logger.H>
#include <x/exception.H>

#include "ewmh.H"
#include "returned_pointer.H"
#include "connection.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::ewmh);

LIBCXXW_NAMESPACE_START

ewmh::ewmh(xcb_connection_t *conn) : ewmh_available(false)
{
	returned_pointer<xcb_generic_error_t *>e;

	auto cookies=xcb_ewmh_init_atoms(conn, this);

	if (!cookies)
		return;

	auto res=xcb_ewmh_init_atoms_replies(this, cookies, e.addressof());

	if (!res)
	{
		if (e)
		{
			LOG_ERROR(connectionObj::implObj::get_error(e));
		}
		xcb_ewmh_connection_wipe(this);
		return;
	}

	ewmh_available=true;
}

ewmh::~ewmh()
{
	if (ewmh_available)
		xcb_ewmh_connection_wipe(this);
}

bool ewmh::get_workarea(size_t screen_number, rectangle &ret)
{
	if (!ewmh_available)
		return false;

	// First, determine the screen's current desktop.

	uint32_t n=0;

	{
		returned_pointer<xcb_generic_error_t *> error;

		if (xcb_ewmh_get_current_desktop_reply
		    (this,
		     xcb_ewmh_get_current_desktop(this, screen_number),
		     &n, error.addressof()))
		{
			LOG_DEBUG("get_workarea: screen number is " << n);
		}
		else
		{
			if (error)
				throw EXCEPTION(connectionObj::implObj::get_error(error));
		}
	}

	// Now, get the work areas.

	returned_pointer<xcb_generic_error_t *> error;

	xcb_ewmh_get_workarea_reply_t wa;

	if (xcb_ewmh_get_workarea_reply(this,
					xcb_ewmh_get_workarea(this,
							      screen_number),
					&wa, error.addressof()))
	{
		LOG_DEBUG("get_workarea: there are " << wa.workarea_len
			  << " workareas");

		if (n < wa.workarea_len)
		{
			ret.x=wa.workarea[n].x;
			ret.y=wa.workarea[n].y;
			ret.width=wa.workarea[n].width;
			ret.height=wa.workarea[n].height;
		}
		xcb_ewmh_get_workarea_reply_wipe(&wa);
	}
	else
	{
		if (error)
			throw EXCEPTION(connectionObj::implObj::get_error(error));
	}

	return true;
}

bool ewmh::get_frame_extents(dim_t &left,
			     dim_t &right,
			     dim_t &top,
			     dim_t &bottom,
			     size_t screen_number,
			     xcb_window_t window_id)
{
	left=right=top=bottom=0;

	if (get_frame_extents(left, right, top, bottom, window_id))
		return true;

	xcb_ewmh_request_frame_extents(this,
				       screen_number,
				       window_id);

	return get_frame_extents(left, right, top, bottom, window_id);
}

bool ewmh::get_frame_extents(dim_t &left,
			     dim_t &right,
			     dim_t &top,
			     dim_t &bottom,
			     xcb_window_t window_id)
{
	LOG_DEBUG("Requesting frame extents");

	returned_pointer<xcb_generic_error_t *> error;

	xcb_ewmh_get_extents_reply_t frame_extents;

	if (xcb_ewmh_get_frame_extents_reply
	    (this,
	     xcb_ewmh_get_frame_extents(this, window_id),
	     &frame_extents,
	     error.addressof()))
	{
		left=frame_extents.left;
		right=frame_extents.right;
		top=frame_extents.top;
		bottom=frame_extents.bottom;
		return true;
	}

	if (error)
		throw EXCEPTION(connectionObj::implObj::get_error(error));
	return false;
}

LIBCXXW_NAMESPACE_END
