/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_impl.H"
#include "x/w/listlayoutmanager.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/focus/focusable.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

menuObj::menuObj(const ref<implObj> &impl,
		 const layout_impl &container_layout_impl)
	: focusable_containerObj{impl, container_layout_impl}, impl{impl}
{
}

menuObj::~menuObj()=default;

focusable_impl menuObj::get_impl() const
{
	focusable_implptr f_impl;

	impl->invoke_layoutmanager([&]
				   (const ref<gridlayoutmanagerObj::implObj> &l)
				   {
					   f_impl=focusable(l->get(0, 0))
						   ->get_impl();
				   });

	return f_impl;
}

layout_impl menuObj::get_layout_impl() const
{
	return impl->menu_popup->get_layout_impl();
}

listlayoutmanager menuObj::get_layoutmanager()
{
	return focusable_containerObj::get_layoutmanager();
}

const_listlayoutmanager menuObj::get_layoutmanager() const
{
	return focusable_containerObj::get_layoutmanager();
}

LIBCXXW_NAMESPACE_END
