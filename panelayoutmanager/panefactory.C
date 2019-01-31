/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/panefactory.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/listlayoutmanager.H"

LIBCXXW_NAMESPACE_START

panefactoryObj::panefactoryObj(const panelayoutmanager &layout)
	: layout{layout}
{
}

panefactoryObj::~panefactoryObj()=default;

void panefactoryObj::configure_new_list(new_listlayoutmanager &nlm,
					bool synchronized)
{
	nlm.variable_height();
	nlm.list_border={};
	if (synchronized)
		nlm.vertical_scrollbar=scrollbar_visibility::always;

	set_scrollbar_visibility(scrollbar_visibility::never);
	padding(0);
	halign(halign::fill);
	valign(valign::fill);
}

LIBCXXW_NAMESPACE_END
