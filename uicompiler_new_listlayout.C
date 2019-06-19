/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "messages.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

/* selection_type */

static list_selection_type_cb_t to_selection_type(const theme_parser_lock &lock,
						  const char *element,
						  const char *parent)
{
	auto v=lowercase_single_value(lock, element, parent);

	if (v == "single")
		return single_selection_type;

	if (v == "single_optional")
		return single_optional_selection_type;

	if (v == "multiple")
		return multiple_selection_type;

	if (v != "no")
		throw EXCEPTION(gettextmsg(_("\"%1%\" is not a valid "
					     "selection type for <%2%>"),
					   v, parent));

	return no_selection_type;
}

#include "uicompiler.inc.H/new_listlayout_parse_parameters.H"
#include "uicompiler.inc.H/new_listlayout_parser.H"

LIBCXXW_NAMESPACE_END
