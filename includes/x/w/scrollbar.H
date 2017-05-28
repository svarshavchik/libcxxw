/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_h
#define x_w_scrollbar_h

#include <cstdint>

#include <x/w/types.H>
#include <x/w/scrollbarfwd.H>

LIBCXXW_NAMESPACE_START

//! When a scrollbar should be shown

//! - never: never show the scrollbar
//!
//! - always: always show the scrollbar
//!
//! - automatic: show the scrollbar only when there's something to scroll
//!
//! - automatic_reserved: like automatic, but reserve the space for the scrollbar

enum class scrollbar_visibility {
	never,
	always,
	automatic,
	automatic_reserved
};

LIBCXXW_NAMESPACE_END

#endif
