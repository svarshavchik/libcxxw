#include "connection.H"
#include "connection_info.H"
#include "connection_thread.H"
#include "screen.H"
#include "messages.H"
#include "x/w/pictformat.H"
#include <x/exception.H>
#include <xcb/xcb_renderutil.h>

LIBCXXW_NAMESPACE_START

connectionObj::connectionObj(const std::experimental::string_view &display)
	: impl(ref<implObj>::create(display))
{
}

connectionObj::~connectionObj() noexcept
{
}

size_t connectionObj::default_screen() const
{
	return impl->info->default_screen;
}

/////////////////////////////////////////////////////////////////////////////

// The first step is to create the connection info handle.

connectionObj::implObj::implObj(const std::experimental::string_view &display)
	: implObj(ref<infoObj>::create(display))
{
}

// Once we have the connection info handle we can create the thread object.
// Immediately after xcb_connect_t(), check for errors, and return
// xcb_get_setup().

static inline const xcb_setup_t *get_setup(xcb_connection_t *conn)
{
	if (xcb_connection_has_error(conn))
		throw EXCEPTION(_("Display connection failed."));
	if (!xcb_render_util_query_version(conn))
		throw EXCEPTION(_("The display server does not support the X render extension"));


	return xcb_get_setup(conn);
}

connectionObj::implObj::implObj(const ref<infoObj> &info)
	: implObj(info,
		  get_setup(info->conn),
		  ref<threadObj>::create(info))
{
}

// Extract screens

static inline std::vector<ref<screenObj::implObj>>
get_screens(const ref<connectionObj::implObj::infoObj> &info,
	    const xcb_setup_t *setup)
{
	std::vector<ref<screenObj::implObj>> v;

	auto iter=xcb_setup_roots_iterator(setup);

	v.reserve(iter.rem);

	while (iter.rem)
	{
		v.push_back(ref<screenObj::implObj>::create(iter.data, info));
		xcb_screen_next(&iter);
	}

	return v;
}

connectionObj::implObj::implObj(const ref<infoObj> &info,
				const xcb_setup_t *setup,
				const ref<threadObj> &thread)
	: info(info),
	  thread(thread),
	  setup(*setup),
	  screens(get_screens(info, setup)),
	  render_info(info->conn)
{
}

connectionObj::implObj::~implObj()=default;

std::string connectionObj::implObj::get_error(const xcb_generic_error_t *e)
{
	std::ostringstream o;

	o.imbue(std::locale("C"));
	static const char bad[][16]={
		"Success",
		"Request",
		"Value",
		"Window",
		"Pixmap",
		"Atom",
		"Cursor",
		"Font",
		"Match",
		"Drawable",
		"Access",
		"Alloc",
		"Colormap",
		"GContext",
		"IDChoice",
		"Name",
		"Length",
		"Implementation",
	};

	o << "X protocol error: ";

	if ((int)e->error_code < sizeof(bad)/sizeof(bad[0]))
	{
		o << "Bad" << bad[(int)e->error_code];
	}
	else
	{
		o << "error " << (int)e->error_code;
	}

	o << ", resource=" << e->resource_id
	  << ", major=" << (int)e->major_code
	  << ", minor=" << (int)e->minor_code;
	return o.str();
}

LIBCXXW_NAMESPACE_END
