/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "main_window_handler.H"
#include "connection_thread.H"
#include "draw_info.H"
#include "layoutmanager.H"
#include "x/w/screen.H"
#include "x/w/connection.H"

LIBCXXW_NAMESPACE_START

main_windowObj::handlerObj::handlerObj(IN_THREAD_ONLY,
				       const screen &parent_screen)
	: generic_windowObj::handlerObj(IN_THREAD, parent_screen),
	on_delete_callback_thread_only([] {})
{
	// The top level window is not a child element in a container,
	// so it is, hereby, initialized!
	//
	// We cannot go through the proper channels, i.e. initialize_if_needed()
	// because it calls schedule_update_visibility() in order to
	// kick-start and held visibility changes. However we CANNOT
	// do that, yet. We can't even do this outside of the constructor,
	// since schedule_update_visibility pokes connection thread-only
	// containers, and we are NOT in the connection thread.

	data(IN_THREAD).initialized=true;

	// Set WM_PROTOCOLS to WM_DELETE_WINDOW -- we handle the window
	// close request ourselves.

	xcb_atom_t protocols[1];

	protocols[0]=conn()->atoms_info.wm_delete_window;

	change_property(IN_THREAD,
			XCB_PROP_MODE_REPLACE,
			conn()->atoms_info.wm_protocols,
			XCB_ATOM_ATOM,
			sizeof(xcb_atom_t)*8,
			1,
			protocols);
}

main_windowObj::handlerObj::~handlerObj()=default;

void main_windowObj::handlerObj
::client_message_event(IN_THREAD_ONLY,
		       const xcb_client_message_event_t *event)
{
	if (event->type == IN_THREAD->info->atoms_info.wm_protocols)
	{
		if (event->data.data32[0] ==
		    IN_THREAD->info->atoms_info.wm_delete_window)
		{
			on_delete_callback(IN_THREAD)();
			return;
		}
	}
}

void main_windowObj::handlerObj::horizvert_updated(IN_THREAD_ONLY)
{
	auto conn=screenref->get_connection()->impl;

	auto p=get_horizvert(IN_THREAD);

	auto minimum_width=p->horiz.minimum();
	auto minimum_height=p->vert.minimum();
	auto new_preferred_width=p->horiz.preferred();
	auto new_preferred_height=p->vert.preferred();

	preferred_width(IN_THREAD)=new_preferred_width;
	preferred_height(IN_THREAD)=new_preferred_height;

	xcb_size_hints_t hints=xcb_size_hints_t();

	xcb_icccm_size_hints_set_min_size(&hints,
					  (dim_t::value_type)minimum_width,
					  (dim_t::value_type)minimum_height);
	xcb_icccm_size_hints_set_base_size(&hints,
					   (dim_t::value_type)new_preferred_width,
					   (dim_t::value_type)new_preferred_height);
	xcb_icccm_size_hints_set_max_size(&hints,
					  (dim_t::value_type)p->horiz.maximum(),
					  (dim_t::value_type)p->vert.maximum());

	xcb_icccm_set_wm_size_hints(conn->info->conn,
				    id(),
				    conn->info->atoms_info.wm_normal_hints,
				    &hints);
}

void main_windowObj::handlerObj::request_visibility(IN_THREAD_ONLY,
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
		(RUN_AS,
		 [me=ref<handlerObj>(this)]
		 (IN_THREAD_ONLY)
		 {
			 me->preferred_dimensions_set(IN_THREAD)=true;

			 // An X server will report a BadValue for a window
			 // of size (0,0). Our metrics can compute such
			 // a preferred size, as an edge case. So, we deal
			 // with it.

			 auto w=me->preferred_width(IN_THREAD);
			 auto h=me->preferred_height(IN_THREAD);

			 if (w == 0 || h == 0)
				 w=h=1;

			 values_and_mask configure_window_vals
				 (XCB_CONFIG_WINDOW_WIDTH,
				  (dim_t::value_type)w,

				  XCB_CONFIG_WINDOW_HEIGHT,
				  (dim_t::value_type)h);

				 ;
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

			 r.width=me->preferred_width(IN_THREAD);
			 r.height=me->preferred_height(IN_THREAD);

			 me->configure_notify(IN_THREAD, r);

			 // Ok, need to wait for the reconfiguration to shake
			 // itself out too, so schedule another job to finally
			 // nail this coffin shut.

			 IN_THREAD->run_as
				 (RUN_AS,
				  [me]
				  (IN_THREAD_ONLY)
				  {
					  me->request_visibility(IN_THREAD,
								 true);
				  });
		 });
}

void main_windowObj::handlerObj::frame_extents_updated(IN_THREAD_ONLY)
{
	generic_windowObj::handlerObj::frame_extents_updated(IN_THREAD);
	containerObj::implObj::tell_layout_manager_it_needs_recalculation
		(IN_THREAD);
}


LIBCXXW_NAMESPACE_END
