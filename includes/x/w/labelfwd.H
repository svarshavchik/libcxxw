/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_labelfwd_h
#define x_w_labelfwd_h

#include <x/w/namespace.H>
#include <string>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC labelObj;

//! Labels properties

struct label_config;

//! A text label widget.

//! Multiply-inherits from \ref textlabel "textlabel" and
//! \ref element "widget" for a normal label widget.
//!
//! \code
//! factory->create_label({"Hello world!"});
//! \endcode
//!
//! \see factory

typedef ref<labelObj> label;

//! A nullable pointer reference to an label.

//! \see label

typedef ptr<labelObj> labelptr;

//! A reference to a constant label object.

//! \see label

typedef const_ref<labelObj> const_label;

//! A nullable pointer reference to a constant label object.

//! \see label

typedef const_ptr<labelObj> const_labelptr;

//! Bi-directional text direction

enum class bidi {

	//! Automatically determine the direction of bi-directional text

	//! Uses the Unicode bi-directional algorithm to determine the
	//! main direction (left to right or right to left) based on the first
	//! directional character in the text or the label.
	automatic,

	//! This text or label's main direction is left-to-right
	left_to_right,

	//! This text or label's main direction is right-to-left
	right_to_left,
};

//! For internal use.

extern const char bidi_names[3][16];

//! Force text direction.

//! If bidi is automatic, the same string gets returned.
//! Otherwise all isolate, override, and directional markers get removed
//! from the string, and replacement by a single override marker for the
//! given direction.
//!
//! This is used with \ref input_field_filter_info "input field filters" which
//! handle singly-directional text only.

std::u32string bidi_override(const std::u32string_view &,
			     bidi direction);

//! Force text direction

//! \overload
std::string bidi_override(const std::string_view &,
			  bidi direction);

LIBCXXW_NAMESPACE_END

#endif
