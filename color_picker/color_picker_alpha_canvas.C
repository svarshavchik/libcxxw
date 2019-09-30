/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_alpha_canvas_impl.H"

LIBCXXW_NAMESPACE_START

color_picker_alpha_canvasObj
::color_picker_alpha_canvasObj(const ref<implObj> &impl)
	: elementObj{impl}, impl{impl}
{
}

color_picker_alpha_canvasObj::~color_picker_alpha_canvasObj()=default;

LIBCXXW_NAMESPACE_END
