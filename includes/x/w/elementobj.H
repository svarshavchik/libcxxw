/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementobj_h
#define x_w_elementobj_h

#include <x/w/elementfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all display elements.

//! \see element

class elementObj : virtual public obj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	elementObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~elementObj() noexcept LIBCXX_HIDDEN;

	//! Show this display element

	void show();

	//! Hide this display element

	void hide();
};

LIBCXXW_NAMESPACE_END

#endif
