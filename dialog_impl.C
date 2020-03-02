/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dialog_impl.H"
#include "dialog_handler.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

dialogObj::implObj::implObj(const ref<handlerObj> &handler,
			    const ref<generic_windowObj::handlerObj> &parent,
			    const main_window_impl_args &args)
	: main_windowObj::implObj(args), handler(handler)
{
	set_parent_window_of(handler, parent->id());
}

dialogObj::implObj::~implObj()
{
	set_parent_window_of(handler, XCB_NONE);
}

LIBCXXW_NAMESPACE_END
