/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include <x/logger.H>
#include <x/exception.H>

#include "ewmh.H"
#include "main_window_handler.H"
#include "returned_pointer.H"
#include "connectionfwd.H"
#include <xcb/xcb_icccm.h>
#include <x/strtok.H>
#include <x/chrcasecmp.H>
#include <algorithm>
#include <unistd.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::ewmh);

LIBCXXW_NAMESPACE_START

ewmh::ewmh(xcb_connection_t *conn) : ewmh_available(false)
{
	returned_pointer<xcb_generic_error_t *>e;

	auto cookies=xcb_ewmh_init_atoms(conn, this);

	if (!cookies)
		return;

	auto res=xcb_ewmh_init_atoms_replies(this, cookies, e.addressof());

	if (!res)
	{
		if (e)
		{
			LOG_ERROR(connection_error(e));
		}
		xcb_ewmh_connection_wipe(this);
		return;
	}

	ewmh_available=true;
}

ewmh::~ewmh()
{
	if (ewmh_available)
		xcb_ewmh_connection_wipe(this);
}

bool ewmh::get_workarea(size_t screen_number, rectangle &ret)
{
	if (!ewmh_available)
		return false;

	// First, determine the screen's current desktop.

	uint32_t n=0;

	{
		returned_pointer<xcb_generic_error_t *> error;

		if (xcb_ewmh_get_current_desktop_reply
		    (this,
		     xcb_ewmh_get_current_desktop(this, screen_number),
		     &n, error.addressof()))
		{
			LOG_DEBUG("get_workarea: screen number is " << n);
		}
		else
		{
			if (error)
				throw EXCEPTION(connection_error(error));
		}
	}

	// Now, get the work areas.

	returned_pointer<xcb_generic_error_t *> error;

	xcb_ewmh_get_workarea_reply_t wa;

	if (xcb_ewmh_get_workarea_reply(this,
					xcb_ewmh_get_workarea(this,
							      screen_number),
					&wa, error.addressof()))
	{
		LOG_DEBUG("get_workarea: there are " << wa.workarea_len
			  << " workareas");

		if (n < wa.workarea_len)
		{
			ret.x=wa.workarea[n].x;
			ret.y=wa.workarea[n].y;
			ret.width=wa.workarea[n].width;
			ret.height=wa.workarea[n].height;
		}
		xcb_ewmh_get_workarea_reply_wipe(&wa);
	}
	else
	{
		if (error)
			throw EXCEPTION(connection_error(error));
	}

	return true;
}

void ewmh::request_frame_extents(size_t screen_number, xcb_window_t window_id)
{
	if (!ewmh_available)
		return;

	xcb_ewmh_request_frame_extents(this,
				       screen_number,
				       window_id);
}

bool ewmh::get_frame_extents(dim_t &left,
			     dim_t &right,
			     dim_t &top,
			     dim_t &bottom,
			     xcb_window_t window_id)
{
	if (!ewmh_available)
		return false;

	LOG_DEBUG("Requesting frame extents");

	returned_pointer<xcb_generic_error_t *> error;

	xcb_ewmh_get_extents_reply_t frame_extents;

	if (xcb_ewmh_get_frame_extents_reply
	    (this,
	     xcb_ewmh_get_frame_extents(this, window_id),
	     &frame_extents,
	     error.addressof()))
	{
		left=frame_extents.left;
		right=frame_extents.right;
		top=frame_extents.top;
		bottom=frame_extents.bottom;
		return true;
	}

	if (error)
		throw EXCEPTION(connection_error(error));
	return false;
}

std::unordered_set<xcb_atom_t> ewmh::get_supported(size_t screen_number)
{
	std::unordered_set<xcb_atom_t> atoms;

	if (ewmh_available)
	{
		returned_pointer<xcb_generic_error_t *> error;
		xcb_ewmh_get_atoms_reply_t reply{};
		if (xcb_ewmh_get_supported_reply
		    (this, xcb_ewmh_get_supported(this, screen_number),
		     &reply, error.addressof()))
		{
			atoms.insert(reply.atoms, reply.atoms+reply.atoms_len);
			xcb_ewmh_get_atoms_reply_wipe(&reply);
		}

		if (error)
			throw EXCEPTION(connection_error(error));
	}

	return atoms;
}

void ewmh::set_wm_icon(xcb_window_t wid,
		       const std::vector<uint32_t> &raw_data)
{
	if (!ewmh_available)
		return;

	xcb_ewmh_set_wm_icon(this, XCB_PROP_MODE_REPLACE, wid,
			     raw_data.size(),
			     const_cast<uint32_t *>(&raw_data[0]));
}

namespace {
#if 0
}
#endif

struct atom_list {
	const char *n;
	xcb_atom_t xcb_ewmh_connection_t::*atom;
};

static const atom_list window_type_atoms[]={
	{"desktop", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DESKTOP},
	{"dock", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DOCK},
	{"toolbar", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_TOOLBAR},
	{"menu", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_MENU},
	{"utility", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_UTILITY},
	{"splash", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_SPLASH},
	{"dialog", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DIALOG},
	{"dropdown_menu",
	 &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DROPDOWN_MENU},
	{"popup_menu", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_POPUP_MENU},
	{"tooltip", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_TOOLTIP},
	{"notification",
	 &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_NOTIFICATION},
	{"combo", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_COMBO},
	{"dnd", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DND},
	{"normal", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_NORMAL},
};

