/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window_handler.H"
#include "connection_thread.H"
#include "x/w/impl/draw_info.H"
#include "x/w/impl/layoutmanager.H"
#include "shared_handler_data.H"
#include "batch_queue.H"
#include "icon.H"
#include "icon_images_vector_element.H"
#include "pixmap_extractor.H"
#include "pixmap_with_picture.H"
#include "screen.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "busy.H"
#include "catch_exceptions.H"
#include "inherited_visibility_info.H"

LIBCXXW_NAMESPACE_START

main_windowObj::handlerObj::handlerObj(ONLY IN_THREAD,
				       const screen &parent_screen,
				       const std::optional<rectangle>
				       &suggested_position,
				       const color_arg &background_color)
	: superclass_t({},
		       IN_THREAD, parent_screen,
		       background_color,
		       shared_handler_data::create(),
		       0),
	  on_delete_callback_thread_only([](THREAD_CALLBACK,
					    const auto &ignore) {}),
	  suggested_position_thread_only{suggested_position}
{
	// Set WM_PROTOCOLS to WM_DELETE_WINDOW -- we handle the window
	// close request ourselves.

	xcb_atom_t protocols[2];

	protocols[0]=conn()->atoms_info.wm_delete_window;

	int n=1;

	{
		mpobj<ewmh>::lock lock{screenref->get_connection()
				->impl->ewmh_info};

		if (lock->ewmh_available)
		{
			protocols[1]=lock->_NET_WM_PING;
			++n;
		}
	}

	change_property(IN_THREAD,
			XCB_PROP_MODE_REPLACE,
			conn()->atoms_info.wm_protocols,
			XCB_ATOM_ATOM,
			sizeof(xcb_atom_t)*8,
			n,
			protocols);
}

main_windowObj::handlerObj::~handlerObj()=default;

main_windowptr main_windowObj::handlerObj::get_main_window()
{
	return public_object.getptr();
}

const char *main_windowObj::handlerObj ::default_wm_class_instance() const
{
	return "main";
}

void main_windowObj::handlerObj
::client_message_event(ONLY IN_THREAD,
		       const xcb_client_message_event_t *event)
{
	if (event->type == IN_THREAD->info->atoms_info.wm_protocols)
	{
		update_user_time(IN_THREAD);

		if (event->data.data32[0] ==
		    IN_THREAD->info->atoms_info.wm_delete_window)
		{
			if (is_input_busy())
				return;

			try {
				busy_impl yes_i_am{*this};

				on_delete_callback(IN_THREAD)(IN_THREAD,
							      yes_i_am);
			} REPORT_EXCEPTIONS(this);

			return;
		}


		mpobj<ewmh>::lock lock{screenref->get_connection()
				->impl->ewmh_info};

		if (lock->client_message(event,
					 screenref->impl->xcb_screen->root))
			return;
	}
	superclass_t::client_message_event(IN_THREAD, event);
}

void main_windowObj::handlerObj::horizvert_updated(ONLY IN_THREAD)
{
	superclass_t::horizvert_updated(IN_THREAD);

	auto p=get_horizvert(IN_THREAD);

	preferred_width(IN_THREAD)=p->horiz.preferred();
	preferred_height(IN_THREAD)=p->vert.preferred();

	if (data(IN_THREAD).reported_inherited_visibility)
		update_size_hints(IN_THREAD);
}

static std::vector<icon> create_icons(drawableObj::implObj &me,
				      const std::vector<std::tuple<std::string,
				      dim_t, dim_t>> &icons)
{
	std::vector<icon> v;

	v.reserve(icons.size());

	for (const auto &i:icons)
		v.push_back(me.create_icon_pixels(std::get<0>(i),
						  render_repeat::none,
						  std::get<1>(i),
						  std::get<2>(i),
						  icon_scale::nomore));
	return v;
}

void main_windowObj::handlerObj
::set_inherited_visibility(ONLY IN_THREAD,
			   inherited_visibility_info &visibility_info)
{
	if (visibility_info.flag)
	{
		// If an icon was not set yet, set the default one.

		if (!wm_icon_set(IN_THREAD))
			try {
				install_window_theme_icon
					(IN_THREAD,
					 create_icons
					 (*this, {
						 {"mainwindow-icon", 16, 16},
						 {"mainwindow-icon", 24, 24},
						 {"mainwindow-icon", 32, 32},
						 {"mainwindow-icon", 48, 48}
					 }));
			} CATCH_EXCEPTIONS;
		update_size_hints(IN_THREAD);
	}
	else
	{
		// Time for suggestions is over.
		suggested_position(IN_THREAD).reset();
	}
	generic_windowObj::handlerObj
		::set_inherited_visibility(IN_THREAD, visibility_info);
}

