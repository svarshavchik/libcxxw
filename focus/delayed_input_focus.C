/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/delayed_input_focus.H"
#include "x/w/impl/focus/focusable.H"

LIBCXXW_NAMESPACE_START

delayed_input_focusObj::delayed_input_focusObj(const focusable_impl &me)
	: me_thread_only{me}
{
}

delayed_input_focusObj::~delayed_input_focusObj()=default;

LIBCXXW_NAMESPACE_END
