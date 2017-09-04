/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogobj_h
#define x_w_dialogobj_h

#include <x/w/dialogfwd.H>
#include <x/w/main_windowobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see dialog

class dialogObj : public main_windowObj {

public:
	class implObj;
	class handlerObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Parent main window or dialog.

	const main_window parent;

	//! Constructor
	dialogObj(const main_window &parent,
		  const ref<implObj> &impl,
		  const ref<layoutmanagerObj::implObj> &lm) LIBCXX_HIDDEN;

	//! Destructor
	~dialogObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
