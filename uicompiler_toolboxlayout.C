/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/toolboxlayoutmanager.H"
#include "x/w/toolboxfactory.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

static void generate_toolboxfactory(const toolboxfactory &f,
				    uielements &elements,
				    const const_vector<toolboxfactory_generator>
				    &gen)
{
	for (const auto &g:*gen)
		g(f, elements);
}

static inline void toollayout_append_tools(const toolboxlayoutmanager &tlm,
					   uielements &elements,
					   const const_vector
					   <toolboxfactory_generator> &gen)
{
	generate_toolboxfactory(tlm->append_tools(), elements, gen);
}

static inline void toollayout_insert_tools(const toolboxlayoutmanager &tlm,
					   uielements &elements,
					   size_t position,
					   const const_vector
					   <toolboxfactory_generator> &gen)
{
	generate_toolboxfactory(tlm->insert_tools(position), elements, gen);
}

#include "uicompiler.inc.H/toolboxlayout_parse_parameters.H"
#include "uicompiler.inc.H/toolboxlayout_parser.H"

LIBCXXW_NAMESPACE_END
