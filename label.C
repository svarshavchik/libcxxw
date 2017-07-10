/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/label.H"
#include "textlabel.H"

LIBCXXW_NAMESPACE_START

labelObj::labelObj(const ref<textlabelObj::implObj> &impl,
		   const ref<elementObj::implObj> &element_impl)
	: elementObj(element_impl),
	  textlabelObj(impl)
{
}

labelObj::~labelObj()=default;

LIBCXXW_NAMESPACE_END
