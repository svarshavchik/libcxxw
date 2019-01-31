/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_canvasobj_h
#define x_w_canvasobj_h

#include <x/w/canvasfwd.H>
#include <x/w/elementobj.H>
#include <x/w/dim_axis_arg.H>

LIBCXXW_NAMESPACE_START

//! An empty display element, perhaps with a background color.

//! \see element

class canvasObj : public elementObj {

public:
	class LIBCXX_PUBLIC implObj;

	const ref<implObj> impl;

	//! Constructor
	canvasObj(const ref<implObj> &impl);

	//! Destructor
	~canvasObj();

	//! Update the canvas's size.
	void update(const dim_axis_arg &new_width,
		    const dim_axis_arg &new_height);
};

LIBCXXW_NAMESPACE_END

#endif