static const atom_list window_state_atoms[]=
	{
	 {"modal", &xcb_ewmh_connection_t::_NET_WM_STATE_MODAL},
	 {"sticky", &xcb_ewmh_connection_t::_NET_WM_STATE_STICKY},
	 {"maximized_vert",
	  &xcb_ewmh_connection_t::_NET_WM_STATE_MAXIMIZED_VERT},
	 {"maximized_horz",
	  &xcb_ewmh_connection_t::_NET_WM_STATE_MAXIMIZED_HORZ},
	 {"shaded", &xcb_ewmh_connection_t::_NET_WM_STATE_SHADED},
	 {"skip_taskbar", &xcb_ewmh_connection_t::_NET_WM_STATE_SKIP_TASKBAR},
	 {"skip_pager", &xcb_ewmh_connection_t::_NET_WM_STATE_SKIP_PAGER},
	 {"hidden", &xcb_ewmh_connection_t::_NET_WM_STATE_HIDDEN},
	 {"fullscreen", &xcb_ewmh_connection_t::_NET_WM_STATE_FULLSCREEN},
	 {"above", &xcb_ewmh_connection_t::_NET_WM_STATE_ABOVE},
	 {"below", &xcb_ewmh_connection_t::_NET_WM_STATE_BELOW},
	 {"demands_attention",
	  &xcb_ewmh_connection_t::_NET_WM_STATE_DEMANDS_ATTENTION},
	};

static void do_parse_atoms(ewmh *me,
			   const atom_list *list,
			   size_t n_atoms,
			   const std::string_view &type,
			   std::vector<xcb_atom_t> &atoms)
{
	std::list<std::string> words;

	strtok_str(type, ", \t\r\n", words);

	atoms.reserve(words.size());
	for (auto &w:words)
	{
		std::transform(w.begin(), w.end(), w.begin(),
			       chrcasecmp::tolower);

		for (const auto *b=list, *e=list+n_atoms; b != e; ++b)
		{
			if (w == b->n)
			{
				atoms.push_back(me->*(b->atom));
				break;
			}
		}
	}
}

template<size_t n>
static void parse_atoms(ewmh *me,
			const atom_list (&list)[n],
			const std::string_view &type,
			std::vector<xcb_atom_t> &atoms)
{
	do_parse_atoms(me, &list[0], n, type, atoms);
}

#if 0
{
#else
}
#endif

void ewmh::set_window_type(xcb_window_t wid,
			   const std::string_view &type)
{
	if (!ewmh_available)
		return;

	std::vector<xcb_atom_t> atoms;

	parse_atoms(this, window_type_atoms, type, atoms);

	if (atoms.empty())
		return;

	xcb_ewmh_set_wm_window_type(this, wid,
				    atoms.size(),
				    &atoms[0]);
}

void ewmh::set_window_name(xcb_window_t wid, const std::string &title)
{
	if (ewmh_available)
		xcb_ewmh_set_wm_name(this,
				     wid,
				     title.size(),
				     title.c_str());
	else
		xcb_icccm_set_wm_name(connection,
				      wid,
				      XCB_ATOM_STRING, 8,
				      title.size(),
				      title.c_str());
}

void ewmh::set_window_pid(xcb_window_t wid)
{
	if (!ewmh_available)
		return;

	xcb_ewmh_set_wm_pid(this, wid, getpid());
}

void ewmh::set_user_time(xcb_window_t wid, xcb_timestamp_t t)
{
	if (!ewmh_available)
		return;

	xcb_ewmh_set_wm_user_time(this, wid, t);
}

void ewmh::set_user_time_window(xcb_window_t wid, xcb_window_t time_wid)
{
	if (!ewmh_available)
		return;
	xcb_ewmh_set_wm_user_time_window(this, wid, time_wid);
}

bool ewmh::client_message(ONLY IN_THREAD,
			  main_windowObj::handlerObj &handler,
			  const xcb_client_message_event_t *event,
			  xcb_window_t root_window)
{
	if (!ewmh_available)
		return false;

	if (event->data.data32[0] == _NET_WM_PING)
	{
		auto message=*event;

		message.response_type=XCB_CLIENT_MESSAGE;
		message.window=root_window;
		xcb_send_event(connection,
			       0,
			       root_window,
			       (XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
				XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT),
			       reinterpret_cast<char *>(&message));

		return true;
	}

	if (event->data.data32[0] == _NET_WM_SYNC_REQUEST)
	{
		set_user_time(handler.id(), event->data.data32[1]);
		handler.reconfigure_sync_request_received(IN_THREAD)=
			static_cast<int64_t>
			((static_cast<uint64_t>
			  (static_cast<uint32_t>
			   (event->data.data32[3]))
			  << 32)
			 |
			 static_cast<uint32_t>(event->data.data32[2])
			 );
		return true;
	}
	return false;
}

void ewmh::set_state(xcb_window_t wid,
		     const std::string_view &state)
{
	if (!ewmh_available)
		return;

	std::vector<xcb_atom_t> atoms;

	parse_atoms(this, window_state_atoms, state, atoms);

	if (atoms.empty())
		xcb_ewmh_set_wm_state(this, wid, 0, 0);
	else
		xcb_ewmh_set_wm_state(this, wid, atoms.size(), &atoms[0]);
}

LIBCXXW_NAMESPACE_END
