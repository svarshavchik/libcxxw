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

	//! This focusable should be tabbed to before the other one.

	void get_focus_before(const focusable &other);

	//! This focusable should be tabbed to after the other one

	void get_focus_after(const focusable &other);

private:

	//! How many real internal focusable objects comprise this focusable.

	//! \internal
	virtual size_t internal_impl_count() const LIBCXX_HIDDEN;

	//! Retrieve an internal focusable object.

	//! \internal
	virtual ref<focusableImplObj> get_impl(size_t) const LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
