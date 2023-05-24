/*
** Copyright 2023 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/peepholelayoutmanager.H"
#include "x/w/peephole_appearance.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

static void set_peephole_factory(const const_vector<factory_generator
				 > &generators)
{
	// new_layoutmanager takes care of this.
}

#include "uicompiler.inc.H/new_peepholelayout_parse_parameters.H"
#include "uicompiler.inc.H/new_peepholelayout_parser.H"

LIBCXXW_NAMESPACE_END
