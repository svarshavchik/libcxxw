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
#include <x/logger.H>
#include <xcb/xcb.h>

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

 public:

	THREAD_DATA_ONLY(grabbed_timestamp);

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
				    rectangle_set &);

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

};

LIBCXXW_NAMESPACE_END

#endif
