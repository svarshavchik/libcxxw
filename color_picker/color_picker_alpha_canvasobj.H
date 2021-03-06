/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_alpha_canvasobj_h
#define x_w_color_picker_alpha_canvasobj_h

#include "color_picker/color_picker_alpha_canvasfwd.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see color_picker_alpha_canvas

class color_picker_alpha_canvasObj: public elementObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	color_picker_alpha_canvasObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~color_picker_alpha_canvasObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
