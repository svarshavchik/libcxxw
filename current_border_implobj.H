/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef current_border_implobj_h
#define current_border_implobj_h

#include "current_border_implfwd.H"
#include "border_implfwd.H"
#include "connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A current border.

//! \see current_border_impl

class LIBCXX_HIDDEN current_border_implObj: virtual public obj {

 protected:

	//! Current border.

	const_border_impl border_thread_only;

 public:

	THREAD_DATA_ONLY(border);

	//! Constructor
	current_border_implObj(const const_border_impl &initial_border);

	//! Destructor
	~current_border_implObj();

	//! The current theme has been updated.

	//! The default implementation does nothing.

	virtual void theme_updated(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
