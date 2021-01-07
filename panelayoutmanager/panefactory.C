/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/panefactory.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/pane_appearance.H"

LIBCXXW_NAMESPACE_START

panefactoryObj::panefactoryObj(const panelayoutmanager &layout)
	: layout{layout},
	  appearance{pane_appearance::base::theme()}
{
}

panefactoryObj::~panefactoryObj()=default;

void panefactoryObj::configure_for_new_list()
{
	appearance=pane_appearance_base::focusable_list();
}

void panefactoryObj::configure_new_list(new_listlayoutmanager &nlm,
					bool synchronized)
{
	nlm.configure_for_pane(synchronized);
	configure_for_new_list();
}

LIBCXXW_NAMESPACE_END
