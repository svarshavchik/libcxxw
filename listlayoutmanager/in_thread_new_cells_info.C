/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/in_thread_new_cells_info.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "x/w/listlayoutmanager.H"
#include "popup/popup_handler.H"

LIBCXXW_NAMESPACE_START

in_thread_new_cells_infoObj
::in_thread_new_cells_infoObj(const listlayoutmanager &lm,
			      const std::vector<list_item_param> &items)
{
	auto list_impl=lm->impl->list_element_singleton->impl;

	list_impl->list_style.create_cells(items, lm->impl, info);
}

in_thread_new_cells_infoObj::~in_thread_new_cells_infoObj()=default;

LIBCXXW_NAMESPACE_END
