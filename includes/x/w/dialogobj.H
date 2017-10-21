/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogobj_h
#define x_w_dialogobj_h

#include <x/w/dialogfwd.H>
#include <x/w/main_windowobj.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see dialog

class dialogObj : public main_windowObj {

public:
	class implObj;
	class handlerObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	dialogObj(const dialog_args &) LIBCXX_PUBLIC;

	//! Constructor for a standard theme-specified dialog.

	//! \internal
	dialogObj(const dialog_args &,
		  const std::string_view &,
		  const standard_dialog_elements_t &) LIBCXX_HIDDEN;

	//! Constructor for a standard theme-specified dialog.

	//! \internal
	dialogObj(const dialog_args &,
		  const std::string_view &,
		  standard_dialog_elements_t &&) LIBCXX_HIDDEN;

	//! Destructor
	~dialogObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
