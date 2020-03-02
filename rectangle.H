/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef rectangle_h
#define rectangle_h

#include "x/w/rectangle.H"
#include <unordered_set>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN rectangle_slicer {

	rectarea first, second;

	rectangle_slicer(const rectarea &a, const rectarea &b);

private:
	rectangle_slicer(const rectarea &a, const rectarea &b,
			 const std::vector<coord_t> &h_slices,
			 const std::vector<coord_t> &v_slices);

};

typedef std::unordered_set<rectangle, rectangle_hash> rectangle_uset;

//! Merge adjacent rectangles.

//! Finds rectangles that share an edge, and replaces them with a single
//! rectangle.
//!
//! When there are more than one way to merge a set of rectangles, it is
//! unspecified how the rectangles get merged.

void merge(rectangle_uset &rectangles) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
