/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetabsingletonlayoutmanager_implobj.H"
#include "booklayoutmanager/pagetabgridcontainer_impl.H"

LIBCXXW_NAMESPACE_START

pagetabsingletonlayoutmanager_implObj
::pagetabsingletonlayoutmanager_implObj
	(const ref<pagetabObj::implObj> &pagetab_container,
	 const element &initial_element)
		: singletonlayoutmanagerObj::implObj{pagetab_container,
		initial_element},
	pagetab_container{pagetab_container}
{
}

pagetabsingletonlayoutmanager_implObj::~pagetabsingletonlayoutmanager_implObj()
=default;

dim_t pagetabsingletonlayoutmanager_implObj::get_left_padding(ONLY IN_THREAD)
{
	return pagetab_container->my_pagetabgridcontainer_impl
		->themedim_element<tab_h_padding_tag>
		::pixels(IN_THREAD);
}

dim_t pagetabsingletonlayoutmanager_implObj::get_right_padding(ONLY IN_THREAD)
{
	return pagetab_container->my_pagetabgridcontainer_impl
		->themedim_element<tab_h_padding_tag>
		::pixels(IN_THREAD);
}

dim_t pagetabsingletonlayoutmanager_implObj::get_top_padding(ONLY IN_THREAD)
{
	return pagetab_container->my_pagetabgridcontainer_impl
		->themedim_element<tab_v_padding_tag>
		::pixels(IN_THREAD);
}

dim_t pagetabsingletonlayoutmanager_implObj::get_bottom_padding(ONLY IN_THREAD)
{
	return pagetab_container->my_pagetabgridcontainer_impl
		->themedim_element<tab_v_padding_tag>
		::pixels(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
