/*
** Copyright 2018-2019 Double Precision, Inc.
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

void panefactoryObj::configure_new_list(new_listlayoutmanager &nlm,
					bool synchronized)
{
	nlm.variable_height();
	nlm.set_pane_theme();

	if (synchronized)
		nlm.vertical_scrollbar=scrollbar_visibility::always;

	appearance=pane_appearance_base::focusable_list();
}

LIBCXXW_NAMESPACE_END
