/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "messages.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/container.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

/////////////////////////////////////////////////////////////////////////////

void gridlayoutmanagerObj::generate(const std::string_view &name,
				    const const_uigenerators &generators,
				    uielements &elements)
{
	remove();

	auto iter=generators->gridlayoutmanager_generators.find({name.begin(),
								 name.end()});

	if (iter == generators->gridlayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}

LIBCXXW_NAMESPACE_END
