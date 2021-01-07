/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "drag_source_elementfwd.H"
#include "drag_source_element.H"
#include "x/w/motion_event.H"
#include "generic_window_handler.H"
#include "defaulttheme.H"
#include "screen.H"
#include "connection_thread.H"
#include "catch_exceptions.H"

LOG_CLASS_INIT(LIBCXXW_NAMESPACE::drag_source);

LIBCXXW_NAMESPACE_START

drag_source_element_baseObj::drag_source_element_baseObj()=default;

drag_source_element_baseObj::~drag_source_element_baseObj()=default;

void drag_source_element_baseObj
::start_dragging(ONLY IN_THREAD,
		 const current_selection &dnd_selection,
		 const std::vector<xcb_atom_t> &source_formats,
		 coord_t start_x,
		 coord_t start_y)
{
	grab_inprogress(IN_THREAD).emplace(IN_THREAD,
					   *this,
					   dnd_selection,
					   source_formats,
					   start_x, start_y,
					   drag_start_horiz(IN_THREAD),
					   drag_start_vert(IN_THREAD));

	get_dragging_element_impl()
		.get_window_handler()
		.selection_announce(IN_THREAD,
				    IN_THREAD->info->atoms_info.XdndSelection,
				    dnd_selection);
}

bool drag_source_element_baseObj
::handle_drag_response(ONLY IN_THREAD,
		       const xcb_client_message_event_t *event)
{
	if (!grab_inprogress(IN_THREAD))
		return false;

	if (event->type == IN_THREAD->info->atoms_info.XdndStatus)
	{
		grab_inprogress(IN_THREAD)->status_update
			(IN_THREAD, event->data.data32[0],
			 (event->data.data32[1] & 1) != 0,
			 {
			  (event->data.data32[2] >> 16) & 0xFFFF,
			  (event->data.data32[2] & 0xFFFF),
			  (event->data.data32[3] >> 16) & 0xFFFF,
			  (event->data.data32[3] & 0xFFFF)
			 });
		return true;
	}

	return false;
}

void drag_source_element_baseObj::release_dragged_selection(ONLY IN_THREAD)
{
	if (!grab_inprogress(IN_THREAD))
		return;

	if (grab_inprogress(IN_THREAD)->drop(IN_THREAD))
		stop_dragging(IN_THREAD, true);
}

void drag_source_element_baseObj
::report_dragged_motion_event(ONLY IN_THREAD,
			      const motion_event &me)
{
	if (me.type != motion_event_type::real_motion)
		return;

	grab_inprogress(IN_THREAD)->report_motion_event(IN_THREAD,
					     me.x,
					     me.y);
}

void drag_source_element_baseObj::abort_dragging(ONLY IN_THREAD)
{
	stop_dragging(IN_THREAD, false);
}

void drag_source_element_baseObj::stop_dragging(ONLY IN_THREAD,
						bool because_of_a_drop)
{
	if (!grab_inprogress(IN_THREAD))
		return;

	grab_inprogress(IN_THREAD)->dnd_selection->stillvalid(IN_THREAD)=false;

	// If we just dropped this dragged selection, the drop target already
	// knows about this.

	if (!because_of_a_drop)
	{
		grab_inprogress(IN_THREAD)->leave_window(IN_THREAD);
		get_dragging_element_impl()
			.get_window_handler().selection_discard
			(IN_THREAD,
			 IN_THREAD->info->atoms_info
			 .XdndSelection);
	}
	grab_inprogress(IN_THREAD).reset();
}

grab_inprogress_info
::grab_inprogress_info(ONLY IN_THREAD,
		       drag_source_element_baseObj &me,
		       const current_selection &dnd_selection,
		       const std::vector<xcb_atom_t> &source_formats,
		       coord_t start_x,
		       coord_t start_y,
		       dim_t drag_start_horiz,
		       dim_t drag_start_vert)
	: drag_source{IN_THREAD,
		me.get_dragging_element_impl(),
		source_formats,
		start_x,
		start_y,
		drag_start_horiz,
		drag_start_vert},
	  dnd_selection{dnd_selection},
	  me{me}
{
}

void grab_inprogress_info::show_droppable_pointer(ONLY IN_THREAD)
{
	try {
		me.show_droppable_pointer(IN_THREAD);
	} REPORT_EXCEPTIONS(&me.get_dragging_element_impl());
}

