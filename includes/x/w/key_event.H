/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef key_event_H
#define key_event_H

#include <x/w/input_mask.H>
#include <x/w/key_eventfwd.H>

LIBCXXW_NAMESPACE_START

//! Parsed information about a key press or release event

class LIBCXX_HIDDEN key_event : public input_mask {

 public:

	//! Key press or release event
	bool keypress;

	//! Unicode character pressed
	char32_t unicode;

	//! Or a keysym
	uint32_t keysym;

	//! Partial constructor

	//! Only initializes input_mask.
	key_event(uint16_t state, const keysyms &k);

	//! Destructor
	~key_event();
};

LIBCXXW_NAMESPACE_END
#endif