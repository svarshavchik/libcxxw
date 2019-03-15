/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_h
#define x_w_button_h

#include <x/w/buttonfwd.H>
#include <x/w/buttonobj.H>
#include <x/w/border_arg.H>
#include <x/w/element.H>
#include <x/w/focusable.H>
#include <x/w/border_arg.H>
#include <x/w/hotspot_bgcolor.H>
#include <x/w/text_param.H>
#include <x/w/shortcut.H>
#include <x/optional_args.H>

LIBCXXW_NAMESPACE_START

struct button_config {
	border_arg border;
};

const button_config &normal_button();

const button_config &default_button();

LIBCXXW_NAMESPACE_END

#endif
