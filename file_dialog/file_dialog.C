/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "file_dialog/file_dialog_impl.H"

LIBCXXW_NAMESPACE_START

file_dialogObj::file_dialogObj(const ref<implObj> &impl)
	: impl(impl)
{
}

file_dialogObj::~file_dialogObj()=default;

dialog file_dialogObj::the_dialog()
{
	return impl->the_dialog;
}

const_dialog file_dialogObj::the_dialog() const
{
	return impl->the_dialog;
}

LIBCXXW_NAMESPACE_END
