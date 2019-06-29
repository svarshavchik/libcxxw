/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include "x/w/table_appearance.H"

LIBCXXW_NAMESPACE_START

// Helper used by <restore>

static new_tablelayoutmanager_restored_position
restore_tablelayoutmanager_position(uicompiler &compiler,
				    const std::string &name)
{
	new_tablelayoutmanager_restored_position restored_position;

	restored_position.restore(compiler.positions_to_restore(),
				  name);

	return restored_position;
}

#include "uicompiler.inc.H/new_tablelayout_parse_parameters.H"
#include "uicompiler.inc.H/new_tablelayout_parser.H"

LIBCXXW_NAMESPACE_END
