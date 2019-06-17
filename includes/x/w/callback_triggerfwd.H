/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_callback_triggerfwd_h
#define x_w_callback_triggerfwd_h

#include <x/w/namespace.H>
#include <x/w/button_eventfwd.H>
#include <x/w/key_eventfwd.H>
#include <x/w/motion_eventfwd.H>

#include <functional>
#include <variant>

LIBCXXW_NAMESPACE_START

//! Placeholder for an \ref callback_trigger_initial "initial callback".

struct initial {};

//! Placeholder for an \ref callback_trigger_prev "back-tab" key.

struct prev_key {};

//! Placeholder for an \ref callback_trigger_next "tab" key.

struct next_key {};

//! \ref callback_trigger_t "callback_trigger_t" is not copyable.

struct callback_trigger_is_not_copyable {

	callback_trigger_is_not_copyable(const callback_trigger_is_not_copyable
					 &)=delete;
};

//! What triggered a callback activation.

//! This parameter specifies the reason for a callback's activation: a
//! key event, a button event,  or something else. The default (monostate)
//! value indicates a default reason that does not have any associated
//! metadata, for example a list item
//! getting deselected because it's getting removed from the list.
//!
//! \see callback_trigger_key_event
//!
//! \see callback_trigger_button_event
//!
//! \see callback_trigger_initial

typedef std::variant<std::monostate,
		     const key_event *,
		     const button_event *,
		     const motion_event *,
		     initial,
		     prev_key,
		     next_key,
		     callback_trigger_is_not_copyable
		     > callback_trigger_t;

//! \ref callback_trigger_t "callback_trigger_t" has a \ref key_event "const INSERT_LIBX_NAMESPACE::w::key_event *".

//! std::variant index where one might find a \ref key_event "key_event".

//! \note The key event is not copyable, and gets destroyed when the
//! callback returns.
constexpr size_t callback_trigger_key_event=1;

//! \ref callback_trigger_t "callback_trigger_t" has a \ref button_event "const INSERT_LIBX_NAMESPACE::w::button_event *".

//! std::variant index where one might find a \ref button_event "button_event".

//! \note The button event is not copyable, and gets destroyed when the
//! callback returns.

constexpr size_t callback_trigger_button_event=2;

//! std::variant index where one might find a \ref motion_event "motion_event".

//! \note The motion event is not copyable, and gets destroyed when the
//! callback returns.

constexpr size_t callback_trigger_motion_event=3;

//! \ref callback_trigger_t "callback_trigger_t" initial invocation.

//! std::variant index used in the following circumstances:
//!
//! - with \ref image_button "image button"s, indicating the callback's
//!   initial invocation.
//!
//! - internal use by \ref editable_comboboxlayoutmanager "editable combo-boxes"
//!   indicating that the combo-box selection was made by auto-completion.
//!
//! - initial invocation from on_pointer_focus() and on_keyboard_focus().
//!
//! - initial invocation from date_input_field's on_change().
//!
//! - a new on_status callback is installed for a list item, and this
//!   reports the current list item status.

constexpr size_t callback_trigger_initial=4;

//! \ref callback_trigger_t "callback_trigger_t" has a \ref prev_key "const INSERT_LIBX_NAMESPACE::w::prev_key &".

//! The callback is getting invoked because \c SHIFT-TAB or \c BACK button
//! was pressed

constexpr size_t callback_trigger_prev=5;

//! \ref callback_trigger_t "callback_trigger_t" has a \ref next_key "const INSERT_LIBX_NAMESPACE::w::next_key &".

//! The callback is getting invoked because \c TAB or \c NEXT button
//! was pressed.

constexpr size_t callback_trigger_next=6;

LIBCXXW_NAMESPACE_END

#endif
