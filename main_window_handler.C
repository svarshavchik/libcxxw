/*
** Copyright 2017-2019 Double Precision, Inc.
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
#include "x/w/generic_window_appearance.H"
#include "x/w/main_window_appearance.H"
#include "busy.H"
#include "catch_exceptions.H"
#include "inherited_visibility_info.H"
#include "size_hints.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

main_window_handler_constructor_params
::main_window_handler_constructor_params(const screen &parent_screen,
					 const char *window_type,
					 const char *window_state,
					 const color_arg &background_color,
					 const const_main_window_appearance &
					 appearance,
					 bool override_redirect)
	: generic_window_handler_constructor_params
	{
	 parent_screen,
	 window_type,
	 window_state,
	 background_color,
	 appearance->toplevel_appearance,
	 shared_handler_data::create(),
	 0,
	 override_redirect
	}, appearance{appearance}
{
}


main_windowObj::handlerObj::handlerObj(const constructor_params &params,
				       const std::optional<rectangle>
				       &suggested_position,
				       const std::string &window_id)
	: superclass_t{{}, params},
	  on_delete_callback_thread_only([](THREAD_CALLBACK,
					    const auto &ignore) {}),
	  net_wm_sync_request_counter{screenref->impl->thread},
	  suggested_position_thread_only{suggested_position},
	  appearance{params.appearance},
	  window_id{window_id}
{
}

void main_windowObj::handlerObj::installed(ONLY IN_THREAD)
{
	superclass_t::installed(IN_THREAD);

	// Set WM_PROTOCOLS to WM_DELETE_WINDOW -- we handle the window
	// close request ourselves.

	xcb_atom_t protocols[3];

	protocols[0]=conn()->atoms_info.wm_delete_window;

	int n=1;

	{
		mpobj<ewmh>::lock lock{screenref->get_connection()
				       ->impl->ewmh_info};

		if (lock->ewmh_available)
		{
			protocols[1]=lock->_NET_WM_PING;
			protocols[2]=lock->_NET_WM_SYNC_REQUEST;
			n += 2;
		}

		xcb_atom_t sync_request_counter{
			net_wm_sync_request_counter.id()};

		change_property(IN_THREAD,
				XCB_PROP_MODE_REPLACE,
				lock->_NET_WM_SYNC_REQUEST_COUNTER,
				XCB_ATOM_CARDINAL,
				32,
				1,
				&sync_request_counter);
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

		if (lock->client_message(IN_THREAD,
					 *this,
					 event,
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

	if (ok_to_publish_hints(IN_THREAD))
		publish_size_hints(IN_THREAD);
}

void main_windowObj::handlerObj::install_size_hints(const size_hints &hints)
{
	current_size_hints_t::lock lock{current_size_hints};

	if (lock->getptr())
		throw EXCEPTION(_("Cannot install more than one element"
				  " with size increment hints"));

	*lock=hints;
}

void main_windowObj::handlerObj::size_hints_updated(ONLY IN_THREAD)
{
	if (ok_to_publish_hints(IN_THREAD))
		publish_size_hints(IN_THREAD);
}

void main_windowObj::handlerObj
::set_default_wm_hints(ONLY IN_THREAD, xcb_icccm_wm_hints_t &hints)
{
	hints.flags=XCB_ICCCM_WM_HINT_INPUT;
	hints.input=1;
}

static std::vector<icon> create_icons(drawableObj::implObj &me,
				      const std::string &icon_name,
				      const std::vector<dim_t> &icon_sizes)
{
	std::vector<icon> v;

	v.reserve(icon_sizes.size());

	for (auto s:icon_sizes)
		v.push_back(me.create_icon_pixels(icon_name,
						  render_repeat::none,
						  s, s,
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
				install_window_icons
					(IN_THREAD,
					 create_icons(*this,
						      appearance->icon,
						      appearance->icon_sizes));
			} CATCH_EXCEPTIONS;
#ifdef MAINWINDOW_HINTS_DEBUG3
		MAINWINDOW_HINTS_DEBUG3();
#endif
	}
	else
	{
		// If we ever reopen, we would like to reopen in the
		// same position.

		auto position=get_absolute_location(IN_THREAD);

		get_absolute_location_on_screen(IN_THREAD, position);

		suggested_position(IN_THREAD)=position;
	}
	generic_windowObj::handlerObj
		::set_inherited_visibility(IN_THREAD, visibility_info);
}

void main_windowObj::handlerObj::publish_size_hints(ONLY IN_THREAD)
{
	auto hints=compute_size_hints(IN_THREAD);
#ifdef MAINWINDOW_HINTS_DEBUG1
	MAINWINDOW_HINTS_DEBUG1();
#endif

	auto conn=screenref->get_connection()->impl;

	// Before making ourselves visible, if we have a suggested position
	// explicitly send it via ConfigureWindow(). Apparently the hints
	// are not enough for XFCE.
	//
	// This is called from update_initial_size().

	if (!ok_to_publish_hints(IN_THREAD) &&
	    (hints.flags & (XCB_ICCCM_SIZE_HINT_US_POSITION |
			    XCB_ICCCM_SIZE_HINT_P_POSITION)))
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
	}
	xcb_icccm_size_hints_set_min_size(&hints,
					  (dim_t::value_type)minimum_width,
					  (dim_t::value_type)minimum_height);

	{
		current_size_hints_t::lock lock{current_size_hints};

		auto sh=lock->getptr();

		if (sh)
		{
			auto minimum_width_v=(dim_t::value_type)minimum_width;
			auto minimum_height_v=(dim_t::value_type)minimum_height;

			auto minimum_width_increment=
				(dim_t::value_type)
				sh->width_increment(IN_THREAD);
			auto minimum_height_increment=
				(dim_t::value_type)
				sh->height_increment(IN_THREAD);

			// Size hints are specified, but set them only if
			// they're more than 1 pixel.

			if (minimum_width_increment > 1 ||
			    minimum_height_increment > 1)
			{
				auto base_width=
					dim_t::truncate
					(sh->base_width(IN_THREAD) *
					 sh->width_increment(IN_THREAD));

				auto base_height=
					dim_t::truncate
					(sh->base_height(IN_THREAD) *
					 sh->height_increment(IN_THREAD));

				minimum_width_v = minimum_width_v > base_width
					? minimum_width_v-base_width:0;

				minimum_height_v =
					minimum_height_v > base_height
					? minimum_height_v - base_height:0;

				xcb_icccm_size_hints_set_base_size
					(&hints,
					 minimum_width_v,
					 minimum_height_v);

				xcb_icccm_size_hints_set_resize_inc
					(&hints,
					 minimum_width_increment,
					 minimum_height_increment);
			}
		}
	}

	xcb_icccm_size_hints_set_max_size(&hints,
					  (dim_t::value_type)p->horiz.maximum(),
					  (dim_t::value_type)p->vert.maximum());

#ifdef MAINWINDOW_HINTS_DEBUG2
	MAINWINDOW_HINTS_DEBUG2();
#endif

	return hints;
}

void main_windowObj::handlerObj
::configure_notify_received(ONLY IN_THREAD, const rectangle &r)
{
	if (!preferred_dimensions_set(IN_THREAD))
	{
		rectangle fake_r={r.x, r.y, 0, 0};

		superclass_t::configure_notify_received(IN_THREAD, fake_r);
		return;
	}
	superclass_t::configure_notify_received(IN_THREAD, r);
}

void main_windowObj::handlerObj::update_resizing_timeout(ONLY IN_THREAD)
{
	if (!preferred_dimensions_set(IN_THREAD))
	{
		resizing(IN_THREAD)=false;
		return;
	}

	superclass_t::update_resizing_timeout(IN_THREAD);
}

void main_windowObj::handlerObj::request_visibility(ONLY IN_THREAD,
						    bool flag)
{
	if (flag == data(IN_THREAD).requested_visibility) // NOOP.
	{
		generic_windowObj::handlerObj::request_visibility(IN_THREAD,
								  flag);
		return;
	}

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

			 auto [w, h]=me->compute_initial_size(IN_THREAD);

			 me->update_initial_size(IN_THREAD, w, h, 0);
		 });
}

std::tuple<dim_t, dim_t>
main_windowObj::handlerObj::compute_initial_size(ONLY IN_THREAD)
{
	auto w=preferred_width(IN_THREAD);
	auto h=preferred_height(IN_THREAD);

	// If we have a suggested window size, use that
	// instead.

	auto hints=compute_size_hints(IN_THREAD);

	if (hints.flags & XCB_ICCCM_SIZE_HINT_US_POSITION)
	{
		w=dim_t::truncate(hints.width);
		h=dim_t::truncate(hints.height);
	}

	// An X server will report a BadValue for a window
	// of size (0,0). Our metrics can compute such
	// a preferred size, as an edge case. So, we deal
	// with it.

	if (w == 0)
		w=1;

	if (h == 0)
		h=1;

	return {w, h};
}

void main_windowObj::handlerObj::update_initial_size(ONLY IN_THREAD,
						     dim_t w,
						     dim_t h,
						     size_t counter)
{
	// Simulate a configure_notify(), so that the
	// display elements get arranged accordingly right
	// away. We will eventually send an xcb_configure_notify
	// to match this.

	auto r=*mpobj<rectangle>::lock(current_position);

	r.width=w;
	r.height=h;

	configure_notify_received(IN_THREAD, r);
	process_configure_notify(IN_THREAD);
	update_resizing_timeout(IN_THREAD);

	// Ok, need to wait for the reconfiguration to shake
	// itself out too, so schedule another job to finally
	// nail this coffin shut.

	IN_THREAD->get_batch_queue()->run_as
		([me=ref{this}, counter, w, h]
		 (ONLY IN_THREAD)
		 {
			 auto [new_w, new_h]=
				 me->compute_initial_size(IN_THREAD);

			 if (new_w != w || new_h != h)
			 {
				 if (counter < 5)
				 {
					 me->update_initial_size(IN_THREAD,
								 new_w, new_h,
								 counter+1);
					 return;
				 }
				 LOG_ERROR("Top level window metrics"
					   " keep changing.");
			 }

			 // Time to send a matching configure window.

			 values_and_mask configure_window_vals
				 (XCB_CONFIG_WINDOW_WIDTH,
				  (dim_t::value_type)w,
				  XCB_CONFIG_WINDOW_HEIGHT,
				  (dim_t::value_type)h);

#ifdef REQUEST_VISIBILITY_LOG
			 REQUEST_VISIBILITY_LOG(w, h);
#endif
			 xcb_configure_window(IN_THREAD->info->conn, me->id(),
					      configure_window_vals.mask(),
					      configure_window_vals.values()
					      .data());

			 me->publish_size_hints(IN_THREAD);

			 // publish_size_hints() is normally called if
			 // ok_to_push_hints. Intentionally set this AFTER
			 // calling it.
			 me->ok_to_publish_hints(IN_THREAD)=true;

			 me->request_visibility(IN_THREAD, true);
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
::install_window_icons(const std::vector<std::string> &icons)
{
	screenref->impl->thread->run_as
		([v=create_icon_vector(icons), me=ref{this}]
		 (ONLY IN_THREAD)
		 {
			 me->install_window_icons(IN_THREAD, v);
		 });
}

void main_windowObj::handlerObj
::install_window_icons(const std::string &icon)
{
	screenref->impl->thread->run_as
		([v=create_icons(*this, icon, appearance->icon_sizes),
		  me=ref{this}]
		 (ONLY IN_THREAD)
		 {
			 me->install_window_icons(IN_THREAD, v);
		 });
}

void main_windowObj::handlerObj
::install_window_icons(ONLY IN_THREAD, const std::vector<icon> &icons)
{
	icon_images(IN_THREAD)=icons;

	icon_images_vector::initialize(IN_THREAD);
	wm_icon_set(IN_THREAD)=true;
	update_net_wm_icon(IN_THREAD);
}

void main_windowObj::handlerObj
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	update_net_wm_icon(IN_THREAD);
}

void main_windowObj::handlerObj
::update_net_wm_icon(ONLY IN_THREAD)
{
	if (icon_images(IN_THREAD).empty())
		return;

	std::vector<uint32_t> raw_data;
	size_t total_size=0;

	for (const auto &i:icon_images(IN_THREAD))
		total_size += 2 + (dim_squared_t::value_type)
			(i->image->get_width()*i->image->get_height());

	raw_data.reserve(total_size);

	for (const auto &i:icon_images(IN_THREAD))
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

void main_windowObj::handlerObj::process_configure_notify(ONLY IN_THREAD)
{
	superclass_t::process_configure_notify(IN_THREAD);

	// If we processed a reconfigure sync request, log it as such.

	if (reconfigure_sync_request_received(IN_THREAD))
	{
		reconfigure_sync_request_processed(IN_THREAD)=
			reconfigure_sync_request_received(IN_THREAD);

		reconfigure_sync_request_received(IN_THREAD).reset();
	}
}

void main_windowObj::handlerObj::idle(ONLY IN_THREAD)
{
	if (reconfigure_sync_request_processed(IN_THREAD))
	{
		auto v=*reconfigure_sync_request_processed(IN_THREAD);

		net_wm_sync_request_counter.set(IN_THREAD, v);
		reconfigure_sync_request_processed(IN_THREAD).reset();
	}

	superclass_t::idle(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
