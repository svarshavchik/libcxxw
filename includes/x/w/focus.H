/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focus_H
#define x_w_focus_H

#include <x/w/callback_triggerfwd.H>
#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

//! Reported keyboard and pointer focus changes.

//! This indication gets passed to a widget's on_pointer_focus() and
//! a focusable element's on_keyboard_focus() callbacks, and indicates
//! how the widget focus has changed.

enum class focus_change {
	gained,       //!< Gained focus
	lost,         //!< Lost focus
	child_gained, //!< Child widget gained focus
	child_lost,   //!< Child widget lost focus
	child_moved,        //!< Focus moved to a different child widget.

	gained_from_child,       //! Child lost focus for me, a gained event will follow
	lost_to_child,           //! After a lost focus, the focus was received by a child widget.

	focus_movement_complete, //! Focus movement has finished (internal state only).
};

//! Convert a very detailed focus_change to a simple boolean value.

//! Returns false for lost and child_lost, true for all others.

bool in_focus(focus_change f);

//! Focus callback

typedef void focus_callback_t(THREAD_CALLBACK,
			      focus_change, const callback_trigger_t &);

LIBCXXW_NAMESPACE_END

#endif
