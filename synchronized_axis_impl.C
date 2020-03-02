/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "synchronized_axis_impl.H"
#include "synchronized_axis_value.H"
#include "metrics_axis.H"
#include <x/derivedvalue.H>

LIBCXXW_NAMESPACE_START

synchronized_axisObj::synchronized_axisObj()
	: synchronized_axisObj{ref<implObj>::create()}
{
}

synchronized_axisObj::synchronized_axisObj(const ref<implObj> &impl)
	: impl{impl}
{
}

synchronized_axisObj::~synchronized_axisObj()=default;

synchronized_axisObj::implObj::implObj() : values{*this}
{
}

synchronized_axisObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
