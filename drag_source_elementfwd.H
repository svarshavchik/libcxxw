/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drag_source_elementfwd_H
#define drag_source_elementfwd_H

#include "drag_source.H"
#include "selection/current_selectionfwd.H"
#include "x/w/elementobj.H"
#include "x/w/motion_eventfwd.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include <xcb/xproto.h>
#include <x/obj.H>
#include <optional>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN drag_source_element_baseObj;
class LIBCXX_HIDDEN grab_inprogress_info;

class grab_inprogress_info : public drag_source {

 public:
	//! The XdndSelection
	const current_selection dnd_selection;

	//! My editor element.

	//! The grab_inprogress_info is an effective member of its
	//! editor element.
	drag_source_element_baseObj &me;

	//! Constructor
	grab_inprogress_info(ONLY IN_THREAD,
			     drag_source_element_baseObj &me,
			     const current_selection &dnd_selection,
			     const std::vector<xcb_atom_t>
			     &source_formats,
			     coord_t start_x,
			     coord_t start_y);

	~grab_inprogress_info();

 private:

	//! Implement show_droppable_pointer

	void show_droppable_pointer(ONLY IN_THREAD) override;

	//! Implement show_notdroppable_pointer

	void show_notdroppable_pointer(ONLY IN_THREAD) override;
};

class LIBCXX_HIDDEN drag_source_element_baseObj;

class drag_source_element_baseObj : virtual public obj {

	//! Whether motion events are dragging the selected text.

	//! We decide when button 1 is pressed what happens, and if we're
	//! dragging this gets initialize.

	std::optional<grab_inprogress_info> grab_inprogress_thread_only;

 public:
	THREAD_DATA_ONLY(grab_inprogress);

	//! Constructor
	drag_source_element_baseObj();

	//! Destructor
	~drag_source_element_baseObj();

	//! Create a grab_inprogress.
	void start_dragging(ONLY IN_THREAD,
			    const current_selection &dnd_selection,
			    const std::vector<xcb_atom_t> &source_formats,
			    coord_t start_x,
			    coord_t start_y);

	//! Forward a motion_event to the grab_inprogress.
	void report_dragged_motion_event(ONLY IN_THREAD,
					 const motion_event &me);

	//! End the dragged content, dropping it if it's acceptable.
	void release_dragged_selection(ONLY IN_THREAD);

	//! Abort dragging, don't drop anything.
	void abort_dragging(ONLY IN_THREAD);

	//! Invoked by the template mixin to handle status updates.
	bool handle_drag_response(ONLY IN_THREAD,
				  const xcb_client_message_event_t *event);

 private:

	//! Abort or release dragging.
	void stop_dragging(ONLY IN_THREAD, bool because_of_a_drop);

 public:
	//! A droppable item pointer should now be shown.

	//! Implemented in the display element.
	virtual void show_droppable_pointer(ONLY IN_THREAD)=0;

	//! A non-droppable item pointer should now be shown.

	//! Implemented in the display element.
	virtual void show_notdroppable_pointer(ONLY IN_THREAD)=0;

	//! Return my display element. Implemented by the mixin template.

	virtual elementObj::implObj &get_dragging_element_impl()=0;
};

template<typename element_impl>
class LIBCXX_HIDDEN drag_source_elementObj;

//! A mixin for a display element with draggable content.

//! The template parameter must be a subclass of child_elementObj, and the
//! template inherits from this class.
//!
//! The display element creates a \ref current_selection "current_selection"
//! representing the content that's being dragged, and invokes
//! start_dragging(), together with the list of formats that the
//! \c current_selection object supports converting to, and the starting
//! coordinates, in the display element, where dragging starts. This is
//! expected to be the most recent pointer x/y coordinates reported to the
//! display element.
//!
//! The display element is responsible for invoking
//! report_dragging_motion_event() while dragging is in progress.
//! report_dragging_motion_event() should simply be called from
//! process_motion_event().
//!
//! The display element can check
//!
//! \code
//!    if (grab_inprogress(IN_THREAD))
//! \endcode
//!
//! To determine if a drag is currently in progress. This returns \c true
//! after start_dragging() and until the display element
//! release_dragged_selection() or abort_dragging().
//!
//! release_dragged_selection() drops it, if the drag ended on top of
//! a droppable display element. abort_dragging() terminates the drag without
//! drop anything, even if the pointer was on top of a droppable display
//! element.
//!
//!
template<typename element_impl>
class drag_source_elementObj
	: public element_impl,
	  public drag_source_element_baseObj {

 public:
	typedef element_impl superclass_t;

	using superclass_t::superclass_t;

	elementObj::implObj &get_dragging_element_impl() override;

	void pointer_focus(ONLY IN_THREAD,
			   const callback_trigger_t &trigger) override;

	bool drag_response(ONLY IN_THREAD,
			   const xcb_client_message_event_t *event) override;
};

LIBCXXW_NAMESPACE_END

#endif
