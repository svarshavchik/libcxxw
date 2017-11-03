/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/listlayoutmanagerbase.H"
#include "x/w/listlayoutmanager.H"

LIBCXXW_NAMESPACE_START

listlayoutmanagerbaseObj::listlayoutmanagerbaseObj()=default;

listlayoutmanagerbaseObj::~listlayoutmanagerbaseObj()=default;

void listlayoutmanagerbaseObj::remove_callback_factory()
{
	callback_factory_container_t::lock lock{callback_factory_container};

	*lock=nullptr;
}

void listlayoutmanagerbaseObj::remove_shortcut_factory()
{
	shortcut_factory_container_t::lock lock{shortcut_factory_container};

	*lock=nullptr;
}

std::function<list_item_status_change_callback_t>
listlayoutmanagerbaseObj::next_callback()
{
	std::function<list_item_status_change_callback_t> cb;

	callback_factory_container_t::lock lock{callback_factory_container};

	if (*lock)
		cb=(*lock)();

	return cb;
}

shortcut listlayoutmanagerbaseObj::next_shortcut()
{
	shortcut sc;

	shortcut_factory_container_t::lock lock{shortcut_factory_container};

	if (*lock)
		sc=(*lock)();

	return sc;
}

void listlayoutmanagerbaseObj::selected(size_t i, bool selected_flag)
{
	selected(i, selected_flag, {});
}

void listlayoutmanagerbaseObj::autoselect(size_t i)
{
	autoselect(i, {});
}

LIBCXXW_NAMESPACE_END
