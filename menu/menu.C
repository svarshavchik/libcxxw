/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_impl.H"
#include "x/w/menulayoutmanager.H"
#include "gridlayoutmanager.H"
#include "focus/focusable.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

menuObj::menuObj(const ref<implObj> &impl,
		 const ref<layoutmanagerObj::implObj> &layout_impl)
	: focusable_containerObj(impl, layout_impl), impl(impl)
{
}

menuObj::~menuObj()=default;

ref<focusableImplObj> menuObj::get_impl() const
{
	ptr<focusableImplObj> f_impl;

	impl->invoke_layoutmanager([&]
				   (const ref<gridlayoutmanagerObj::implObj> &l)
				   {
					   f_impl=focusable(l->get(0, 0))
						   ->get_impl();
				   });

	return f_impl;
}

ref<layoutmanagerObj::implObj> menuObj::get_layout_impl() const
{
	return impl->menu_popup->get_layout_impl();
}

menulayoutmanager menuObj::get_layoutmanager()
{
	return focusable_containerObj::get_layoutmanager();
}

const_menulayoutmanager menuObj::get_layoutmanager() const
{
	return focusable_containerObj::get_layoutmanager();
}

LIBCXXW_NAMESPACE_END