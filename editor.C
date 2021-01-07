/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "x/w/factory.H"
#include "peephole/peepholed_fontelement.H"

LIBCXXW_NAMESPACE_START

editorObj::editorObj(const ref<implObj> &impl)
	: superclass_t{impl, impl},
	  impl(impl)
{
}

editorObj::~editorObj()=default;

focusable_impl editorObj::get_impl() const
{
	return impl;
}

LIBCXXW_NAMESPACE_END
