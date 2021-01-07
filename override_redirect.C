/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "override_redirect.H"
#include "screen.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

override_redirectObj::override_redirectObj()=default;

override_redirectObj::~override_redirectObj()=default;

void override_redirectObj
::set_override_redirected_window_position(ONLY IN_THREAD)
{
	auto &wh=get_override_redirected_window_handler();

	// Wait until we're becoming visible, before moving us.
	//
	// request_visibility() will call us, when we are, and that's when
	// the show starts.

	if (!wh.data(IN_THREAD).requested_visibility)
		return;

	auto hv=wh.get_horizvert(IN_THREAD);

	rectangle r=*mpobj<rectangle>::lock{wh.current_position};

	// Opening bid: our preferred size.

	r.width=hv->horiz.preferred();
	r.height=hv->vert.preferred();

	auto screen_width=wh.screenref->impl->width_in_pixels();
	auto screen_height=wh.screenref->impl->height_in_pixels();

	recalculate_window_position(IN_THREAD,
				    r,
				    screen_width,
				    screen_height);

	// Whatever recalculate_window_position() wanted, adjust it
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
		mpobj<rectangle>::lock lock{wh.current_position};

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

	xcb_configure_window(IN_THREAD->info->conn, wh.id(),
			     configure_window_vals.mask(),
			     configure_window_vals.values().data());

#ifdef POPUP_SIZE_SET
	POPUP_SIZE_SET();
#endif

	most_recent_configuration=r;

	{
		mpobj<rectangle>::lock lock(wh.current_position);

		if (*lock == most_recent_configuration)
			return;
	}

	// Do not wait for the ConfigureNotify event, take the bull by the
	// horns. When it arrives it'll be ignored.

	wh.do_configure_notify_received(IN_THREAD,
					most_recent_configuration);

	wh.do_process_configure_notify(IN_THREAD);

}

LIBCXXW_NAMESPACE_END
