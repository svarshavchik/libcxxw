/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "x/w/factory.H"
#include "peephole/peepholed_element.H"

LIBCXXW_NAMESPACE_START

editorObj::editorObj(const ref<implObj> &impl)
	: superclass_t(impl),
	  impl(impl)
{
}

editorObj::~editorObj()=default;

focusable_impl editorObj::get_impl() const
{
	return impl;
}

dim_t editorObj::horizontal_increment(IN_THREAD_ONLY) const
{
	return impl->font_nominal_width(IN_THREAD);
}

dim_t editorObj::vertical_increment(IN_THREAD_ONLY) const
{
	return impl->font_height(IN_THREAD);
}


LIBCXXW_NAMESPACE_END
