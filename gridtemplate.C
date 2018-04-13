/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridtemplate.H"
#include "messages.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/container.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

void gridtemplate::generate(const factory &f,
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
				  const std::unordered_map<std::string,
				  functionref<void (const factory &)>>
				  &elements,
				  const std::unordered_map<std::string,
				  shortcut> &shortcuts,
				  std::unordered_map<std::string,
				  container> &new_layouts)
{
	gridtemplate telements{elements, shortcuts, new_layouts};

	remove();

	auto theme=*current_theme_t::lock{
		impl->layout_container_impl
		->container_element_impl().get_screen()
		->impl->current_theme};

	theme->layout_insert(gridlayoutmanager(this), &telements,
			     std::string{name.begin(), name.end()});
}

LIBCXXW_NAMESPACE_END