void main_windowObj::handlerObj::update_size_hints(ONLY IN_THREAD)
{
	auto hints=compute_size_hints(IN_THREAD);

	auto conn=screenref->get_connection()->impl;

	// Before making ourselves visible, if we have a suggested position
	// explicitly send it via ConfigureWindow(). Apparently the hints
	// are not enough for XFCE.

	if (!data(IN_THREAD).reported_inherited_visibility &&
	    (hints.flags & XCB_ICCCM_SIZE_HINT_US_POSITION))
	{
		values_and_mask configure_window_vals
			(XCB_CONFIG_WINDOW_X, hints.x,
			 XCB_CONFIG_WINDOW_Y, hints.y);

		xcb_configure_window(IN_THREAD->info->conn, id(),
				     configure_window_vals.mask(),
				     configure_window_vals.values().data());
	}
	xcb_icccm_set_wm_size_hints(conn->info->conn,
				    id(),
				    conn->info->atoms_info.wm_normal_hints,
				    &hints);

}

xcb_size_hints_t main_windowObj::handlerObj::compute_size_hints(ONLY IN_THREAD)
{
	auto p=get_horizvert(IN_THREAD);

	auto minimum_width=p->horiz.minimum();
	auto minimum_height=p->vert.minimum();
#if 0
	auto new_preferred_width=p->horiz.preferred();
	auto new_preferred_height=p->vert.preferred();
#endif
	xcb_size_hints_t hints=xcb_size_hints_t();

	// set_screen_position() was used to set the suggested position.
	if (suggested_position(IN_THREAD))
	{
		auto width=suggested_position(IN_THREAD)->width;
		auto height=suggested_position(IN_THREAD)->height;

		// Bounds check the width and the height against our metrics.

		if (width < minimum_width)
			width=minimum_width;

		if (height < minimum_height)
			height=minimum_height;

		if (width > p->horiz.maximum())
			width=p->horiz.maximum();

		if (height > p->vert.maximum())
			height=p->horiz.maximum();

		// Now, make sure that x and y fits inside the workarea.

		auto x=suggested_position(IN_THREAD)->x;
		auto y=suggested_position(IN_THREAD)->y;

		auto &workarea=frame_extents(IN_THREAD).workarea;

		coord_t min_x=coord_t::truncate(workarea.x+
						frame_extents(IN_THREAD).left);
		coord_t min_y=coord_t::truncate(workarea.y+
						frame_extents(IN_THREAD).top);
		coord_t max_x=coord_t::truncate(workarea.width-
						frame_extents(IN_THREAD).right);
		coord_t max_y=coord_t::truncate(workarea.height-
						frame_extents(IN_THREAD).bottom)
			;
		max_x=coord_t::truncate(max_x-width);
		max_y=coord_t::truncate(max_y-height);

		if (x > max_x)
			x=max_x;
		if (y > max_y)
			y=max_y;

		if (x < min_x)
			x=min_x;
		if (y < min_y)
			y=min_y;

		xcb_icccm_size_hints_set_position(&hints, 1,
						  coord_t::truncate(x),
						  coord_t::truncate(y));
		xcb_icccm_size_hints_set_size(&hints, 1,
					      dim_t::truncate(width),
					      dim_t::truncate(height));

		xcb_icccm_size_hints_set_win_gravity(&hints,
						     XCB_GRAVITY_STATIC);
#ifdef MAINWINDOW_HINTS_DEBUG1
	MAINWINDOW_HINTS_DEBUG1();
#endif
	}
	xcb_icccm_size_hints_set_min_size(&hints,
					  (dim_t::value_type)minimum_width,
					  (dim_t::value_type)minimum_height);

#if 0
	xcb_icccm_size_hints_set_base_size(&hints,
					   (dim_t::value_type)new_preferred_width,
					   (dim_t::value_type)new_preferred_height);
#endif
	xcb_icccm_size_hints_set_max_size(&hints,
					  (dim_t::value_type)p->horiz.maximum(),
					  (dim_t::value_type)p->vert.maximum());

#ifdef MAINWINDOW_HINTS_DEBUG2
	MAINWINDOW_HINTS_DEBUG2();
#endif

	return hints;
}

