/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_motion_event_h
#define x_w_motion_event_h

#include <x/w/namespace.H>
#include <x/w/input_mask.H>
#include <x/w/types.H>
#include <x/w/motion_eventfwd.H>

LIBCXXW_NAMESPACE_START

enum class motion_event_type {
	real_motion,
	button_event,
	enter_event,
};

//! A motion event.

class motion_event {


public:
	//! Current input mask
	const input_mask &mask;

	//! Type of the event
	motion_event_type type;

	//! Coordinate
	coord_t x;

	//! Coordinate
	coord_t y;

	//! Constructor

	motion_event(const input_mask &mask,
		     motion_event_type type,
		     coord_t x,
		     coord_t y) : mask(mask), type(type), x(x), y(y) {}

};

LIBCXXW_NAMESPACE_END

#endif