void grab_inprogress_info::show_notdroppable_pointer(ONLY IN_THREAD)
{
	try {
		me.show_notdroppable_pointer(IN_THREAD);
	} REPORT_EXCEPTIONS(&me.get_dragging_element_impl());
}

grab_inprogress_info::~grab_inprogress_info()=default;

drag_source::drag_source(ONLY IN_THREAD,
			 elementObj::implObj &dragging_element,
			 const std::vector<xcb_atom_t> &source_formats,
			 coord_t start_x,
			 coord_t start_y,
			 dim_t drag_start_horiz,
			 dim_t drag_start_vert)
	: drag_source{IN_THREAD,
		dragging_element.get_absolute_location_on_screen(IN_THREAD),
		dragging_element.get_window_handler(),
		source_formats,
		start_x,
		start_y,
		drag_start_horiz,
		drag_start_vert}
{
}

drag_source::drag_source(ONLY IN_THREAD,
			 const rectangle &dragging_element_absolute_pos,
			 generic_windowObj::handlerObj &window_handler,
			 const std::vector<xcb_atom_t> &source_formats,
			 coord_t start_x,
			 coord_t start_y,
			 dim_t drag_start_horiz,
			 dim_t drag_start_vert)
	: drag_source{IN_THREAD,
		window_handler.id(),
		window_handler.get_screen(),
		source_formats,
		dragging_element_absolute_pos.x,
		dragging_element_absolute_pos.y,
		start_x,
		start_y,
		drag_start_horiz,
		drag_start_vert}
{
}

drag_source::drag_source(ONLY IN_THREAD,
			 xcb_window_t source_window,
			 const screen &window_screen,
			 const std::vector<xcb_atom_t
			 > &source_formats,
			 coord_t dragging_element_abs_x,
			 coord_t dragging_element_abs_y,
			 coord_t x,
			 coord_t y,
			 dim_t drag_start_horiz,
			 dim_t drag_start_vert)
	: source_window{source_window},
	  source_screen{window_screen->impl->xcb_screen},
	  source_formats{source_formats},
	  dragging_element_abs_x{dragging_element_abs_x},
	  dragging_element_abs_y{dragging_element_abs_y},
	  start_x{x}, start_y{y},
	  drag_start_horiz{drag_start_horiz},
	  drag_start_vert{drag_start_vert}
{
}

