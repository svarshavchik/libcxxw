/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef button_h
#define button_h

#include "x/w/impl/child_elementobj.H"
#include "container_elementfwd.H"
#include "container_visible_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/buttonobj.H"
#include "x/w/factoryfwd.H"
#include "x/w/factoryobj.H"
#include "x/w/border_arg.H"
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Internal function

button do_create_button_with_explicit_borders
(factoryObj &f,
 const border_arg &left_border,
 const border_arg &right_border,
 const border_arg &top_border,
 const border_arg &bottom_border,
 const color_arg &normal_color,
 const color_arg &selected_color,
 const color_arg &active_color,
 const function<factoryObj::factory_creator_t> &creator,
 const shortcut &shortcut_key,
 const child_element_init_params &init_params) LIBCXX_HIDDEN;

//!  button implementation object.

class LIBCXX_HIDDEN buttonObj::implObj
	: public container_visible_elementObj<container_elementObj
					      <child_elementObj>> {

 public:
	typedef container_visible_elementObj<
		container_elementObj<child_elementObj>
	 > superclass_t;

	//! Constructor

	implObj(const container_impl &container,
		const child_element_init_params &init_params);


	//! Destructor
	~implObj();
};


LIBCXXW_NAMESPACE_END

#endif
