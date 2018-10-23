/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_state_H
#define x_w_element_state_H

#include <x/w/rectangle.H>

LIBCXXW_NAMESPACE_START

//! Information passed to a \ref element "element's" state update callback.

//! The callback installed by on_state_update() gets invoked with the following
//! information provided.

struct element_state {

	//! When this is passed to a state update handler, what kind of an update this is.

	enum state_update_t {
		current_state,
		before_showing,
		after_showing,
		before_hiding,
		after_hiding
	};

	//! State update type

	//! - current_state: the current state of the display element.
	//! The initial invocation of a newly-installed callback, or if it's
	//! been resized.
	//!
	//! - before_showing: the display element is about to be shown.
	//!
	//! - after_showing: the display element is shown.
	//!
	//! - before_hiding: the display element is about to be hidden.
	//!
	//! - after_hiding: the display element is hidden.

	state_update_t state_update;

	//! Whether the display element is currently shown.

	bool shown;

	//! Its current dimensions.

	rectangle current_position;
};

LIBCXXW_NAMESPACE_END

#endif
