/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_position_h
#define x_w_screen_position_h

#include <x/w/types.H>
#include <x/w/rectangle.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! A position of a \ref main_window "main window" on its screen.

//! This is used to save and restore main window locations on its screen.
//!
//! \note
//! The location includes the screen number, for multi-screen displays.
//! If so desired, the screen number can be reset(), resulting in the
//! window always getting opened on the default screen.

struct screen_position : public rectangle {

	std::optional<size_t> screen_number;
};

LIBCXXW_NAMESPACE_END

#endif
