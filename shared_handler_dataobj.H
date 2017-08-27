/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef shared_handler_dataobj_h
#define shared_handler_dataobj_h

#include "shared_handler_datafwd.H"
#include "popup/popup_handler.H"
#include "connection_threadfwd.H"
#include "x/w/generic_windowobj.H"
#include "x/w/motion_event.H"

#include <x/weaklist.H>
#include <x/weakptr.H>
#include <x/weakmultimap.H>
#include <xcb/xproto.h>

#include <optional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see shared_handler_data

class LIBCXX_HIDDEN shared_handler_dataObj : virtual public obj {

	class handler_mcguffinObj;

	//! Currently opened combobox popup.

	weakptr<ptr<handler_mcguffinObj>> opened_combobox_popup;

	//! A container for an opened hierarchy of popup menus.

	//! Keyed by each opened menu's nesting_level, so that the
	//! appropriate hierarchy can be closed. Note that the multimap
	//! is ordered by the highest nesting level (most recently opened
	//! menu popup) first.

	typedef weakmultimap<size_t, handler_mcguffinObj,
			     std::greater<size_t>> opened_menu_popups_t;

	//! Opened hierarchy of popup menus.

	const opened_menu_popups_t opened_menu_popups;

	//! Hide all popups starting with the given iterator

	void hide_menu_popups_until(IN_THREAD_ONLY,
				    opened_menu_popups_t::base::iterator);
public:
	//! Constructor
	shared_handler_dataObj() LIBCXX_HIDDEN;

	//! Destructor
	~shared_handler_dataObj() LIBCXX_HIDDEN;

	//! A new combo-box popup is being opened.

	//! Invokes close_combobox_popup(). Returns a mcguffin for the
	//! now-visible combo-box popup.

	ref<obj> opening_combobox_popup(IN_THREAD_ONLY,
					const ref<popupObj::handlerObj> &);

	//! The opened combo-box popup is being closed.

	void closing_combobox_popup(IN_THREAD_ONLY,
				    const popupObj::handlerObj &popup);

	//! Close the currently-opened combo-box popup, if there is one.
	void close_combobox_popup(IN_THREAD_ONLY);

	//! A new menu popup is being opened.

	//! Close any existing menu popup with the same or higher nesting level.

	ref<obj> opening_menu_popup(IN_THREAD_ONLY,
				    const ref<popupObj::handlerObj> &);

	//! A menu popup is being closed

	//! Close all existing menu popups with higher nesting level.

	void closing_menu_popup(IN_THREAD_ONLY,
				const popupObj::handlerObj &);

	//! Close all menu popups.
	void close_all_menu_popups(IN_THREAD_ONLY);

	//! Forward a key event to whichever popup should be getting them.

	bool handle_key_event(IN_THREAD_ONLY,
			      const xcb_key_release_event_t *event,
			      bool keypress);

	//! Find a popup to report a pointer event to.

	//! Given a motion event with x/y coordinates relative to the root
	//! window, find the first popup that includes these x/y coordinates.

	ptr<generic_windowObj::handlerObj>
		find_popup_for_xy(IN_THREAD_ONLY, const motion_event &);
};

LIBCXXW_NAMESPACE_END

#endif
