/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedim_axis_elementfwd_H
#define x_w_impl_themedim_axis_elementfwd_H

#include <x/w/impl/themedim_elementfwd.H>
#include <x/w/metrics/axis.H>
#include <x/w/dim_axis_arg.H>

LIBCXXW_NAMESPACE_START

template<typename base>
class LIBCXX_PUBLIC themedim_axis_widthObj;
template<typename base>
class LIBCXX_PUBLIC themedim_axis_heightObj;

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

	metrics::axis get_width_axis(ONLY IN_THREAD) const;

	void update_width_axis(ONLY IN_THREAD,
			       const dim_axis_arg &arg,
			       const const_defaulttheme &current_theme);
};

//! Vertical metrics axis specification mixin.

//! The template parameter is the superclass. This declares a subclass
//! of the template superclass and the themedim_elementObj mixin that
//! attaches three dimension arguments: minimum, preferred, and maximum
//! vertical dimensions.
//!
//! get_height_axis() return a metrics::axis value for the current dimensions.

template<typename base>
class themedim_axis_heightObj
	: public themedim_elementObj<base,
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

	metrics::axis get_height_axis(ONLY IN_THREAD) const;

	void update_height_axis(ONLY IN_THREAD,
				const dim_axis_arg &arg,
				const const_defaulttheme &current_theme);
};

LIBCXXW_NAMESPACE_END

#endif
