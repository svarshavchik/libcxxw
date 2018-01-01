/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedim_axis_elementfwd_H
#define themedim_axis_elementfwd_H

#include "themedim_elementfwd.H"
#include "x/w/metrics/axis.H"
#include "x/w/dim_axis_arg.H"

LIBCXXW_NAMESPACE_START

struct themedim_width_min;
struct themedim_width_pref;
struct themedim_width_max;

struct themedim_height_min;
struct themedim_height_pref;
struct themedim_height_max;

//! Horizontal metrics axis specification mixin.

//! The template parameter is the superclass. This declares a subclass
//! of the template superclass and the themedim_elementObj mixin that
//! attaches three dimension arguments: minimum, preferred, and maximum
//! horizontal dimensions.
//!
//! get_width_axis() return a metrics::axis value for the current dimensions.

template<typename base>
class themedim_axis_widthObj : public themedim_elementObj<base,
							  themedim_width_min,
							  themedim_width_pref,
							  themedim_width_max> {

	typedef themedim_elementObj<base,
				    themedim_width_min,
				    themedim_width_pref,
				    themedim_width_max> superclass_t;


public:
	template<typename ...Args>
	themedim_axis_widthObj(const dim_axis_arg &dims, Args && ...args);

	~themedim_axis_widthObj();

	metrics::axis get_width_axis(IN_THREAD_ONLY) const;
};

//! Vertical metrics axis specification mixin.

//! The template parameter is the superclass. This declares a subclass
//! of the template superclass and the themedim_elementObj mixin that
//! attaches three dimension arguments: minimum, preferred, and maximum
//! vertical dimensions.
//!
//! get_height_axis() return a metrics::axis value for the current dimensions.

template<typename base>
class themedim_axis_heightObj : public themedim_elementObj<base,
							  themedim_height_min,
							  themedim_height_pref,
							  themedim_height_max> {

	typedef themedim_elementObj<base,
				    themedim_height_min,
				    themedim_height_pref,
				    themedim_height_max> superclass_t;


public:
	template<typename ...Args>
	themedim_axis_heightObj(const dim_axis_arg &dims,
				Args && ...args);

	~themedim_axis_heightObj();

	metrics::axis get_height_axis(IN_THREAD_ONLY) const;
};

LIBCXXW_NAMESPACE_END

#endif
