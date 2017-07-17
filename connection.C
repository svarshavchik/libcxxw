/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection.H"
#include "connection_info.H"
#include "connection_thread.H"
#include "screen.H"
#include "messages.H"
#include "pictformat.H"
#include "defaulttheme.H"
#include "fonts/cached_font.H"
#include "fonts/fontcollection.H"
#include "fonts/fontid_t_hash.H"
#include "xim/ximxtransport.H"
#include "x/w/pictformat.H"
#include <x/exception.H>
#include <xcb/xcb_renderutil.h>

LIBCXXW_NAMESPACE_START

connectionObj::connectionObj(const std::string_view &display)
	: impl(ref<implObj>::create(display))
{
}

connectionObj::~connectionObj()
{
}

size_t connectionObj::screens() const
{
	return impl->screens.size();
}

size_t connectionObj::default_screen() const
{
	return impl->info->default_screen;
}

ref<obj> connectionObj::mcguffin() const
{
	return impl->info;
}

void connectionObj::on_disconnect(const std::function<void ()> &callback)
{
	return impl->thread->install_on_disconnect(callback);
}

const_pictformat connectionObj::find_alpha_pictformat_by_depth(depth_t d) const
{
	return impl->render_info.find_alpha_pictformat_by_depth(d);
}

/////////////////////////////////////////////////////////////////////////////

// The first step is to create the connection info handle.

connectionObj::implObj::implObj(const std::string_view &display)
	: implObj(connection_info::create(display))
{
}

// Once we have the connection info handle we can create the thread object.
// Immediately after xcb_connect_t(), check for errors, and return
// xcb_get_setup().

connectionObj::implObj::implObj(const connection_info &info)
	: implObj(info,
		  xcb_get_setup(info->conn),
		  connection_thread::create(info))
{
}

// Extract screens

// This is called by the constructor, to create the screens.

static inline std::vector<ref<screenObj::implObj>>
get_screens(const connection_thread &thread,
	    const render &render_info,
	    const xcb_setup_t *setup)
{
	std::vector<ref<screenObj::implObj>> v;
	size_t screen_number=0;

	auto iter=xcb_setup_roots_iterator(setup);

	v.reserve(iter.rem);

	if (iter.rem == 0)
		throw EXCEPTION("No screens reported by the display server?");

	// Peek at screen #0, in order to construct the default theme
	// configuration.

	auto theme_config=defaulttheme::base::get_config(iter.data, thread);

	while (iter.rem)
	{
		auto xcb_screen=iter.data;

		// Calculate the visual for each screen's top level windows.

		// Start with the root window's advertised visual, then search
		// for a compatible visual that has an alpha channel.

		auto screen_depths=screenObj::implObj
			::create_screen_depths(xcb_screen,
					       render_info,
					       screen_number);

		auto root_visual=screenObj::implObj::root_visual(xcb_screen,
								 screen_depths);
		auto root_pictformat=root_visual->render_format;

		auto compatible_pictformats=
			render_info.compatible_pictformats(root_pictformat);

		for (const auto &candidate:compatible_pictformats)
		{
			if (candidate->alpha_depth
			    > root_pictformat->alpha_depth)
				root_pictformat=candidate;
		}

		bool found=false;
		for (const auto &depth: *screen_depths)
		{
			if (depth->depth != root_pictformat->depth)
				continue;

			for (const auto &v:depth->visuals)
			{
				// This pictformat should be somewhere here

				if (v->render_format->impl->id !=
				    root_pictformat->impl->id)
					continue;

				root_visual=v;
				found=true;
				break;
			}
			if (found)
				break;
		}

		auto new_theme=defaulttheme::create(xcb_screen, theme_config);

		auto s=ref<screenObj::implObj>::create(xcb_screen,
						       screen_number++,
						       render_info,
						       screen_depths,
						       new_theme,
						       root_visual,
						       root_pictformat,
						       thread);

		new_theme->load(theme_config, s);

		v.push_back(s);
		xcb_screen_next(&iter);
	}

	return v;
}

// This is the callback that gets invoked when the CXXWTHEMES property is
// updated.
//
// We take care of loading and installing the new theme into each
// screen object. The connection thread takes care of notifying all windows.

static inline void update_themes(const std::vector<ref<screenObj::implObj>> &s,
				 const connection_thread &thread)
{
	auto theme_config=defaulttheme::base::get_config(s.at(0)->xcb_screen,
							 thread);

	for (const auto &screen:s)
	{
		auto new_theme=defaulttheme::create(screen->xcb_screen,
						    theme_config);

		new_theme->load(theme_config, screen);
		current_theme_t::lock lock(screen->current_theme);

		*lock=new_theme;
	}
}

////////////////////////////////////////////////////////////////////////////
//
// Wrapper to start and stop the connection thread.

connectionObj::implObj::connection_wrapper
::connection_wrapper(const connection_thread &thread)
	: connection_thread(thread),
	  running_thread(start_threadmsgdispatcher(thread))
{
}

connectionObj::implObj::connection_wrapper
::~connection_wrapper()
{
	(*this)->stop();
}

bool connectionObj::implObj::is_connection_thread() const
{
	return thread.running_thread->get_id() == std::this_thread::get_id();
}

connectionObj::implObj::implObj(const connection_info &info,
				const xcb_setup_t *setup,
				const connection_thread &thread)
	: info(info),
	  render_info(info),
	  ewmh_info(info->conn),
	  keysyms_info_thread_only(info->conn),
	  setup(*setup),
	  thread(thread),
	  screens(get_screens(thread, render_info, setup)),
	  font_cache(font_cache_t::create()),
	  sorted_font_cache(sorted_font_cache_t::create())
{
	thread->set_theme_changed_callback(screens.at(0)->xcb_screen->root,
					   [screens=this->screens, thread]
					   {
						   update_themes(screens,
								 thread);
					   });
}

connectionObj::implObj::~implObj()
{
}

std::string connection_error(const xcb_generic_error_t *e)
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

ximxtransport connectionObj::implObj::get_ximxtransport(const connection &conn)
{
	xim_t::lock lock{xim};

	auto p=lock->getptr();

	if (p)
		return p;

	//! Create an X Input Method client on screen 0.

	auto new_xim=ximxtransport::create(screen::create(conn, 0));

	*lock=new_xim;

	return new_xim;
}

LIBCXXW_NAMESPACE_END
