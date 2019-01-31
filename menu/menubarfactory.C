/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/menu.H"
#include "x/w/label.H"

LIBCXXW_NAMESPACE_START

menubarfactoryObj::menubarfactoryObj(const menubarlayoutmanager &layout)
	: layout(layout)
{
}

menubarfactoryObj::~menubarfactoryObj()=default;

menu menubarfactoryObj::do_add_text(const text_param &t,
				    const function<menu_content_creator_t> &cf)
{
	return do_add(make_function<menu_creator_t>
		      ([&](const auto &f)
		       {
			       f->create_label(t);
		       }),
		      cf);
}

LIBCXXW_NAMESPACE_END
