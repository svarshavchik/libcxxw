/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_alignment_H
#define x_w_alignment_H

#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

//! Horizontal alignment

enum class halign { left, center, right, fill};

//! halign text strings

//! Mostly for internal use.
extern const char halign_names[4][8];

//! Vertical alignment

enum class valign { top, middle, bottom, fill};

//! valign text strings

//! Mostly for internal use.
extern const char valign_names[4][8];

LIBCXXW_NAMESPACE_END

#endif
