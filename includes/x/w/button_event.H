/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_event_h
#define x_w_button_event_h

#include <x/w/namespace.H>
#include <x/w/input_mask.H>
#include <x/w/button_eventfwd.H>

LIBCXXW_NAMESPACE_START

//! A pointer button event.

//! The object passed to an \ref element "widget"'s on_button() callback.

class button_event : public input_mask {

public:

	//! Constructor
	button_event(uint16_t SETofKEYBUTMASK, const keysyms &k,
		     int button,
		     bool press,
		     unsigned click_count,
		     button_event_redirect_info &redirect_info) LIBCXX_HIDDEN;

	//! Destructor
	~button_event() LIBCXX_HIDDEN;

	int button; //!< Pointer button number
	bool press; //!< Press or release
	unsigned click_count; //!< Consecutive click counter

	//! Focus redirection tracker, for internal use.
	button_event_redirect_info &redirect_info;
};


LIBCXXW_NAMESPACE_END

#endif
