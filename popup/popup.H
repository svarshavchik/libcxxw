/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_popup_h
#define x_w_popup_h

#include "x/w/impl/popup/popup.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

void set_parent_window_of(const ref<generic_windowObj::handlerObj> &handler,
			  xcb_window_t parent_window_id) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
