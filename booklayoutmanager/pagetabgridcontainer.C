/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetabgridcontainer_impl.H"
#include "always_visible.H"
#include "container_element.H"
#include "x/w/impl/themedim_element.H"
#include "x/w/impl/background_color_element.H"

LIBCXXW_NAMESPACE_START

pagetabgridcontainer_implObj
::pagetabgridcontainer_implObj(const ref<containerObj::implObj>
			       &parent_container,
			       const dim_arg &h_padding,
			       const dim_arg &v_padding,
			       const color_arg &inactive_color,
			       const color_arg &active_color)
	: superclass_t{h_padding, themedimaxis::width,
		v_padding, themedimaxis::height,
		inactive_color, active_color,
		parent_container}
{
}

pagetabgridcontainer_implObj
::~pagetabgridcontainer_implObj()=default;

LIBCXXW_NAMESPACE_END
