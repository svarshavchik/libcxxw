/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_richtextiterator_H
#define x_w_richtextiterator_H

#include <x/ref.H>
#include <x/w/richtext/richtextiteratorfwd.H>
#include <x/w/richtext/richtextiteratorobj.H>
#include <x/w/text_param.H>

LIBCXXW_NAMESPACE_START

//! What at() returns.

struct richtextiteratorObj::at_info {
	rectangle position; //!< Where the character is drawn.
	char32_t character; //!< The character at the iterator
	text_hotspotptr link; //!< The hotspot at the iterator
};

LIBCXXW_NAMESPACE_END

#endif
