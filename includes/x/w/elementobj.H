/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementobj_h
#define x_w_elementobj_h

#include <x/w/elementfwd.H>
#include <x/w/screenfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all display elements.

//! \see element

class elementObj : virtual public obj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Application data.

	//! The library doesn't use this opaque pointer in any way.

	ptr<obj> appdata;

	//! Constructor
	elementObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~elementObj() LIBCXX_HIDDEN;

	//! Return my screen

	screen get_screen();

	//! Return my screen

	const_screen get_screen() const;

	//! Show this display element

	void show();

	//! Hide this display element

	void hide();
};

LIBCXXW_NAMESPACE_END

#endif
