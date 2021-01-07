/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panecontainer_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/theme_font_element.H"

LIBCXXW_NAMESPACE_START

panecontainer_implObj
::panecontainer_implObj(const container_impl &parent,
			const child_element_init_params &init_params)
	: superclass_t{parent->container_element_impl()
		.label_theme_font(), parent, init_params}
{
}

panecontainer_implObj::~panecontainer_implObj()=default;

LIBCXXW_NAMESPACE_END
