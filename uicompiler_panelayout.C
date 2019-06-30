/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/panefactory.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

static void generate_panefactory(const panefactory &f, uielements &elements,
				 const const_vector<panefactory_generator> &gen)
{
	for (const auto &g:*gen)
		g(f, elements);
}

static inline void panelayout_append_panes(const panelayoutmanager &plm,
					   uielements &elements,
					   const const_vector
					   <panefactory_generator> &gen)
{
	generate_panefactory(plm->append_panes(), elements, gen);
}

static inline void panelayout_insert_panes(const panelayoutmanager &plm,
					   uielements &elements,
					   size_t position,
					   const const_vector
					   <panefactory_generator> &gen)
{
	generate_panefactory(plm->insert_panes(position), elements, gen);
}

static inline void panelayout_replace_panes(const panelayoutmanager &plm,
					    uielements &elements,
					    size_t position,
					    const const_vector
					    <panefactory_generator> &gen)
{
	generate_panefactory(plm->replace_panes(position), elements, gen);
}

static inline void panelayout_replace_all_panes(const panelayoutmanager &plm,
						uielements &elements,
						const const_vector
						<panefactory_generator> &gen)
{
	generate_panefactory(plm->replace_all_panes(), elements, gen);
}

#include "uicompiler.inc.H/panelayout_parse_parameters.H"
#include "uicompiler.inc.H/panelayout_parser.H"

LIBCXXW_NAMESPACE_END
