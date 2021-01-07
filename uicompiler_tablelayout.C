/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

static void tablelayout_replace_header(const tablelayoutmanager &tlm,
				       uielements &elements,
				       size_t column,
				       const factory_generator &gen)
{
	gen(tlm->replace_header(column), elements);
}

#include "uicompiler.inc.H/tablelayout_parse_parameters.H"
#include "uicompiler.inc.H/tablelayout_parser.H"

LIBCXXW_NAMESPACE_END
