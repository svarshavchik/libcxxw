/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/panefactory.H"
#include "x/w/panelayoutmanager.H"

LIBCXXW_NAMESPACE_START

panefactoryObj::panefactoryObj(const panelayoutmanager &layout)
	: layout{layout}
{
}

panefactoryObj::~panefactoryObj()=default;

LIBCXXW_NAMESPACE_END
