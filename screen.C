/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen.H"
#include "connection_thread.H"
#include "screen_depthinfo.H"
#include "xid_t.H"
#include <x/mpobj.H>
#include <x/weakptr.H>
#include <x/refptr_traits.H>
#include <x/exception.H>
#include "messages.H"

LIBCXXW_NAMESPACE_START

LOG_CLASS_INIT(screenObj::implObj);

typedef mpobj< weakptr<refptr_traits<connection>::ptr_t> > default_connection_t;

static default_connection_t default_connection;

// Autocreate the default connection to the server.

static connection get_default_connection()
{
	default_connection_t::lock lock{default_connection};

	auto conn=lock->getptr();

	if (!conn.null())
		return conn;

	auto newconn=connection::create();

	*lock=newconn;
	return newconn;
}

screen screenBase::create()
{
	return create(get_default_connection());
}

screen screenBase::create(size_t n)
{
	return create(get_default_connection(), n);
}

screen screenBase::create(const connection &conn)
{
	return create(conn, conn->default_screen());
}

screen screenBase::create(const connection &conn, size_t n)
{
	if (n >= conn->impl->screens.size())
		throw EXCEPTION(_("Requested screen does not exist"));

	return ptrrefBase::objfactory<screen>::create(conn->impl->screens
						      .at(n), conn);
}

screenObj::screenObj(const ref<implObj> &impl,
		     const connection &conn)
	: impl(impl),
	  screen_depths(impl->screen_depths),
	  connref(conn)
{
}

screenObj::~screenObj()
{
}

ref<obj> screenObj::mcguffin() const
{
	return get_connection()->mcguffin();
}

///////////////////////////////////////////////////////////////////////////////
//
// Extended Window Manager Hints.

bool screenObj::get_frame_extents(dim_t &left,
				  dim_t &right,
				  dim_t &top,
				  dim_t &bottom) const
{
	mpobj<ewmh>::lock lock(get_connection()->impl->ewmh_info);

	return lock->get_frame_extents(left, right, top, bottom,
				       impl->screen_number,
				       impl->xcb_screen->root);
}

rectangle screenObj::get_workarea() const
{
	rectangle ret{coord_t(0),
			coord_t(0), width_in_pixels(), height_in_pixels()};

	mpobj<ewmh>::lock lock(get_connection()->impl->ewmh_info);

	lock->get_workarea(impl->screen_number, ret);

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
//
// Screen metrics


dim_t screenObj::width_in_pixels() const
{
	return impl->width_in_pixels();
}

dim_t screenObj::height_in_pixels() const
{
	return impl->height_in_pixels();
}

dim_t screenObj::width_in_millimeters() const
{
	return impl->width_in_millimeters();
}

dim_t screenObj::height_in_millimeters() const
{
	return impl->height_in_millimeters();
}

///////////////////////////////////////////////////////////////////////////////

screenObj::implObj::toplevelwindow_colormapObj
::toplevelwindow_colormapObj(const connection_thread &thread,
			     xcb_window_t window,
			     xcb_visualid_t visual)
	: xid_t<xcb_colormap_t>(thread)
{
	xcb_create_colormap(thread->info->conn,
			    XCB_COLORMAP_ALLOC_NONE,
			    xid_obj->id_,
			    window,
			    visual);
}

screenObj::implObj::toplevelwindow_colormapObj
::~toplevelwindow_colormapObj()
{
	xcb_free_colormap(thread()->info->conn, xid_obj->id_);
}

///////////////////////////////////////////////////////////////////////////////

screenObj::implObj::implObj(const xcb_screen_t *xcb_screen,
			    size_t screen_number,
			    const render &render_info,
			    const vector<const_ref<depthObj>> &screen_depths,
			    const screen::base::visual_t &toplevelwindow_visual,
			    const const_pictformat &toplevelwindow_pictformat,
			    const connection_thread &thread)
	: xcb_screen(xcb_screen),
	  screen_number(screen_number),
	  thread(thread),
	  toplevelwindow_visual(toplevelwindow_visual),
	  toplevelwindow_pictformat(toplevelwindow_pictformat),
	  toplevelwindow_colormap(ref<toplevelwindow_colormapObj>
				  ::create(thread, xcb_screen->root,
					   toplevelwindow_visual->impl
					   ->visual_id)),
	  screen_depths(screen_depths)
{
}

screenObj::implObj::~implObj()=default;

depth_t screenObj::implObj::root_depth() const
{
	return depth_t(xcb_screen->root_depth);
}

screen::base::visual_t screenObj::implObj::root_visual() const
{
	return root_visual(xcb_screen, screen_depths);
}

screen::base::visual_t screenObj::implObj
::root_visual(const xcb_screen_t *xcb_screen,
	      const vector<const_ref<depthObj>> &screen_depths)
{
	depth_t d{xcb_screen->root_depth};

	for (const auto &depth:*screen_depths)
	{
		if (depth->depth != d)
			continue;

		for (const auto &v:depth->visuals)
		{
			if (v->impl->visual_id == xcb_screen->root_visual)
				return v;
		}
	}
	throw EXCEPTION("Cannot find root visual");
}

LIBCXXW_NAMESPACE_END
