/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_filter_h
#define x_w_input_field_filter_h

#include <x/w/input_fieldfwd.H>
#include <x/w/richtext/richtextiterator.H>

LIBCXXW_NAMESPACE_START

//! Identifies the kind of a filtering event

//! \see input_field_filter_info

enum class input_filter_type { inserting, deleting, replacing, move_only };

//! Parameter to the input field filter callback.

//! An \ref input_field "input field"'s on_filter() callback receives this
//! parameter.
//!
//! A description of the change to the input field that results from a key
//! press (or a cut/paste operation).
//!
//! \see input_field

struct input_field_filter_info {

	//! Type of the input change.

	//! - deleting: deleting characters in the input field.
	//!
	//! - inserting: adding characters to the input field.
	//!
	//! - replacing: replacing the contents of the input field in any
	//! other way (such as by calling input_field's set() method).
	//!
	//! - move_only: the cursor is being moved, no changes to the input
	//! field.

	input_filter_type type;

	//! Input field's direction

	//! Either left_to_right or right_to_left
	bidi direction;

	//! Iterator to the starting position of the modified text
	richtextiterator starting_pos;

	//! Iterator to the ending position of the modified text
	richtextiterator ending_pos;

	//! Size of modified text.

	//! This will always be ending_pos->pos() - starting_pos->pos(), except
	//! when this is a multi-line input field containing bi-directional
	//! text.
	size_t n_deleted;

	//! New contents to be inserted at the starting position.

	//! n_delete may also be set to a non-0 value. For example, highlighting
	//! a portion of the input field and executing a paste operation sets
	//! n_delete to the number of characters that are highlighted, and
	//! new_contents to the pasted contents.

	std::u32string_view new_contents;

	//! Current number of characters in the input field.
	size_t size;

	//! Maximum size of the input field. Originally from input_field_config.
	size_t maximum_size;

	//! Return the original position of the cursor in the input field

	//! For type=move_only, starting_pos is the position where the cursor
	//! is getting moved to, and this returns the original cursor
	//! starting position; n_delete is 0, and new_contents is an empty
	//! string.
	//!
	//! For all other types this returns the same value as starting_pos.
	virtual size_t original_pos() const=0;

	//! Constructor
	input_field_filter_info(input_filter_type type,
				bidi direction,
				const richtextiterator &starting_pos,
				const richtextiterator &ending_pos,
				const std::u32string_view &new_contents,
				size_t size,
				size_t maximum_size);

	//! Apply this change.
	void update() const LIBCXX_PUBLIC;

	//! Apply some other kind of a change.
	virtual void update(const richtextiterator &starting_pos,
			    const richtextiterator &ending_pos,
			    const std::u32string_view &new_contents) const
		LIBCXX_PUBLIC=0;

	//! Move the cursor to the given position.
	virtual void move(size_t pos) const LIBCXX_PUBLIC=0;

	//! Move the cursor to select the entire contents of the input field.

	virtual void select_all() const LIBCXX_PUBLIC=0;
};

LIBCXXW_NAMESPACE_END

#endif
