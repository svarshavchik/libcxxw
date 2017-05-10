/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusableobj_h
#define x_w_focusableobj_h

#include <x/w/focusablefwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all display elements that can receive input focus.

//! \see focusable

class focusableObj : virtual public obj {

public:
	class ownerObj;

	//! Constructor
	focusableObj() LIBCXX_HIDDEN;

	//! Destructor
	~focusableObj() LIBCXX_HIDDEN;

	//! Return the implementation object.

	//! \internal
	virtual ref<focusableImplObj> get_impl() const LIBCXX_HIDDEN=0;


	//! Enable/disable the focusable object.

	void set_enabled(bool);
};

LIBCXXW_NAMESPACE_END

#endif
