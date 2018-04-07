/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_canvasobj_h
#define x_w_canvasobj_h

#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

//! An empty display element, perhaps with a background color.

//! \see element

class canvasObj : public elementObj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	canvasObj(const ref<implObj> &impl);

	//! Destructor
	~canvasObj();
};

LIBCXXW_NAMESPACE_END

#endif
