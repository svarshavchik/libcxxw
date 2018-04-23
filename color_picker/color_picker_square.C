/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_square_impl.H"
#include "x/w/factory.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

color_picker_squareObj::color_picker_squareObj(const ref<implObj> &impl)
	: canvasObj{impl},
	  impl{impl}
{
}

color_picker_squareObj::~color_picker_squareObj()=default;

LIBCXXW_NAMESPACE_END
