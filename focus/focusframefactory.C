/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusframefactory.H"
#include "layoutmanager.H"
#include "container.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"

LIBCXXW_NAMESPACE_START

focusframefactoryObj::focusframefactoryObj(const container &ffc)
	: glm(ffc->get_layoutmanager())
{
}

focusframefactoryObj::~focusframefactoryObj()=default;

ref<containerObj::implObj> focusframefactoryObj::get_container_impl()
{
	return glm->layoutmanagerObj::impl->container_impl;
}

elementObj::implObj &focusframefactoryObj::get_element_impl()
{
	return glm->layoutmanagerObj::impl->container_impl->get_element_impl();
}

void focusframefactoryObj::created(const element &e)
{
	glm->remove();

	// If the focusframe is inside a grid layout and the cell is filled
	// with the focus frame, return the courtesy by centering the contents
	// of the focus frame.
	glm->append_row()->padding(0)
		.halign(halign::center)
		.valign(valign::middle)
		.created_internally(e);
}

LIBCXXW_NAMESPACE_END
