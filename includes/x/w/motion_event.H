/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_motion_event_h
#define x_w_motion_event_h

#include <x/w/namespace.H>
#include <x/w/input_mask.H>
#include <x/w/types.H>
#include <x/w/motion_eventfwd.H>

LIBCXXW_NAMESPACE_START

//! Motion events are, sometimes, reported for other than real pointer motions.

enum class motion_event_type {
	real_motion,		//!< Honest to goodness pointer motion event
	button_event,		//!< X & Y coordinates for a button event.
	button_action_event,	//!< X & Y cooridnates for a button event that activates something.
	enter_event,		//!< Pointer entered this display event.
	keyboard_action_event,  //!< Simulated motion by keyboard activity
};

//! A motion event.

class motion_event {


public:
	//! Current input mask
	const input_mask &mask;

	//! What the motion event is being reported for.

	motion_event_type type;

	//! Coordinate
	coord_t x;

	//! Coordinate
	coord_t y;
};

LIBCXXW_NAMESPACE_END

#endif
