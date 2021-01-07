/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_impl.H"
#include "x/w/listlayoutmanager.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/singletonlayoutmanager.H"
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
				   (const ref<singletonlayoutmanagerObj
				    ::implObj> &l)
				   {
					   f_impl=focusable(l->get())
						   ->get_impl();
				   });

	return f_impl;
}

layout_impl menuObj::get_layout_impl() const
{
	return impl->menu_popup->get_layout_impl();
}

void menuObj::on_popup_state_update(const functionref<element_state_callback_t>
				    &callback)
{
	impl->menu_popup->on_state_update(callback);
}

void menuObj::on_popup_state_update(ONLY IN_THREAD,
				    const functionref<element_state_callback_t>
				    &callback)
{
	impl->menu_popup->on_state_update(IN_THREAD, callback);
}

LIBCXXW_NAMESPACE_END
