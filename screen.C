#include "screen.H"
#include "connection_info.H"
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

screenObj::~screenObj() noexcept
{
}

ref<obj> screenObj::mcguffin() const
{
	return conn()->mcguffin();
}

///////////////////////////////////////////////////////////////////////////////
//
// Extended Window Manager Hints.

bool screenObj::get_frame_extents(dim_t &left,
				  dim_t &right,
				  dim_t &top,
				  dim_t &bottom) const
{
	mpobj<ewmh>::lock lock(conn()->impl->ewmh_info);

	return lock->get_frame_extents(left, right, top, bottom,
				       impl->screen_number,
				       impl->xcb_screen->root);
}

rectangle screenObj::get_workarea() const
{
	rectangle ret{coord_t(0),
			coord_t(0), width_in_pixels(), height_in_pixels()};

	mpobj<ewmh>::lock lock(conn()->impl->ewmh_info);

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

screenObj::implObj::implObj(const xcb_screen_t *xcb_screen,
			    size_t screen_number,
			    const render &render_info,
			    const ref<connectionObj::implObj::infoObj> &info)
	: xcb_screen(xcb_screen),
	  screen_number(screen_number),
	  info(info),
	  screen_depths(create_screen_depths(xcb_screen,
					     render_info,
					     screen_number))
{
}

screenObj::implObj::~implObj() noexcept=default;

LIBCXXW_NAMESPACE_END
