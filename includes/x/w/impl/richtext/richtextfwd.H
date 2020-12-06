/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextfwd_H
#define x_w_impl_richtext_richtextfwd_H

#include <x/ptrfwd.H>
#include <x/w/alignment.H>
#include <x/w/types.H>
#include <x/w/input_fieldfwd.H>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

class richtextObj;

//! A list of text paragraphs.

//! This object represents one or more paragraphs of "rich text", text with
//! colors and fonts.
//!
//! The implementation object is protected by a mutex, because it is not
//! thread safe. finish_initialization() must be invoked every time the mutex
//! protecting this object gets locked, except in order to obtain the
//! current contents of the rich text. This is required in order to finish
//! construction of this object.

typedef ref<richtextObj> richtext;

//! A nullable pointer reference to a \ref richtext "rich text string".

typedef ptr<richtextObj> richtextptr;

//! Options for a new richtext object.

struct richtext_options {
	//! Text alignment
	halign alignment=halign::left;

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
};

LIBCXXW_NAMESPACE_END

#endif
