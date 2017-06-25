/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
#include "generic_window_handler.H"
#include "element_screen.H"
#include "screen.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

popupObj::handlerObj::handlerObj(IN_THREAD_ONLY,
				 const ref<generic_windowObj::handlerObj>
				 &parent)
	: generic_windowObj::handlerObj(IN_THREAD, parent->get_screen())
{
	// We are hereby initialized

	data(IN_THREAD).initialized=true;
}

popupObj::handlerObj::~handlerObj()=default;

void popupObj::handlerObj::frame_extents_updated(IN_THREAD_ONLY)
{
	set_popup_position(IN_THREAD);
}

void popupObj::handlerObj::horizvert_updated(IN_THREAD_ONLY)
{
	set_popup_position(IN_THREAD);
}

void popupObj::handlerObj::theme_updated(IN_THREAD_ONLY,
					 const defaulttheme &new_theme)
{
	generic_windowObj::handlerObj::theme_updated(IN_THREAD, new_theme);

	set_popup_position(IN_THREAD);
}

void popupObj::handlerObj::set_popup_position(IN_THREAD_ONLY)
{
	auto hv=get_horizvert(IN_THREAD);

	rectangle r=*mpobj<rectangle>::lock{current_position};

	// Opening bid: our preferred size.

	r.width=hv->horiz.preferred();
	r.height=hv->vert.preferred();

	auto screen_width=screenref->impl->width_in_pixels();
	auto screen_height=screenref->impl->height_in_pixels();

	recalculate_popup_position(IN_THREAD,
				   r,
				   screen_width,
				   screen_height);

	// Whatever recalculate_popup_position() wanted, adjust the popup
	// so that it fits on the screen.

	if (r.width > screen_width)
		r.width=screen_width;

	if (r.height > screen_height)
		r.height=screen_height;

	coord_t max_x=coord_t::truncate(screen_width-r.width);
	coord_t max_y=coord_t::truncate(screen_height-r.height);

	if (r.x < 0)
		r.x=0;
	if (r.y < 0)
		r.y=0;

	if (r.x > max_x)
		r.x=max_x;
	if (r.y > max_y)
		r.y=max_y;

	{
		mpobj<rectangle>::lock lock{current_position};

		if (r == *lock)
			return; // No change
	}

	auto w=r.width;
	auto h=r.height;

	// ConfigureWindow() does not like width and height of 0.
	if (w == 0 || h == 0)
		w=h=1;

	values_and_mask configure_window_vals
		(XCB_CONFIG_WINDOW_WIDTH,
		 (dim_t::value_type)w,

		 XCB_CONFIG_WINDOW_HEIGHT,
		 (dim_t::value_type)h,

		 XCB_CONFIG_WINDOW_X,
		 (coord_t::value_type)r.x,

		 XCB_CONFIG_WINDOW_Y,
		 (coord_t::value_type)r.y);

	xcb_configure_window(IN_THREAD->info->conn, id(),
			     configure_window_vals.mask(),
			     configure_window_vals.values().data());

#ifdef POPUP_SIZE_SET
	POPUP_SIZE_SET();
#endif

	// Do not wait for the ConfigureNotify event, take the bull by the
	// horns. When it arrives it'll be ignored.
	generic_windowObj::handlerObj::configure_notify(IN_THREAD, r);
}

void popupObj::handlerObj::configure_notify(IN_THREAD_ONLY,
					    const rectangle &r)
{
	// Ignoring the ConfigureNotify event, see?
}

LIBCXXW_NAMESPACE_END
