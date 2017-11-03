/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlistlayoutmanager/extra_list_row_info.H"

LIBCXXW_NAMESPACE_START

void extra_list_row_infoObj::default_status_change_callback(list_lock &, size_t,
							    bool)
{
}

bool extra_list_row_infoObj::enabled() const
{
	return row_type == list_row_type_t::enabled;
}

LIBCXXW_NAMESPACE_END
