/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldobj_h
#define x_w_input_fieldobj_h

#include <x/w/containerobj.H>

LIBCXXW_NAMESPACE_START

//! A text input_field.

//! \see input_field

class input_fieldObj : public containerObj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	input_fieldObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~input_fieldObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
