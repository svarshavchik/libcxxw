/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_labelobj_h
#define x_w_labelobj_h

#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

//! A text label.

//! \see label

class labelObj : public elementObj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	labelObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~labelObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif