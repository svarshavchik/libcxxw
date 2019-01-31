/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_pictformat_H
#define x_w_pictformat_H

#include <x/ref.H>
#include <x/w/pictformatfwd.H>
#include <x/w/pictformatobj.H>
#include <x/w/rgb.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

//! Indexed colors, for indexed pictformats

class pictformatObj::color_index {

public:
	//! Pixel value
	const uint32_t index;

	//! Pixel color
	const rgb color;

	//! Constructor
	color_index(uint32_t indexArg, rgb &&colorArg) LIBCXX_HIDDEN;

	//! Destructor
	~color_index(); LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
