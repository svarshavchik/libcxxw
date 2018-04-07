/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef shared_handler_dataobj_h
#define shared_handler_dataobj_h

#include "shared_handler_datafwd.H"
#include "popup/popup_handler.H"
#include "x/w/connection_threadfwd.H"
#include "shortcut/installed_shortcutfwd.H"
#include "x/w/generic_windowobj.H"
#include "x/w/motion_event.H"

#include <x/weaklist.H>
#include <x/weakptr.H>
#include <x/weakmultimap.H>
#include <x/mpobj.H>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see shared_handler_data

class LIBCXX_HIDDEN shared_handler_dataObj : virtual public obj {

	class handler_mcguffinObj;

	//! Top level handler object.
	//! This is the top level main window object, that owns a strong
	//! reference to this object.

	weakptr<ptr<generic_windowObj::handlerObj>> toplevel_handler;

	//! Currently opened exclusive

	weakptr<ptr<handler_mcguffinObj>> opened_exclusive_popup;

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

	void hide_menu_popups_until(ONLY IN_THREAD,
				    opened_menu_popups_t::base::iterator);
public:
	//! Constructor
	shared_handler_dataObj() LIBCXX_HIDDEN;

	//! Destructor
	~shared_handler_dataObj() LIBCXX_HIDDEN;

	//! Set our top level handler object.

	void set_toplevel_handler(const ref<generic_windowObj::handlerObj> &);

	//! A new exclusive popup is being opened.

	//! Invokes close_exclusive_popup(). Returns a mcguffin for the
	//! now-visible exclusive popup.

	ref<obj> opening_exclusive_popup(ONLY IN_THREAD,
					const ref<popupObj::handlerObj> &);

	//! The opened exclusive popup is being closed.

	void closing_exclusive_popup(ONLY IN_THREAD,
				    const popupObj::handlerObj &popup);

	//! Close the currently-opened exclusive popup, if there is one.
	void close_exclusive_popup(ONLY IN_THREAD);

	//! A new menu popup is being opened.

	//! Close any existing menu popup with the same or higher nesting level.

	ref<obj> opening_menu_popup(ONLY IN_THREAD,
				    const ref<popupObj::handlerObj> &);

	//! A menu popup is being closed

	//! Close all existing menu popups with higher nesting level.

	void closing_menu_popup(ONLY IN_THREAD,
				const popupObj::handlerObj &);

	//! Close all menu popups.
	void close_all_menu_popups(ONLY IN_THREAD);

	//! Forward a key event to whichever popup should be getting them.

	bool handle_key_event(ONLY IN_THREAD,
			      const xcb_key_release_event_t *event,
			      bool keypress);

	//! The window handler takes care of button events, just an FYI.

	//! We'll invoke, ourselves, ungrab_pointer for everything else.
	//! This way when a button press opens a popup, when the button
	//! release gets processed we'll unhighlight the button in the original
	//! window.

	void reporting_button_event_to
		(ONLY IN_THREAD,
		 const ref<generic_windowObj::handlerObj> &from,
		 const ref<generic_windowObj::handlerObj> &to,
		 const button_event &);

	//! Find a popup to report a pointer event to.

	//! Given a motion event with x/y coordinates relative to the root
	//! window, find the first popup that includes these x/y coordinates.

	ptr<generic_windowObj::handlerObj>
		find_popup_for_xy(ONLY IN_THREAD, const motion_event &);

	//! Look up installed shortcuts

	mpobj<shortcut_lookup_t> installed_shortcuts;
};

LIBCXXW_NAMESPACE_END

#endif
