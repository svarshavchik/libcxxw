/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlabel.H"

LIBCXXW_NAMESPACE_START

child_element_init_params textlabel_config::default_child_element_init()
{
	return {
		"label@libcxx.com"
	};
}

textlabelObj::textlabelObj(const ref<implObj> &label_impl)
	: label_impl(label_impl)
{
}

textlabelObj::~textlabelObj()=default;

void textlabelObj::update(const text_param &string)
{
	label_impl->update(string);
}

LIBCXXW_NAMESPACE_END
