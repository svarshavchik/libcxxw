/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "image_button.H"
#include "image_button_internal.H"

LIBCXXW_NAMESPACE_START

image_buttonObj::implObj::implObj(const image_button_internal &button)
	: button(button)
{
}

image_buttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
