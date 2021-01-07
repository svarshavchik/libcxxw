/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "shortcut/independent_shortcut_activation.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

generic_windowObj::handlerObj &independent_shortcut_activationObj
::shortcut_window_handler()
{
	return *window_handler;
}

independent_shortcut_activationObj
::independent_shortcut_activationObj(const ref<generic_windowObj
				     ::handlerObj> &window_handler)
	: window_handler{window_handler}
{
}

independent_shortcut_activationObj::~independent_shortcut_activationObj()
{
	uninstall_shortcut();
}

LIBCXXW_NAMESPACE_END
