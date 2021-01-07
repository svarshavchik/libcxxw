/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "radio_group.H"

LIBCXXW_NAMESPACE_START

radio_groupObj::radio_groupObj() : impl(ref<implObj>::create())
{
}

radio_groupObj::~radio_groupObj()=default;

LIBCXXW_NAMESPACE_END
