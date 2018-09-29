/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef window_handler_h
#define window_handler_h

#include "xid_t.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/values_and_mask.H"
#include "x/w/rectangle.H"
#include "x/w/screenfwd.H"
#include "x/w/impl/clock.H"
#include "window_handlerfwd.H"
#include "selection/current_selectionfwd.H"
#include "xim/ximclientfwd.H"
#include <x/logger.H>
#include <xcb/xcb.h>
#include <unordered_map>
#include <optional>

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
//!
//! Every instance of this class must be owned by a
//! \ref windowObj "windowObj". \c window_handlerObj's constructor calls
//! xcb_create_window() and the destructor calls xcb_destroy_window().
//! The \ref windowObj "windowObj owner" is responsible for registering
//! this window with the connection thread, in its constructor, and
//! setting up the callbacks to deregister this window in the destructor.

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

	//! There's a conversion in progress, for us.
	std::optional<tick_clock_t::time_point
		      > selection_request_expiration_thread_only;

	//! Conversion is over, or timed out.

	void end_conversion_request(ONLY IN_THREAD);

	//! Once a conversion is in progress, whether the converted type is ok.

	std::optional<bool> do_this_conversion;

	//! INCR conversion in progress
	bool converting_incrementally=false;

	//! The type of the data being converted.
	xcb_atom_t conversion_type=XCB_NONE;

	//! The conversion target
	xcb_atom_t conversion_property=XCB_NONE;

	//! Exposed rectangles.

	//! connection_thread collects Exposure events here, then calls
	//! process_collected_exposures() after all exposure events got
	//! buffered.

	struct exposed_t {

		//! The collected rectangles

		rectarea rectangles;

		//! The next exposure event processing should be a full exposure
		bool full_exposure=false;

		//! Count on the most recent Exposure event was 0

		//! Updated by connection_thread, which uses it, as part of its
		//! loop, to determine when to call
		//! process_collected_exposures().
		bool complete=false;
	};

	//! Exposure event's collected rectangles

	exposed_t exposure_rectangles_thread_only;

	//! GraphicsExposure event's collected rectangles

	exposed_t graphics_exposure_rectangles_thread_only;

	//! A ConfigureNotify was received for this window

	//! configure_notify_received() gets invoked as soon as it's received.
	//! It does not get processed immediately. When resizing a window it's
	//! common to get a bunch of these. They are expensive to process.
	//! So, configure_notify_received() gets invoked immediately, and when
	//! the dust settles process_configure_notify() gets invoked to deal
	//! with it.
	//! This is for connection_thread's use. It records the particulars
	//! of the ConfigureNotify event here. configure_notify_received()
	//! and process_configure_notify()

	bool pending_configure_notify_event_thread_only=false;

 public:

	THREAD_DATA_ONLY(grabbed_timestamp);
	THREAD_DATA_ONLY(grab_locked);
	THREAD_DATA_ONLY(selections);
	THREAD_DATA_ONLY(selection_request_expiration);
	THREAD_DATA_ONLY(exposure_rectangles);
	THREAD_DATA_ONLY(graphics_exposure_rectangles);
	THREAD_DATA_ONLY(pending_configure_notify_event);

	//! My screen
	const screen screenref;

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
	window_handlerObj(ONLY IN_THREAD,
			  const constructor_params &params);

	//! The destructor calls xcb_destroy_window()

	~window_handlerObj();

	//! Invoked after the handler gets installed.

	//! The handler is now registered with the connection thread.

	virtual void installed(ONLY IN_THREAD);

	//! Unexpected disconnection from the server.

	//! The default implementation does nothing.

	virtual void disconnected(ONLY IN_THREAD);

	//! Logically retain the passive grab.

	//! This can only be called when there's a passive grab (i.e.
	//! button or key press event). This sets grab_locked(), then
	//! allowsevents.

	virtual bool keep_passive_grab(ONLY IN_THREAD);

	//! Unlock the current grab, and release it.

	virtual void ungrab(ONLY IN_THREAD);

	//! Whether the window actively grabbed the pointer on its own.

	//! The default implemention returns false.
	virtual bool is_pointer_actively_grabbed(ONLY IN_THREAD);

	//! Release this handler's grabs, if there are any.

	void release_grabs(ONLY IN_THREAD);

	//! Set a window property.

	void change_property(ONLY IN_THREAD,
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
	virtual void client_message_event(ONLY IN_THREAD,
					  const xcb_client_message_event_t *);

	//! Connection thread wants us to process the collected exposures.

	virtual void process_collected_exposures(ONLY IN_THREAD);

	//! Connection thread wants us to process the collected graphics exposures.

	virtual void process_collected_graphics_exposures(ONLY IN_THREAD);

	//! A new theme has been installed

	virtual void theme_updated_event(ONLY IN_THREAD);

	//! Configure notification

	//! Do not process the configure notification just yet. The
	//! connection thread will invoke process_configure_notify().
	virtual void configure_notify_received(ONLY IN_THREAD,
					       const rectangle &);

	//! Process most recent configure notification

	//! The connection thread may invoke configure_notify_received()
	//! multiple times before process_configure_notify(), for efficiency.
	//!
	//! process_configure_notify() is typically expensive, and while a
	//! window is being resized this allows optimization of
	//! ConfigureNotify message processing. The cheap stuff goes into
	//! configure_notify_received(), which gets called for every
	//! ConfigureNotify event, and this should take little time.
	//!
	//! The expensive work goes into process_configure_notify().
	//!
	//! connection_thread buffers Exposure and ConfigureNotify events.
	//! After the message queue from the X servers gets drained, if there
	//! was at least one ConfigureNotify event received and buffered,
	//! process_configure_notify() gets called where all expensive work,
	//! that results from a resized window, occurs.

	virtual void process_configure_notify(ONLY IN_THREAD);

	//! MapNotify event
	virtual void process_map_notify_event(ONLY IN_THREAD);

	//! UnmapNotify event
	virtual void process_unmap_notify_event(ONLY IN_THREAD);

	//! Key press event
	virtual void key_press_event(ONLY IN_THREAD,
				     const xcb_key_press_event_t *event,
				     uint16_t sequencehi);

	//! Key release event
	virtual void key_release_event(ONLY IN_THREAD,
				       const xcb_key_release_event_t *event,
				       uint16_t sequencehi);

	//! Process a key press or release event.

	//! Invoked by XIM server when a forwarded key event is received.
	virtual bool handle_key_event(ONLY IN_THREAD,
				      const xcb_key_release_event_t *event,
				      bool keypress);

	//! Button press event
	virtual void button_press_event(ONLY IN_THREAD,
					const xcb_button_press_event_t *event);

	//! Button release event
	virtual void button_release_event(ONLY IN_THREAD,
					  const xcb_button_release_event_t *event);
	//! Pointer motion event.
	virtual void pointer_motion_event(ONLY IN_THREAD,
					  const xcb_motion_notify_event_t *);
	//! Enter event.
	virtual void enter_notify_event(ONLY IN_THREAD,
					const xcb_enter_notify_event_t *);
	//! Leave event.
	virtual void leave_notify_event(ONLY IN_THREAD,
					const xcb_enter_notify_event_t *);

	//! Focus change on the window
	virtual void focus_change_event(ONLY IN_THREAD, bool);

	//! Received a SelectionClear event
	void selection_clear_event(ONLY IN_THREAD,
				   xcb_atom_t selection_atom,
				   xcb_timestamp_t timestamp);

	//! Received a SelectionRequest event
	void selection_request_event(ONLY IN_THREAD,
				     const xcb_selection_request_event_t
				     &request,
				     xcb_selection_notify_event_t &reply);
 private:
	//! Handle request for a MULTIPLE target.
	void selection_request_multiple(ONLY IN_THREAD,
					const xcb_selection_request_event_t
					&request,
					xcb_selection_notify_event_t &reply,
					const current_selection &selection);
 public:
	//! An element in this window wishes to announce a new selection.

	//! If there's an existing selection, its clear() method gets
	//! invoked, after the new selection is replaced.

	void selection_announce(ONLY IN_THREAD,
				xcb_atom_t selection_atom,
				const current_selection &selection);

	//! The element in this window wishes to discard its selection

	//! The element must arrange for its current_selection to return
	//! \c false to stillvalid(). If the specified selection returns
	//! \c false, it is removed.
	void selection_discard(ONLY IN_THREAD,
			       xcb_atom_t selection_atom);

	//! selection_notify event, the selection has been pasted.

	void selection_notify_event(ONLY IN_THREAD,
				    const xcb_selection_notify_event_t *msg);

	////////////////////////////////////////////////////////////////////
	//
	// Receive converted selections.

	//! Convert selection to the given type.

	//! A subclass invokes convert_selection(). This will have three
	//! possible results.
	//!
	//! - a false gets returned if another conversion in progress. Only
	//! one conversion can be in progress. Otherwise true gets returned
	//! to indicate that the conversion is in progress.
	//!
	//! - conversion_failed() gets invoked if the conversion fails for
	//!   some reason.
	//!
	//! - otherwise, at some point later, begin_converted_data(),
	//!   converted_data(), and end_converted_data() get invoked.

	bool convert_selection(ONLY IN_THREAD, xcb_atom_t selection,
			       xcb_atom_t property,
			       xcb_atom_t type, xcb_timestamp_t timestamp);

	//! Selection conversion failed.

	virtual void conversion_failed(ONLY IN_THREAD,
				       xcb_atom_t type);

	//! Called by the connection thread.

	//! Checks if the conversion timed out.

	void timeout_selection_request(ONLY IN_THREAD, int &poll_for);
	//! Potentially convert the requested selection.

	//! The subclass that calls convert_selection() must implement
	//! begin_converted_data() as follows:
	//!
	//! - if the subclass previously requested a selection to be
	//! converted to the given type, it must return true. The converted
	//! selection's data will be passed by repeatedly invoking
	//! converted_data() followed by end_converted_data().
	//!
	//! - the subclass must return false if the subclass did not request
	//! conversion to this type.
	//!
	//! begin_converted_data() gets invoked upon receipt of every
	//! PropertyNotify for a new property. If the subclass returns true
	//! the property gets automatically deleted, after it is processed.
	//!
	//! If the subclass returned true for the initial call to
	//! begin_converted_data(), the subclass must also return true for
	//! all subsequent calls.
	//!
	//! Generally, the return value from begin_converted_data() should
	//! depend only on "type". The subclass should always accept converted
	//! data of the same type.

	virtual bool begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
					  xcb_timestamp_t timestamp);

	//! Converted selection data.

	virtual void converted_data(ONLY IN_THREAD, xcb_atom_t selection,
				    xcb_atom_t actual_type,
				    xcb_atom_t format,
				    void *data,
				    size_t size);

	//! Notify the subclass of the end of converted selection.

	//! If the converted selection's data was empty, converted_data()
	//! never gets called, only end_converted_data().

	virtual void end_converted_data(ONLY IN_THREAD);

	//! Someone pasted something to us?

	//! This default implementation checks for CXXWPASTE property
	//! notifications, and makes the appropriate calls to
	//! begin_converted_data(), converted_data()/converting_incrementally()
	//! and end_converted_data().

	virtual void property_notify_event(ONLY IN_THREAD,
					   const xcb_property_notify_event_t
					   *msg);

	///////////////////////////////////////////////////////////////////

	//! A pointer to this window's X Input Method client.

	ximclientptr ximclient_ptr;

	//! Perform a XIM client function safely.

	template<typename lambda>
		void with_xim_client(lambda &&l)
	{
		auto copy=ximclient_ptr; // Makes a copy of the ptr.

		if (copy) // The client has been installed.
			l(copy);
	}

	//! Paste a string into the window

	//! Called by the XIM server.
	virtual void pasted_string(ONLY IN_THREAD,
				   const std::u32string_view &);
};

LIBCXXW_NAMESPACE_END

#endif
