/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_impl.H"
#include "gridlayoutmanager.H"
#include "focus/focusable.H"

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

LIBCXXW_NAMESPACE_END
