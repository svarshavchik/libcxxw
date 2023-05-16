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
#include "x/w/table_appearance.H"

LIBCXXW_NAMESPACE_START

static void add_table_header_factory(
	new_tablelayoutmanager_plainptr new_layout,
	uielements &factories,
	const const_vector <factory_generator> &generators)
{
	new_layout->header_factories.push_back(
		[&factories, generators]
		(const factory &f)
		{
			for (const auto &g:*generators)
			{
				g(f, factories);
			}
		}
	);

	new_layout->columns=
		new_layout->header_factories.size();
}

#include "uicompiler.inc.H/new_tablelayout_parse_parameters.H"
#include "uicompiler.inc.H/new_tablelayout_parser.H"

LIBCXXW_NAMESPACE_END
