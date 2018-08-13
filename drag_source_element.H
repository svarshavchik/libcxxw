/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drag_source_element_H
#define drag_source_element_H

#include "drag_source_elementfwd.H"
#include "selection/current_selection.H"

LIBCXXW_NAMESPACE_START

//! Implement get_dragging_element_impl().

template<typename element_impl>
elementObj::implObj &drag_source_elementObj<element_impl>
::get_dragging_element_impl()
{
	return *this;
}

//! Process drag and drop status updates.
template<typename element_impl>
bool drag_source_elementObj<element_impl>
::drag_response(ONLY IN_THREAD,
		const xcb_client_message_event_t *event)
{
	return this->handle_drag_response(IN_THREAD, event);
}

//! Override pointer_focus()

//! The pointer is grabbed while dragging, so it shouldn't lose its focus;
//! but if it does we pull the cord here.

template<typename element_impl>
void drag_source_elementObj<element_impl>
::pointer_focus(ONLY IN_THREAD,
		const callback_trigger_t &trigger)
{
	superclass_t::pointer_focus(IN_THREAD, trigger);

	// We grab the pointer when we start dragging, but
	// button_1_dragging gets constructed immediately after button 1
	// down event. If we lose pointer focus beforehand, we'll bail out
	// completely.

	if (!this->current_pointer_focus(IN_THREAD))
		this->abort_dragging(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
