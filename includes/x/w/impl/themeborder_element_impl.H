/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themeborder_element_impl_H
#define x_w_impl_themeborder_element_impl_H

#include <x/w/impl/current_border_implfwd.H>
#include <x/w/impl/defaultthemefwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/border_arg.H>
#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

//! Mixin for a current_border_impl used by a display element.

//! Maintains a current_border_impl, made available as a current_border,
//! that a display element uses.
//!
//! \see themeborder_element

class themeborder_element_implObj : virtual public obj {

 public:

	//! The current border.

	const current_border_impl current_border;

	//! Constructor
	themeborder_element_implObj(const border_arg &arg,
				    elementObj::implObj &e);

	//! Destructor
	~themeborder_element_implObj();

	//! Return the element the border is attached to.
	virtual elementObj::implObj &get_border_element_impl()=0;

	//! Implement initialize().
	void initialize(ONLY IN_THREAD);

	//! Implement theme_updated().
	void theme_updated(ONLY IN_THREAD,
			   const defaulttheme &new_theme);

};

LIBCXXW_NAMESPACE_END

#endif