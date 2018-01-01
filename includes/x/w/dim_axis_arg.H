/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dim_axis_arg_h
#define x_w_dim_axis_arg_h

#include <x/w/dim_arg.H>

LIBCXXW_NAMESPACE_START

//! Horizontal/vertical axis dimensions.

//! A parameter that specifies the allowable horizontal or vertical
//! dimensions, typically for a display element.
//!
//! The dimensions are given as three values: minimum preferred, and
//! maximum values.

struct dim_axis_arg {

	//! Minimum value
	dim_arg minimum;

	//! Preferred value
	dim_arg preferred;

	//! Maximum value

	//! A NAN value means unbound maximum value.

	dim_arg maximum;

	//! Default dimensions

	//! 0 minimum and preferred, unbound maximum value.
	dim_axis_arg();

	//! Fixed dimensions

	//! Sets the same value to minimum, preferred, and maximum.
	dim_axis_arg(const dim_arg &a);

	//! Minimum dimensions

	//! Sets minimum and preferred values, maximum is NAN.

	dim_axis_arg(const dim_arg &minimum,
		     const dim_arg &preferred);

	//! Explicit dimensions.
	dim_axis_arg(const dim_arg &minimum,
		     const dim_arg &preferred,
		     const dim_arg &maximum);

	//! Destructor

	~dim_axis_arg();
};

LIBCXXW_NAMESPACE_END

#endif
