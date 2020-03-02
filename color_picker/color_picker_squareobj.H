/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_squareobj_h
#define x_w_color_picker_squareobj_h

#include "color_picker/color_picker_squarefwd.H"
#include "x/w/canvasobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see color_picker_square

class color_picker_squareObj : public canvasObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	color_picker_squareObj(const ref<implObj> &impl);

	//! Destructor
	~color_picker_squareObj();
};

LIBCXXW_NAMESPACE_END

#endif
