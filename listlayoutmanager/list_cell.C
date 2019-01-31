/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_cell.H"

LIBCXXW_NAMESPACE_START

list_cellObj::list_cellObj(valign valignment)
	: valignment{valignment}
{
}

list_cellObj::~list_cellObj()=default;

LIBCXXW_NAMESPACE_END