void drag_source::report_motion_event(ONLY IN_THREAD,
				      coord_t x,
				      coord_t y)
{
	if (!dragged_sufficiently_for_showing_drag_pointer)
	{
		// Maybe now?

		coord_t hmin=coord_t::truncate(start_x-drag_start_horiz);
		coord_t hmax=coord_t::truncate(start_x+drag_start_horiz);
		coord_t vmin=coord_t::truncate(start_y-drag_start_vert);
		coord_t vmax=coord_t::truncate(start_y+drag_start_vert);

		if (x < hmin || x > hmax || y < vmin || y > vmax)
		{
			dragged_sufficiently_for_showing_drag_pointer=true;
		}
		else
		{
			// We're on top of ourselves. Avoid causing a flurry
			// of messages, all due to a jittery pointer.
			return;
		}
	}

	auto info=IN_THREAD->info;

	x=coord_t::truncate(x+dragging_element_abs_x);
	y=coord_t::truncate(y+dragging_element_abs_y);

	most_recent_absolute_x=x;
	most_recent_absolute_y=y;
	most_recent_timestamp=IN_THREAD->timestamp(IN_THREAD);

	// Search for an XdndAware window under the pointer.

	auto window=source_screen->root;
	xcb_window_t src_window=window;
	xcb_window_t candidate_proxy_window;

	std::optional<int> candidate_version;

	// Keep track of windows we searched, and didn't find XdndAware.

	std::unordered_set<xcb_window_t> windows_checked;

	while(1)
	{
		candidate_proxy_window=window;

		if (window == window_under_pointer)
		{
			// If the window_under_pointer has XdndAware set,
			// and we just found our way back to it, we can
			// bail out here.

			if (version)
			{
				send_position_update(IN_THREAD);
				return;
			}
		}

		// Optimization: if we already saw this window, we don't
		// need to recheck its properties.

		if (parent_windows_under_pointer.find(window) ==
		    parent_windows_under_pointer.end())
		{
			// If this window property exists, it must be of type
			// XA_WINDOW and must contain the ID of the proxy
			// window that should be checked for XdndAware and
			// that should receive all the client messages, etc.
			// In order for the proxy window to behave correctly,
			// the appropriate field of the client messages,
			// window or data.l[0], must contain the ID of the
			// window in which the mouse is located, not the
			// proxy window that is receiving the messages. The
			// only place where the proxy window should be used is
			// when checking XdndAware
			// and in the calls to XSendEvent().

			info->collect_property_with
				(window, info->atoms_info.XdndProxy,
				 XCB_ATOM_WINDOW, 0,
				 [&]
				 (xcb_atom_t type,
				  uint8_t format,
				  const void *data,
				  size_t data_size)
				 {
					 if (data_size <
					     sizeof(candidate_proxy_window))
						 return;

					 auto cp=reinterpret_cast<const char
								  *>(data);

					 std::copy(cp,
						   cp+sizeof
						   (candidate_proxy_window),
						   reinterpret_cast
						   <char *>
						   (&candidate_proxy_window));
				 });

			info->collect_property_with
				(candidate_proxy_window,
				 info->atoms_info.XdndAware,
				 XCB_ATOM_ATOM, 0,
				 [&]
				 (xcb_atom_t type,
				  uint8_t format,
				  const void *data,
				  size_t data_size)
				 {
					 xcb_atom_t a;

					 if (data_size < sizeof(a))
						 return;

					 auto cp=reinterpret_cast<const
								  char *>(data);

					 std::copy(cp, cp+sizeof(a),
						   reinterpret_cast
						   <char *>(&a));
					 candidate_version=a;
				 });
		}

		if (candidate_version)
			break;

		parent_windows_under_pointer.insert(window);

		returned_pointer<xcb_generic_error_t *> error;

		auto return_value=xcb_translate_coordinates_reply
			(info->conn,
			 xcb_translate_coordinates(info->conn,
						   src_window, window,
						   (coord_t::value_type)(x),
						   (coord_t::value_type)(y)),
			 error.addressof());

		if (error)
			return; // TODO

		if (!return_value->child)
			break;

		src_window=window;

		window=return_value->child;
		x=return_value->dst_x;
		y=return_value->dst_y;
	}

	if (window == window_under_pointer)
	{
		send_position_update(IN_THREAD);
		return;
	}

	leave_window(IN_THREAD);

	window_under_pointer=window;
	proxy_window=candidate_proxy_window;
	parent_windows_under_pointer=windows_checked;

	if (candidate_version)
	{
		LOG_DEBUG("Window " << window_under_pointer
			  << " (" << proxy_window
			  << ") supports Xdnd version "
			  << *candidate_version);
		if (*candidate_version > 5)
			*candidate_version=5;

		version=candidate_version;

		xcb_client_message_event_t enter_message{};

		enter_message.response_type=XCB_CLIENT_MESSAGE;
		enter_message.window=proxy_window;
		enter_message.format=32;
		enter_message.type=IN_THREAD->info->atoms_info.XdndEnter;
		enter_message.data.data32[0]=source_window;

		enter_message.data.data32[1]=(5 << 24);

		auto b=source_formats.begin(), e=source_formats.end();

		for (size_t i=2; i<5; ++i)
		{
			if (b != e)
				enter_message.data.data32[i]=*b++;
		}

		xcb_send_event(IN_THREAD->info->conn, 0, proxy_window,
			       0,
			       reinterpret_cast<char *>(&enter_message));

		status_received=true;
		will_accept_drop=false;
		target_do_not_update={};

		// Need to make sure that send_position_update() will always
		// do it.
		most_recent_sent_xy.reset();
		send_position_update(IN_THREAD);
	}
	else
	{
		LOG_DEBUG("Window " << window_under_pointer
			  << " does not support Xdnd");
	}
}

void drag_source::status_update(ONLY IN_THREAD,
				xcb_window_t w,
				bool flag,
				const rectangle &r)
{
	LOG_DEBUG("XdndUpdate from " << w);

	if (!version)
	{
		LOG_DEBUG("Ignoring XdndUpdate from unknown source");
		return;
	}

	if (w != window_under_pointer)
	{
		LOG_DEBUG("Expecting XdndUpdate from " << window_under_pointer
			  << ", ignoring");
		return;
	}

	if (flag)
		show_droppable_pointer(IN_THREAD);
	else
		show_notdroppable_pointer(IN_THREAD);

	will_accept_drop=flag;
	target_do_not_update=r;

	LOG_DEBUG("Target status: will_accept_drop="
		  << will_accept_drop
		  << ", target_do_not_update="
		  << target_do_not_update);
	status_received=true;
	send_position_update(IN_THREAD);
}

