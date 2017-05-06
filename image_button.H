/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_button_h
#define image_button_h

#include "x/w/image_button.H"
#include "image_button_internalfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object for the image button display element.

class LIBCXX_HIDDEN image_buttonObj::implObj : virtual public obj {

 public:

	//! The internal focusable button.
	const image_button_internal button;

	//! Constructor
	implObj(const image_button_internal &button);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
