/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusframefactory.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"

LIBCXXW_NAMESPACE_START

focusframefactoryObj::focusframefactoryObj(const container &ffc)
	: factoryObj(ffc->impl),
	  glm(ffc->get_layoutmanager())
{
}

focusframefactoryObj::~focusframefactoryObj()=default;

void focusframefactoryObj::created(const element &e)
{
	glm->remove();

	glm->append_row()->padding(0).created_internally(e);
}

LIBCXXW_NAMESPACE_END
