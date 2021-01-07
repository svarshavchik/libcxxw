/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_slider_original_sizes_h
#define pane_slider_original_sizes_h

#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! Sizes of panes when a slider starts moving.

//! We capture the initial size of both panes on each side of the
//! slider.

struct LIBCXX_HIDDEN pane_slider_original_sizes {
	dim_t first_pane=0;
	dim_t second_pane=0;
};

LIBCXXW_NAMESPACE_END

#endif
