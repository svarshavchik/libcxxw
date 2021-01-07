/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef button_h
#define button_h

#include "x/w/impl/child_elementobj.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/container_visible_elementfwd.H"
#include "x/w/impl/bordercontainer_elementfwd.H"
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
 const button_config &config,
 const function<factoryObj::factory_creator_t> &creator,
 const shortcut &shortcut_key,
 const child_element_init_params &init_params) LIBCXX_HIDDEN;

//!  button implementation object.

class LIBCXX_HIDDEN buttonObj::implObj
	: public container_visible_elementObj<bordercontainer_elementObj
					      <container_elementObj
					       <child_elementObj>>> {

 public:
	typedef container_visible_elementObj<
		bordercontainer_elementObj<
			container_elementObj<child_elementObj>>
	 > superclass_t;

	//! Constructor

	implObj(const button_config &config,
		const container_impl &container,
		const child_element_init_params &init_params);

	//! Destructor
	~implObj();
};


LIBCXXW_NAMESPACE_END

#endif
