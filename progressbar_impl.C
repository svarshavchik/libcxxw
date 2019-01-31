/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_impl.H"
#include "progressbar_slider.H"

LIBCXXW_NAMESPACE_START

progressbarObj::implObj::implObj(const ref<handlerObj> &handler,
				 const container &contents,
				 const progressbar_slider &slider)
	: handler(handler),
	  contents(contents),
	  slider(slider)
{
}

progressbarObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
