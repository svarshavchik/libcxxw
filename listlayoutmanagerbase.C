/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/listlayoutmanagerbase.H"

LIBCXXW_NAMESPACE_START

listlayoutmanagerbaseObj::listlayoutmanagerbaseObj()=default;

listlayoutmanagerbaseObj::~listlayoutmanagerbaseObj()=default;

void listlayoutmanagerbaseObj::remove_callback_factory()
{
	callback_factory_container_t::lock lock{callback_factory_container};

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

LIBCXXW_NAMESPACE_END
