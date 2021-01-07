/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dim_axis_arg_h
#define x_w_dim_axis_arg_h

#include <x/w/dim_arg.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/metrics/axisfwd.H>

LIBCXXW_NAMESPACE_START

//! Horizontal/vertical axis dimensions.

//! A parameter that specifies the allowable horizontal or vertical
//! dimensions, typically for a widget.
//!
//! The dimensions are given as three values: minimum, preferred, and
//! maximum values.
//!
//! The minimum value is the element's smallest size. The preferred value
//! is typically the widget's initial, or default size (which is
//! subject to any bounds imposed by the element's container). The maximum
//! value specifies the upper bounds of the element's size.
//!
//! The minimum, preferred, and maximum values are specified as
//! \ref dim_arg "INSERT_LIBX_NAMESPACE::w::dim_arg"s. A numeric value
//! specifies the size in millimeters, or a string value uses the size set
//! by the display theme (this is used mostly internally, by the library).
//!
//! Setting the maximum value as NAN specifies a widget with no
//! maximum size.

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

	//! Compute actual metrics, using the given theme

	//! Mostly for internal use.

	metrics::axis compute(const const_defaulttheme &theme,
			      themedimaxis wh) const;
};

LIBCXXW_NAMESPACE_END

#endif
