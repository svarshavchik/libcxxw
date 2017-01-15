/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef rectangle_h
#define rectangle_h

#include "x/w/rectangle.H"
#include <set>
#include <vector>

LIBCXXW_NAMESPACE_START

//! Merge adjacent rectangles.

//! Finds rectangles that share an edge, and replaces them with a single
//! rectangle.
//!
//! When there are more than one way to merge a set of rectangles, it is
//! unspecified how the rectangles get merged.

void merge(std::set<rectangle> &rectangles) LIBCXX_HIDDEN;

struct rectangle_slicer {

	std::vector<rectangle> slicee_v;
	std::vector<rectangle> slicer_v;

	rectangle_slicer(const std::set<rectangle> &slicee,
			 const std::set<rectangle> &slicer);
	~rectangle_slicer();

	void slice_slicee();
	void slice_slicer();
};

//! Slice rectangles.

//! If any slicer rectangle's edge goes through a slicee, split the slicee
//! at the edge.

void slice(std::set<rectangle> &slicee,
	   const std::set<rectangle> &slicer) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
