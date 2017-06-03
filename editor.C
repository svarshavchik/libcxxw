/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "editor.H"
#include "editor_impl.H"
#include "x/w/factory.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontcollection.H"
#include "peephole/peepholed_element.H"

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
	: superclass_t(impl),
	  impl(impl)
{
}

editorObj::~editorObj()=default;

dim_t editorObj::horizontal_increment(IN_THREAD_ONLY) const
{
	return impl->font->fc(IN_THREAD)->nominal_width();
}

dim_t editorObj::vertical_increment(IN_THREAD_ONLY) const
{
	return impl->font->fc(IN_THREAD)->height();
}


LIBCXXW_NAMESPACE_END
