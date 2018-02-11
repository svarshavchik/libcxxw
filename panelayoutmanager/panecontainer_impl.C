/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panecontainer_impl.H"
#include "container_element.H"
#include "reference_font_element.H"

LIBCXXW_NAMESPACE_START

panecontainer_implObj
::panecontainer_implObj(const ref<containerObj::implObj> &parent)
	: superclass_t{theme_font{"list"}, parent}
{
}

panecontainer_implObj::~panecontainer_implObj()=default;

LIBCXXW_NAMESPACE_END
