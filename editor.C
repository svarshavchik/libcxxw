/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "x/w/factory.H"

LIBCXXW_NAMESPACE_START

editor create_editor(const ref<containerObj::implObj> &parent_container,
		     const text_param &initial_contents,
		     const input_field_config &config)
{
	auto impl=ref<editorObj::implObj>::create(parent_container,
						  initial_contents,
						  config);

	return editor::create(impl);
}

editorObj::editorObj(const ref<implObj> &impl)
	: elementObj(impl),
	  focusableObj::ownerObj(impl),
	  impl(impl)
{
}

editorObj::~editorObj()=default;

LIBCXXW_NAMESPACE_END
