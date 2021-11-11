/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlabel.H"
#include "x/w/focusable_label.H"

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
	update(string, {});
}

void textlabelObj::update(const text_param &string,
			  const label_hotspots_t &hotspots)
{
	label_impl->update(string, hotspots);
}

void textlabelObj::update(ONLY IN_THREAD, const text_param &string)
{
	update(IN_THREAD, string, {});
}

void textlabelObj::update(ONLY IN_THREAD,
			  const text_param &string,
			  const label_hotspots_t &hotspots)
{
	label_impl->update(IN_THREAD, string, hotspots);
}

LIBCXXW_NAMESPACE_END
