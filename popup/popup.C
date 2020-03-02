/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
#include "x/w/impl/layoutmanager.H"

LIBCXXW_NAMESPACE_START

popupObj::popupObj(const ref<implObj> &impl,
		   const layout_impl &layout,
		 const layout_impl &content_layout)
	: generic_windowObj{impl, layout},
	  impl{impl},
	  content_layout{content_layout}
{
}

popupObj::~popupObj()=default;

layout_impl popupObj::get_layout_impl() const
{
	return content_layout;
}

LIBCXXW_NAMESPACE_END
