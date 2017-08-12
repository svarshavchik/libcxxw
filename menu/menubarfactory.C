/*
** Copyright 2017 Double Precision, Inc.
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

menu menubarfactoryObj::add_text(const text_param &t)
{
	return add([&](const auto &f)
	    {
		    f->create_label(t);
	    });
}

LIBCXXW_NAMESPACE_END
