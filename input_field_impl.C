/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field.H"
#include "editor.H"
#include "peepholed_focusable.H"

LIBCXXW_NAMESPACE_START

input_fieldObj::implObj::implObj(const impl_mixin &impl,
				 const editor &editor_element)
	: impl(impl),
	  editor_element(editor_element)
{
}

input_fieldObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
