/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include "x/w/pane_layout_appearance.H"

LIBCXXW_NAMESPACE_START

// Helper used by <restore>

static new_panelayoutmanager_restored_position
restore_panelayoutmanager_position(uicompiler &compiler,
				    const std::string &name)
{
	new_panelayoutmanager_restored_position restored_position;

	restored_position.restore(compiler.positions_to_restore(),
				  name);

	return restored_position;
}

#include "uicompiler.inc.H/new_panelayout_parse_parameters.H"
#include "uicompiler.inc.H/new_panelayout_parser.H"

LIBCXXW_NAMESPACE_END
