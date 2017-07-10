/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "label.H"

LIBCXXW_NAMESPACE_START

labelObj::labelObj(const ref<implObj> &impl) : elementObj(impl),
					       impl(impl)
{
}

labelObj::~labelObj()=default;

void labelObj::update(const text_param &string)
{
	impl->update(string);
}

LIBCXXW_NAMESPACE_END
