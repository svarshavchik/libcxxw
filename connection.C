/*
** Copyright 2017-2021 Double Precision, Inc.
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
#include "configfile.H"
#include "fonts/cached_font.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "fonts/fontid_t_hash.H"
#include "xim/ximxtransport.H"
#include "x/w/pictformat.H"
#include <x/exception.H>
#include <xcb/xcb_renderutil.h>
#include <algorithm>

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

void connectionObj::on_disconnect(const functionref<void ()> &callback)
{
	return impl->thread->install_on_disconnect(callback);
}

const_pictformat connectionObj::find_alpha_pictformat_by_depth(depth_t d) const
{
	return impl->render_info.find_alpha_pictformat_by_depth(d);
}

void connectionObj::in_thread(const functionref<void (THREAD_CALLBACK)> &cb)
	const
{
	impl->thread->run_as([cb]
			     (ONLY IN_THREAD)
			     {
				     cb(IN_THREAD);
			     });
}

void connectionObj::in_thread_idle(const functionref<void (THREAD_CALLBACK)>
				   &cb)
	const
{
	impl->thread->run_as([cb]
			     (ONLY IN_THREAD)
			     {
				     IN_THREAD->idle_callbacks(IN_THREAD)
					     ->push_back(cb);
			     });
}

bool connectionObj::selection_has_owner(const std::string_view &selection)
	const
{
	auto selection_atom=impl->info->get_atom(selection);

	if (selection_atom == XCB_NONE)
		return false;

	return impl->info->get_selection_owner(selection_atom) != XCB_NONE;
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

static std::vector<ref<screenObj::implObj>>
get_screens(const connection_thread &thread,
	    const render &render_info,
	    mpobj<ewmh> &ewmh_info,
	    const xcb_setup_t *setup,
	    const std::string &theme_property,
	    bool &theme_exception)
{
	std::vector<ref<screenObj::implObj>> v;
	size_t screen_number=0;

	auto iter=xcb_setup_roots_iterator(setup);

	v.reserve(iter.rem);

	// Peek at screen #0, in order to construct the default theme
	// configuration.

	xcb_screen_t *screen_0=iter.data;

	// If get_config() throws an exception, try again.
	theme_exception=true;
	auto theme_config=defaulttheme::base::get_config(theme_property);
	theme_exception=false;

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

		auto root_pictformat=root_visual->render_format
			->compatible_pictformat_with_alpha_channel();

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

		std::unordered_set<std::string> supported;

		{
			mpobj<ewmh>::lock lock{ewmh_info};

			auto supported_atoms=lock->get_supported(screen_number);

			for (auto a:supported_atoms)
				supported.insert(thread->info
						 ->get_atom_name(a));

		}

		// If the constructor throws an exception, try again.

		theme_exception=true;
		auto new_theme=defaulttheme::create(xcb_screen, theme_config);
		theme_exception=false;

		auto s=ref<screenObj::implObj>::create(xcb_screen,
						       screen_number++,
						       render_info,
						       screen_depths,
						       supported,
						       new_theme,
						       root_visual,
						       root_pictformat,
						       thread);

		v.push_back(s);
		xcb_screen_next(&iter);
	}

	// If we did not find the CXXWTHEME property, now is
	// the time to set it.

	if (theme_property.empty())
		load_cxxwtheme_property
			(screen_0,
			 thread,
			 theme_config.themename,
			 theme_config.themescale * 100,
			 theme_config.enabled_theme_options
			 );

	return v;
}

static inline std::vector<ref<screenObj::implObj>>
get_screens(const connection_thread &thread,
	    const render &render_info,
	    mpobj<ewmh> &ewmh_info,
	    const xcb_setup_t *setup)
{
	auto iter=xcb_setup_roots_iterator(setup);

	if (iter.rem == 0)
		throw EXCEPTION("No screens reported by the display server?");

	xcb_screen_t *screen_0=iter.data;

	auto property=defaulttheme::base::cxxwtheme_property(screen_0, thread);

	std::vector<ref<screenObj::implObj>> screens;

	bool theme_exception=false;

	try {
		screens=get_screens(thread, render_info, ewmh_info, setup,
				    property,
				    theme_exception);
	} catch (const exception &e)
	{
		if (!theme_exception)
			throw;
		e->caught();

		// Ok, there was a problem initializing the theme,
		// fallback to the default theme.

		property="100:default";
		screens=get_screens(thread, render_info, ewmh_info, setup,
				    property,
				    theme_exception);
	}

	return screens;
}

// This is the callback that gets invoked when the CXXWTHEMES property is
// updated.
//
// We take care of loading and installing the new theme into each
// screen object. The connection thread takes care of notifying all windows.

static void update_themes(ONLY IN_THREAD,
			  const std::vector<ref<screenObj::implObj>> &s,
			  const defaulttheme::base::config &theme_config)
{
	// Construct all the themes first.

	std::vector<defaulttheme> new_themes;

	new_themes.reserve(s.size());

	for (const auto &screen:s)
	{
		auto new_theme=defaulttheme::create(screen->xcb_screen,
						    theme_config);

		new_themes.push_back(new_theme);
	}

	// Now that all the themes have been built, install them.

	auto theme_iter=new_themes.begin();

	for (const auto &screen:s)
	{
		screen->update_current_theme(IN_THREAD, *theme_iter);
		++theme_iter;
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
	  screens(get_screens(thread, render_info, ewmh_info, setup)),
	  font_cache(font_cache_t::create()),
	  sorted_font_cache(sorted_font_cache_t::create())
{
	thread->set_theme_changed_callback
		(screens.at(0)->xcb_screen->root,
		 [screens=this->screens]
		 (ONLY IN_THREAD)
		 {
			 auto screen_0=screens.at(0)->xcb_screen;

			 auto property=defaulttheme::base
				 ::cxxwtheme_property(screen_0, IN_THREAD);
			 auto theme_config=defaulttheme::base
				 ::get_config(property);

			 update_themes(IN_THREAD, screens, theme_config);
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

void connectionObj::implObj::set_wm_icon(xcb_window_t wid,
					 const std::vector<uint32_t> &raw_data)
{
	mpobj<ewmh>::lock lock{ewmh_info};

	lock->set_wm_icon(wid, raw_data);
}

//////////////////////////////////////////////////////////////////////////////

void connectionObj::set_and_save_theme(ONLY IN_THREAD,
				       const std::string &identifier,
				       int factor,
				       const enabled_theme_options_t
				       &enabled_options)

{
	set_theme(IN_THREAD,
		  identifier, factor, enabled_options, false, {"_"});
	save_config(identifier, factor, enabled_options);
}

void connectionObj::set_theme(ONLY IN_THREAD,
			      const std::string &identifier,
			      int factor,
			      const enabled_theme_options_t &enabled_options,
			      bool this_connection_only,
			      const std::unordered_set<std::string>
			      &updated_settings)
{
	auto available_themes=connection::base::available_themes();

	if (std::find_if(available_themes.begin(),
			 available_themes.end(),
			 [&]
			 (const auto &t)
			 {
				 return t.identifier == identifier;
			 }) == available_themes.end())
		throw EXCEPTION(gettextmsg(_("No such theme: %1%"),
					   identifier));

	if (factor < SCALE_MIN || factor > SCALE_MAX)
		throw EXCEPTION(gettextmsg(_("Theme scaling factor must be between %1% and %2%"),
					   SCALE_MIN, SCALE_MAX));

	if (this_connection_only) // cxxwtheme
	{
		auto config=defaulttheme::base::get_config(identifier,
							   factor/100.0,
							   enabled_options);

		update_themes(IN_THREAD, impl->screens, config);

		for (const auto &setting:updated_settings)
			if (*setting.c_str() != '_')
			{
				IN_THREAD->theme_updated(IN_THREAD);
				break;
			}
		return;

	}

	load_cxxwtheme_property(impl->screens.at(0)->xcb_screen,
				impl->thread,
				identifier,
				factor,
				enabled_options);
}

LIBCXXW_NAMESPACE_END
