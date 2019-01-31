/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef independent_shortcut_activationobj_h
#define independent_shortcut_activationobj_h

#include "shortcut/independent_shortcut_activationfwd.H"
#include "shortcut/shortcut_activation_element_impl.H"
#include "x/w/generic_windowobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see independent_shortcut_activation

class independent_shortcut_activationObj
	: public shortcut_activation_element_implObj {


 public:

	//! The window handler for this shortcut.
	const ref<generic_windowObj::handlerObj> window_handler;

	//! Implement shortcut_window_handler().
	generic_windowObj::handlerObj &shortcut_window_handler() override;

	//! Constructor
	independent_shortcut_activationObj(const ref<generic_windowObj
					   ::handlerObj> &window_handler);

	//! Destructor
	~independent_shortcut_activationObj();
};

LIBCXXW_NAMESPACE_END

#endif