void main_windowObj::handlerObj::request_visibility(ONLY IN_THREAD,
						    bool flag)
{
	if (!flag || preferred_dimensions_set(IN_THREAD))
	{
		generic_windowObj::handlerObj::request_visibility(IN_THREAD,
								  flag);
		return;
	}

	// Don't do this immediately. Schedule a thread job to do this, after
	// everything shakes down.

	IN_THREAD->run_as
		([me=ref<handlerObj>(this)]
		 (ONLY IN_THREAD)
		 {
			 me->preferred_dimensions_set(IN_THREAD)=true;

			 auto w=me->preferred_width(IN_THREAD);
			 auto h=me->preferred_height(IN_THREAD);

			 // If we have a suggested window size, use that
			 // instead.

			 auto hints=me->compute_size_hints(IN_THREAD);

			 if (hints.flags & XCB_ICCCM_SIZE_HINT_US_POSITION)
			 {
				 w=dim_t::truncate(hints.width);
				 h=dim_t::truncate(hints.height);
			 }

			 // An X server will report a BadValue for a window
			 // of size (0,0). Our metrics can compute such
			 // a preferred size, as an edge case. So, we deal
			 // with it.

			 values_and_mask configure_window_vals
				 (XCB_CONFIG_WINDOW_WIDTH,
				  (w == 0 || h == 0 ? 1
				   : (dim_t::value_type)w),

				  XCB_CONFIG_WINDOW_HEIGHT,
				  (w == 0 || h == 0 ? 1
				   : (dim_t::value_type)h));

#ifdef REQUEST_VISIBILITY_LOG
			 REQUEST_VISIBILITY_LOG(me->preferred_width(IN_THREAD),
						me->preferred_height(IN_THREAD)
						);
#endif
			 xcb_configure_window(IN_THREAD->info->conn, me->id(),
					      configure_window_vals.mask(),
					      configure_window_vals.values()
					      .data());

			 // Also simulate a configure_notify(), so that the
			 // display elements get arranged accordingly right
			 // away. We don't know when we'll get back the
			 // ConfigureNotify message from the display server.

			 auto r=*mpobj<rectangle>::lock(me->current_position);

			 r.width=w;
			 r.height=h;

			 me->process_configure_notify(IN_THREAD, r);

			 // Ok, need to wait for the reconfiguration to shake
			 // itself out too, so schedule another job to finally
			 // nail this coffin shut.

			 IN_THREAD->get_batch_queue()->run_as
				 ([me]
				  (ONLY IN_THREAD)
				  {
					  me->request_visibility(IN_THREAD,
								 true);
				  });
		 });
}

void main_windowObj::handlerObj::frame_extents_updated(ONLY IN_THREAD)
{
	generic_windowObj::handlerObj::frame_extents_updated(IN_THREAD);
	containerObj::implObj::tell_layout_manager_it_needs_recalculation
		(IN_THREAD);
}

///////////////////////////////////////////////////////////////////////////
//
// NET_WM_ICON

void main_windowObj::handlerObj
::install_window_theme_icon(const std::vector<std::tuple<std::string,
			    dim_t, dim_t>> &icons)
{
	screenref->impl->thread->run_as
		([v=create_icons(*this, icons), me=ref(this)]
		 (ONLY IN_THREAD)
		 {
			 me->install_window_theme_icon(IN_THREAD, v);
		 });
}

void main_windowObj::handlerObj
::install_window_icon(const std::vector<std::tuple<std::string,
		      dim_t, dim_t>> &icons)
{
	screenref->impl->thread->run_as
		([v=create_icons(*this, icons), me=ref(this)]
		 (ONLY IN_THREAD)
		 {
			 me->install_window_icon(IN_THREAD, v);
		 });
}

void main_windowObj::handlerObj
::install_window_theme_icon(ONLY IN_THREAD, const std::vector<icon> &icons)
{
	icon_images(IN_THREAD)=install_window_icon(IN_THREAD, icons);
}

std::vector<icon> main_windowObj::handlerObj
::install_window_icon(ONLY IN_THREAD, const std::vector<icon> &icons)
{
	// Need to finish initializing the icons.

	auto cpy=icons;

	for (auto &i:cpy)
		i=i->initialize(IN_THREAD);

	update_net_wm_icon(IN_THREAD, cpy);

	wm_icon_set(IN_THREAD)=true;

	return cpy;
}

void main_windowObj::handlerObj
::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	update_net_wm_icon(IN_THREAD, icon_images(IN_THREAD));
}

void main_windowObj::handlerObj
::update_net_wm_icon(ONLY IN_THREAD,
		     const std::vector<icon> &icon_images)
{
	if (icon_images.empty())
		return;

	std::vector<uint32_t> raw_data;
	size_t total_size=0;

	for (const auto &i:icon_images)
		total_size += 2 + (dim_squared_t::value_type)
			(i->image->get_width()*i->image->get_height());

	raw_data.reserve(total_size);

	for (const auto &i:icon_images)
	{
		pixmap_extractor extract{i->image};

		raw_data.push_back((dim_t::value_type)extract.width);
		raw_data.push_back((dim_t::value_type)extract.height);

		for (dim_t y=0; y<extract.height; y++)
			for (dim_t x=0; x<extract.width; x++)
			{
				auto pixel=extract.get_rgb(x, y);

#define CHANNEL(x) ((uint32_t)((pixel.x) >> ( (sizeof(rgb_component_t)-1)*8)))

				raw_data.push_back((CHANNEL(a) << 24)
						   |
						   (CHANNEL(r) << 16)
						   |
						   (CHANNEL(g) << 8)
						   |
						   CHANNEL(b));
			}
	}

	screenref->get_connection()->impl->set_wm_icon(id(), raw_data);
}

LIBCXXW_NAMESPACE_END
