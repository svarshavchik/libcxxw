/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef window_handler_h
#define window_handler_h

#include "xid_t.H"
#include "connection_threadfwd.H"
#include "values_and_mask.H"
#include "x/w/rectangle.H"
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

 public:
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
				    std::set<rectangle> &);

	//! Configure notification

	virtual void configure_notify(IN_THREAD_ONLY,
				      const rectangle &);
};

LIBCXXW_NAMESPACE_END

#endif
