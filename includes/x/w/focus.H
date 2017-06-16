/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focus_H
#define x_w_focus_H

#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

//! Reported keyboard and pointer focus changes.

enum class focus_change {
	gained,       //!< Gained focus
	lost,         //!< Lost focus
	child_gained, //!< Child element gained focus
	child_lost,   //!< Child element lost focus
	child_moved_from,        //!< Focus moved to a different child element.
	child_moved_to,          //!< Focus moved to a different child element.

	gained_from_child,       //! Child lost focus for me, a gained event will follow
	lost_to_child,           //! After a lost focus, the focus was received by a child element.

	focus_movement_complete, //! Focus movement has finished (internal state only).
};

//! Convert a very detailed focus_change to a simple boolean value.

//! Returns true for gained, child_gained, child_moved_to, and
//! gained_from_child; and false for all others

bool in_focus(focus_change f);

//! Focus callback

typedef void focus_callback_t(focus_change);

LIBCXXW_NAMESPACE_END

#endif
