/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_impl.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "x/w/impl/focus/focusframecontainer_element.H"
#include "x/w/impl/container_element.H"

LIBCXXW_NAMESPACE_START

static child_element_init_params create_menu_init_params(const popup &p)
{
	child_element_init_params params{"menufocusframe@libcxx.com"};

	params.attached_popup=p;

	return params;
}

menuObj::implObj::implObj(const popup &menu_popup,
			  const const_focus_border_appearance &appearance,
			  const container_impl &parent_container)
	: superclass_t{appearance,
		       0,
		       0,
		       parent_container,
		       parent_container,
		       create_menu_init_params(menu_popup)},
	  menu_popup{menu_popup}
{
}

menuObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
