/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/create_pane_properties.H"

LIBCXXW_NAMESPACE_START

create_pane_properties_t::create_pane_properties_t()
	: dimension{50},
	  left_padding_set("grid_horiz_padding"),
	  right_padding_set(left_padding_set),
	  top_padding_set("grid_vert_padding"),
	  bottom_padding_set(top_padding_set)
{
}

LIBCXXW_NAMESPACE_END
