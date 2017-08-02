/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldfwd_h
#define x_w_input_fieldfwd_h

#include <x/w/input_field_configfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class input_fieldObj;

//! A text input field.

//! \code
//! #include <INSERT_LIBX_NAMESPACE/w/input_field.H>
//!
//! INSERT_LIBX_NAMESPACE::w::input_field f=factory->create_input_field({""}, {20});
//! \endcode
//!
//! This is a text input field, with a blinking cursor.
//!
//! \code
//! f->set("Hello world");
//!
//! f->set_unicode(U"Hello world");
//! \endcode
//!
//! set() specifies the new contents of the input field, in UTF-8.
//! set_unicode() specifies the new contents as a unicode string.
//!
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::input_lock lock{f};
//! \endcode
//!
//! Constructing an \ref input_lock "INSERT_LIBX_NAMESPACE::w::input_lock"
//! blocks the internal library execution thread (and all other execution
//! threads from accessing the
//! \ref input_field "input_field", providing access to its contents).
//!
//! An \c input_lock is not required to set() new contents of the text
//! input field. The internal library execution thread actually updates
//! the contents, and set() only sends a message to the execution thread.
//! set() returns immediately after doing so.
//!
//! \note
//! For that reason,
//! getting the contents of the input field immediately after setting it may
//! still retrieve the original contents. The input field, like all other
//! display elements, gets updated by an internal execution thread, which
//! might be busy with other things, before focusing its attention on
//! updating the input field's contents.
//!
//! \see factory

typedef ref<input_fieldObj> input_field;

//! A nullable pointer reference to an input_field.

//! \see input_field

typedef ptr<input_fieldObj> input_fieldptr;

//! A reference to a constant input_field object.

//! \see input_field

typedef const_ref<input_fieldObj> const_input_field;

//! A nullable pointer reference to a constant input_field object.

//! \see input_field

typedef const_ptr<input_fieldObj> const_input_fieldptr;

class input_lock;

//! Identifies what kind of a change the input field change callback gets invoked for.

//! - deleted: characters were deleted.
//!
//! - inserted: characters were inserted.
//!
//! - set: set() was called.

enum class input_change_type {
	deleted,
	inserted,
	set};

//! Information passed to the input_field's on_changed() callback.

struct input_change_info_t {

	//! What kind of a change

	input_change_type type;

	//! How many unicode characters were inserted

	size_t inserted;

	//! How many unicode characters were deleted

	//! A non-0 value may be passed for \c inserted changes, indicating
	//! that the inserted text replaced a selection portion of the
	//! original text.
	//!
	//! A non-0 value gets passed for \c set changes, indicating the
	//! size of the replaced text.

	size_t deleted;
};

LIBCXXW_NAMESPACE_END

#endif
