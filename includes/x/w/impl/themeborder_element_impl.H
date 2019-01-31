/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themeborder_element_impl_H
#define x_w_impl_themeborder_element_impl_H

#include <x/w/impl/current_border_implfwd.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/impl/connection_threadfwd.H>
#include <x/w/impl/border_impl.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/border_arg.H>
#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

//! Mixin for a current_border_impl used by a display element.

//! Maintains a current_border_impl, made available as a current_border,
//! that a display element uses.
//!
//! There is no value-added to the themeborder_element_implObj, at this time.
//! This is a placeholder in case some features that alter a border's
//! appearance, based on the element that it's attached to, ever get
//! developed.
//!
//! \see themeborder_element

class themeborder_element_implObj : virtual public obj {

	//! The current border.

	current_border_impl current_border_thread_only;

 public:

	THREAD_DATA_ONLY(current_border);

	//! Constructor
	themeborder_element_implObj(const border_arg &arg,
				    elementObj::implObj &e);

	//! Destructor
	~themeborder_element_implObj();

	//! Return the element the border is attached to.
	virtual elementObj::implObj &get_border_element_impl()=0;

	//! Set a new theme border
	void set_new_border(ONLY IN_THREAD, const border_arg &arg);
};

LIBCXXW_NAMESPACE_END

#endif
