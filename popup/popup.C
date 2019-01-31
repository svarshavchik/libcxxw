/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"

LIBCXXW_NAMESPACE_START

popupObj::popupObj(const ref<implObj> &impl,
		   const layout_impl &layout)
	: generic_windowObj(impl, layout),
	  impl(impl)
{
}

popupObj::~popupObj()=default;

LIBCXXW_NAMESPACE_END
