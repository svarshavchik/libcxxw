/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_linebreak_info_H
#define richtext_linebreak_info_H

#include "x/w/namespace.H"
#include "richtext/richtext_linebreak_infofwd.H"
#include "x/w/impl/richtext/richtextstring.H"
#include <courier-unicode.h>
#include <queue>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Run the unicode linebreak algorithm on one or more richtextstrings.

//! Additionally, detect changes in left-to-right rendering, and
//! make sure that linebreaking is allowed at the change of direction
//! point. Whenever the rendering direction changes, UNICODE_LB_NONE is
//! overridden to UNICODE_LB_ALLOWED.
//!
//! Writes the linebreaking values to a preallocated array of unicode_lbs.
//!
//! A pointer to an array of pointers of richtextstrings define the character
//! sequence whose linebreaks get computed, n_strings is the number of the
//! richtextstrings.
//!
//! 'ptr' points to the preallocated array of unicode_lbs, whose size is 'todo'
//! which may be smaller than the total number of characters. Together with
//! 'skip' this defines a subset of the linebreaking values to calculate.
//!
//! So, if 'skip' is 4, and 'todo' is 10, this saves the linebreaking values
//! for characters 4 through 13 into ptr (with character 4's value in ptr[0]).

void richtext_linebreak_info(size_t skip,
			     size_t todo,
			     unicode_lb *ptr,
			     richtextstring * const *strings,
			     size_t n_strings) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END;
#endif
