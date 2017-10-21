/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialogobj_h
#define x_w_file_dialogobj_h

#include <x/w/file_dialogfwd.H>
#include <x/w/dialogfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see file_dialog

class file_dialogObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	file_dialogObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~file_dialogObj() LIBCXX_HIDDEN;

	//! Return the actual file dialog.

	dialog the_dialog();

	//! Return the actual file dialog.

	const_dialog the_dialog() const;
};

LIBCXXW_NAMESPACE_END

#endif
