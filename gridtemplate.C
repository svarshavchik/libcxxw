/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridtemplate.H"
#include "messages.H"
#include "gridlayoutmanager.H"
#include "container.H"
#include "screen.H"
#include "element_screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

void gridtemplate::generate(const gridfactory &f,
			    const std::string &name) const
{
	auto iter=generators.find(name);

	if (iter == generators.end())
		throw EXCEPTION(gettextmsg(_("Element \"%1%\" not defined."),
					   name));

	iter->second(f);
}

/////////////////////////////////////////////////////////////////////////////


void gridlayoutmanagerObj::create(const std::string_view &name,
				  std::unordered_map<std::string,
				  std::function<void (const gridfactory &)>>
				  &&elements)
{
	gridtemplate telements{std::move(elements)};

	remove();

	auto theme=*current_theme_t::lock{
		impl->container_impl->get_element_impl().get_screen()
		->impl->current_theme};

	theme->layout_insert(gridlayoutmanager(this), &telements,
			     std::string{name.begin(), name.end()});
}

LIBCXXW_NAMESPACE_END
