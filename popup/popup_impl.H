/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_impl_h
#define popup_impl_h

#include "popup/popup.H"
#include "windowobj.H"
#include "generic_window.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Popup implementation object

//! Owns a reference on the handler object. Sets window manager hint
//! indicating our parent window.

class LIBCXX_HIDDEN popupObj::implObj : public generic_windowObj::implObj {

public:
	//! Handler

	const ref<handlerObj> handler;

	//! Constructor

	implObj(IN_THREAD_ONLY,
		//! Popup handler
		const ref<handlerObj> &handler,

		//! Parent top level display element.
		const ref<generic_windowObj::handlerObj> &parent);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif