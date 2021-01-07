/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_canvas_h
#define x_w_canvas_h

#include <x/w/canvasfwd.H>
#include <x/w/canvasobj.H>
#include <x/w/dim_axis_arg.H>
#include <x/w/rgb.H>
#include <x/ref.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Canvas options

struct canvas_config {

	//! Canvas background color, if specified.

	std::optional<color_arg> background_color;

	//! Initial width
	dim_axis_arg width;

	//! Initial height
	dim_axis_arg height;
};

LIBCXXW_NAMESPACE_END

#endif
