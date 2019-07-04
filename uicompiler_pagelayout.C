/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/pagefactory.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

static void generate_pagefactory(const pagefactory &f, uielements &elements,
				 const const_vector<pagefactory_generator> &gen)
{
	for (const auto &g:*gen)
		g(f, elements);
}

static inline void pagelayout_append(const pagelayoutmanager &plm,
				     uielements &elements,
				     const const_vector <pagefactory_generator>
				     &gen)
{
	generate_pagefactory(plm->append(), elements, gen);
}

static inline void pagelayout_insert(const pagelayoutmanager &plm,
				     uielements &elements,
				     size_t position,
				     const const_vector <pagefactory_generator>
				     &gen)
{
	generate_pagefactory(plm->insert(position), elements, gen);
}

#include "uicompiler.inc.H/pagelayout_parse_parameters.H"
#include "uicompiler.inc.H/pagelayout_parser.H"

LIBCXXW_NAMESPACE_END
