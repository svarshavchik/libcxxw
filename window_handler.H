/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef window_handler_h
#define window_handler_h

#include "xid_t.H"
#include "connection_threadfwd.H"
#include "x/w/values_and_mask.H"
#include "x/w/rectangle.H"
#include "x/w/screenfwd.H"
#include "selection/current_selectionfwd.H"
#include <x/logger.H>
#include <xcb/xcb.h>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! A window message handler.

//! Provides callbacks that the connection thread invokes, in response
//! to messages.
//!
//! This contains a reference to the connection handler, and the connection
//! handler's shared_data contains references to these handlers. The
//! reference from shared_data is installed in generic_windowObj::implObj's
//! constructor. This circular reference is removed from implObj's
//! destructor.

class LIBCXX_HIDDEN window_handlerObj : public xid_t<xcb_window_t>,
					virtual public obj {

	//! The timestamp of the most recent activated passive grab.

	//! When the connection thread has nothing to do, all grabs are going
	//! to get released.
	xcb_timestamp_t grabbed_timestamp_thread_only=XCB_CURRENT_TIME;

	//! Semi-permanent passive grab.

	//! A display element wishes to extend the passive grab.

	bool grab_locked_thread_only=false;

	//! Selections announced from this window

	std::unordered_map<xcb_atom_t,
			   current_selection> selections_thread_only;
 public:

	THREAD_DATA_ONLY(grabbed_timestamp);
	THREAD_DATA_ONLY(grab_locked);
	THREAD_DATA_ONLY(selections);

	//! My screen
	const screen screenref;

	LOG_CLASS_SCOPE;

	//! Constructor parameters.
	struct constructor_params {

		screen screenref;
		xcb_window_t parent;
		depth_t depth;
		rectangle initial_position;

		uint16_t window_class;
		xcb_visualid_t visual;
		values_and_mask events_and_mask;
	};

	//! The constructor calls xcb_create_window()
	window_handlerObj(IN_THREAD_ONLY,
			  const constructor_params &params);

	//! The destructor calls xcb_destroy_window()

	~window_handlerObj();

	//! Release this handler's grabs, if there are any.

	void release_grabs(IN_THREAD_ONLY);

	//! Set a window property.

	void change_property(IN_THREAD_ONLY,
			     uint8_t mode,
			     xcb_atom_t property,
			     xcb_atom_t type,
			     uint8_t format,
			     uint32_t data_len,
			     void *data);

	////////////////////////////////////////////////////////////////////
	//
	// Implement in top level windows:

	//! Protocol client message
	virtual void client_message_event(IN_THREAD_ONLY,
					  const xcb_client_message_event_t *);

	//! Exposure event

	//! The implementor is free to trash the rectangle set.

	virtual void exposure_event(IN_THREAD_ONLY,
				    const rectangle_set &);

	//! A new theme has been installed

	virtual void theme_updated_event(IN_THREAD_ONLY);

	//! Configure notification

	virtual void configure_notify(IN_THREAD_ONLY,
				      const rectangle &);

	//! Key press event
	virtual void key_press_event(IN_THREAD_ONLY,
				     const xcb_key_press_event_t *event,
				     uint16_t sequencehi);

	//! Key release event
	virtual void key_release_event(IN_THREAD_ONLY,
				       const xcb_key_release_event_t *event,
				       uint16_t sequencehi);
	//! Button press event
	virtual void button_press_event(IN_THREAD_ONLY,
					const xcb_button_press_event_t *event);

	//! Button release event
	virtual void button_release_event(IN_THREAD_ONLY,
					  const xcb_button_release_event_t *event);
	//! Pointer motion event.
	virtual void pointer_motion_event(IN_THREAD_ONLY,
					  const xcb_motion_notify_event_t *);
	//! Enter event.
	virtual void enter_notify_event(IN_THREAD_ONLY,
					const xcb_enter_notify_event_t *);
	//! Leave event.
	virtual void leave_notify_event(IN_THREAD_ONLY,
					const xcb_enter_notify_event_t *);

	//! Focus change on the window
	virtual void focus_change_event(IN_THREAD_ONLY, bool);

	//! Received a SelectionClear event
	void selection_clear_event(IN_THREAD_ONLY,
				   xcb_atom_t selection_atom,
				   xcb_timestamp_t timestamp);

	//! Received a SelectionRequest event
	void selection_request_event(IN_THREAD_ONLY,
				     const xcb_selection_request_event_t
				     &request,
				     xcb_selection_notify_event_t &reply);
 private:
	//! Handle request for a MULTIPLE target.
	void selection_request_multiple(IN_THREAD_ONLY,
					const xcb_selection_request_event_t
					&request,
					xcb_selection_notify_event_t &reply,
					const current_selection &selection);
 public:
	//! An element in this window wishes to announce a new selection.

	//! If there's an existing selection, its clear() method gets
	//! invoked, after the new selection is replaced.

	void selection_announce(IN_THREAD_ONLY,
				xcb_atom_t selection_atom,
				const current_selection &selection);

	//! The element in this window wishes to discard its selection

	//! The element must arrange for its current_selection to return
	//! \c false to stillvalid(). If the specified selection returns
	//! \c false, it is removed.
	void selection_discard(IN_THREAD_ONLY,
			       xcb_atom_t selection_atom);
};

LIBCXXW_NAMESPACE_END

#endif
