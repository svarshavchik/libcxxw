/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "radio_group.H"
#include "image_button_internal_impl.H"

LIBCXXW_NAMESPACE_START

radio_groupObj::implObj::implObj() : button_list(button_list_t::create())
{
}

radio_groupObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
