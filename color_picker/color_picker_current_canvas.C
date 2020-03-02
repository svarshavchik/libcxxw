/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_current_canvas_impl.H"

LIBCXXW_NAMESPACE_START

color_picker_current_canvasObj
::color_picker_current_canvasObj(const ref<implObj> &impl)
	: canvasObj{impl},
	  impl{impl}
{
}

color_picker_current_canvasObj::~color_picker_current_canvasObj()=default;

LIBCXXW_NAMESPACE_END
