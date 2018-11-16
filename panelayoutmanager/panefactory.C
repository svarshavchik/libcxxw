/*
** Copyright 2018 Double Precision, Inc.
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

void panefactoryObj::configure_new_list(new_listlayoutmanager &nlm)
{
	nlm.variable_height();
	nlm.list_border={};
	set_scrollbar_visibility(x::w::scrollbar_visibility::never);
	padding(0);
	halign(x::w::halign::fill);
	valign(x::w::valign::fill);
}

LIBCXXW_NAMESPACE_END
