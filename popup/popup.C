/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"

LIBCXXW_NAMESPACE_START

popupObj::popupObj(const ref<implObj> &impl,
		   const ref<layoutmanagerObj::implObj> &layout)
	: generic_windowObj(impl, layout),
	  impl(impl)
{
}

popupObj::~popupObj()=default;

LIBCXXW_NAMESPACE_END