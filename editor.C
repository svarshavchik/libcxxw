/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "x/w/factory.H"

LIBCXXW_NAMESPACE_START

editor create_editor(const factory &f,
		     const text_param &initial_contents,
		     const input_field_config &config)
{
	auto impl=ref<editorObj::implObj>::create(f->container_impl,
						  initial_contents,
						  config);

	auto e=editor::create(impl);

	f->created_internally(e);

	return e;
}

editorObj::editorObj(const ref<implObj> &impl)
	: elementObj(impl),
	  focusableObj::ownerObj(impl),
	  impl(impl)
{
}

editorObj::~editorObj()=default;

LIBCXXW_NAMESPACE_END
