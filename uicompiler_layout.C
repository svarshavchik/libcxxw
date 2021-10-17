/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "uicompiler.H"
#include "messages.H"
#include "x/w/uielements.H"

LIBCXXW_NAMESPACE_START

static void invoke_creator(const layoutmanager &layout,
			   const uielements &elements,
			   const std::string &creator)
{
	auto iter=elements.layout_creators.find(creator);

	if (iter == elements.layout_creators.end())
		throw EXCEPTION(gettextmsg
				(_("Layout manager creator \"%1%\" "
				   "was not found"),
				 creator));

	iter->second(layout);
}

#include "uicompiler.inc.H/layout_parse_parameters.H"
#include "uicompiler.inc.H/layout_parser.H"

LIBCXXW_NAMESPACE_END