void drag_source::send_position_update(ONLY IN_THREAD)
{
	if (!version)
		return;

	if (!status_received)
		return;

	if (most_recent_absolute_x >= target_do_not_update.x &&
	    most_recent_absolute_y >= target_do_not_update.y &&
	    target_do_not_update.width >
	    dim_t::truncate(most_recent_absolute_x - target_do_not_update.x) &&
	    target_do_not_update.height >
	    dim_t::truncate(most_recent_absolute_y - target_do_not_update.y))
	{
		LOG_TRACE("Position update excluded");
		return;
	}

	if (most_recent_sent_xy)
	{
		auto &[x, y] = *most_recent_sent_xy;

		if (x == most_recent_absolute_x &&
		    y == most_recent_absolute_y)
			return;
	}

	most_recent_sent_xy.emplace(most_recent_absolute_x,
				    most_recent_absolute_y);

	xcb_client_message_event_t position_message{};
	position_message.response_type=XCB_CLIENT_MESSAGE;
	position_message.window=proxy_window;

	position_message.format=32;
	position_message.type=IN_THREAD->info->atoms_info.XdndPosition;
	position_message.data.data32[0]=source_window;
	position_message.data.data32[2]=
		(xcoord_t::truncate(most_recent_absolute_x) << 16)
		 |
		xcoord_t::truncate(most_recent_absolute_y);
	if (*version >= 1)
		position_message.data.data32[3]=most_recent_timestamp;
	if (*version >= 2)
		position_message.data.data32[4]=
			IN_THREAD->info->atoms_info.XdndActionCopy;

	xcb_send_event(IN_THREAD->info->conn, 0, proxy_window,
		       0,
		       reinterpret_cast<char *>(&position_message));

	LOG_DEBUG("Position update sent: ("
		  << most_recent_absolute_x
		  << ", "
		  << most_recent_absolute_y
		  << ")");
	status_received=false;
}

void drag_source::leave_window(ONLY IN_THREAD)
{
	if (!version)
		return;

	LOG_DEBUG("Leaving window " << window_under_pointer);

	version.reset();

	xcb_client_message_event_t leave_message{};

	leave_message.response_type=XCB_CLIENT_MESSAGE;
	leave_message.window=proxy_window;

	leave_message.format=32;
	leave_message.type=IN_THREAD->info->atoms_info.XdndLeave;
	leave_message.data.data32[0]=source_window;

	xcb_send_event(IN_THREAD->info->conn, 0, proxy_window,
		       0,
		       reinterpret_cast<char *>(&leave_message));
}

bool drag_source::drop(ONLY IN_THREAD)
{
	if (!version)
	{
		LOG_DEBUG("Window does not implement Xdnd");
		return false;
	}

	if (!will_accept_drop)
	{
		LOG_DEBUG("Drop refused");
		return false;
	}
	LOG_DEBUG("Notifying target about the drop.");

	xcb_client_message_event_t drop_message{};

	drop_message.response_type=XCB_CLIENT_MESSAGE;
	drop_message.window=proxy_window;

	drop_message.format=32;
	drop_message.type=IN_THREAD->info->atoms_info.XdndDrop;
	drop_message.data.data32[0]=source_window;

	if (*version >= 1)
		drop_message.data.data32[2]=most_recent_timestamp;

	xcb_send_event(IN_THREAD->info->conn, 0, proxy_window,
		       0,
		       reinterpret_cast<char *>(&drop_message));
	return true;
}

void generic_windowObj::handlerObj
::process_drag_response(ONLY IN_THREAD,
			const xcb_client_message_event_t *event)
{
	LOG_FUNC_SCOPE(drag_source::logger);

	auto e=current_pointer_event_destination(IN_THREAD);

	if (event->type == IN_THREAD->info->atoms_info.XdndFinished)
	{
		selection_discard
			(IN_THREAD,
			 IN_THREAD->info->atoms_info.XdndSelection);
		LOG_DEBUG("XdndFinished processed");
		return;
	}

	if (e && e->drag_response(IN_THREAD, event))
		return;

	LOG_ERROR("Unprocessed "
		  << IN_THREAD->info->get_atom_name(event->type)
		  << " message.");
}

bool elementObj::implObj::drag_response(ONLY IN_THREAD,
					const xcb_client_message_event_t *event)
{
	return false;
}

LIBCXXW_NAMESPACE_END
