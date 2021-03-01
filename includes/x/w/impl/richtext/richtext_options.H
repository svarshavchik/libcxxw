/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextoptions_H
#define x_w_impl_richtext_richtextoptions_H

#include <x/ptrfwd.H>
#include <x/w/alignment.H>
#include <x/w/types.H>
#include <x/w/impl/richtext/richtext_optionsfwd.H>
#include <courier-unicode.h>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Options for a new richtext object.

struct richtext_options {

	//! Text alignment
	std::optional<halign> alignment;

	//! Width of the rich text

	//! Sets targeted word-wrap width. If 0, no wrapping.
	dim_t initial_width=0;

	//! Substitute character for unprintable characters.

	//! The default '\0' effectively hides unprintable characters.
	//! Spaces are specified for input fields, since '\n' appear at
	//! the end of each paragraph, and a space serves nicely as a blinking
	//! cursor there.
	char32_t unprintable_char='\0';

	//! Embedding direction for this richtext.

	//! If unset it gets derived from the initial or set string.
	std::optional<unicode_bidi_level_t> paragraph_embedding_level;

	//! Bi-directional formatting
	bidi_format directional_format=bidi_format::automatic;

	//! Initialize paragraph_embedding_level basic on bidi values.

	void set_bidi(bidi level);

	//! Whether this is the richtext object for an input field

	//! The richtext object will have a trailing space, and this activates
	//! additional functionality: if all but the last trailing space
	//! is getting replaced, this gets handled as set(), which updates
	//! the default embedding level and alignment, as appropriate.
	bool is_editor=false;
};

LIBCXXW_NAMESPACE_END

#endif
