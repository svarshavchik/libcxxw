/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_labelobj_h
#define x_w_labelobj_h

#include <x/w/labelfwd.H>
#include <x/w/elementobj.H>
#include <x/w/textlabelobj.H>

LIBCXXW_NAMESPACE_START

//! A text label.

//! \see label

class labelObj : public elementObj, public textlabelObj {

public:
	//! Constructor
	labelObj(const ref<textlabelObj::implObj> &impl,
		 const ref<elementObj::implObj> &element_impl) LIBCXX_HIDDEN;

	//! Destructor
	~labelObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
