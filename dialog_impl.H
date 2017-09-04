/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef dialog_impl_h
#define dialog_impl_h

#include "x/w/dialog.H"
#include "main_window.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN dialogObj::implObj : public main_windowObj::implObj {

 public:
	//! My handler object.

	const ref<handlerObj> handler;

	//! Constructor
	implObj(const ref<handlerObj> &handler,
		const ref<generic_windowObj::handlerObj> &parent,
		const main_window_impl_args &args);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
