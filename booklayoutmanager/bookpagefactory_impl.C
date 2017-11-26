/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/bookpagefactory.H"
#include "x/w/shortcut.H"

LIBCXXW_NAMESPACE_START

bookpagefactoryObj::bookpagefactoryObj()=default;


bookpagefactoryObj::~bookpagefactoryObj()=default;

void bookpagefactoryObj::do_add(const function<void (const factory &,
						     const factory &)> &f)
{
	do_add(f, {});
}

LIBCXXW_NAMESPACE_END
