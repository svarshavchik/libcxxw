/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menulayoutmanager_impl.H"
#include "menu/menuitemextrainfo.H"
#include "listlayoutmanager/listitemcontainer.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "x/w/menulayoutmanager.H"

LIBCXXW_NAMESPACE_START

menulayoutmanagerObj::implObj
::implObj(const ref<peepholed_toplevel_listcontainer_implObj> &container_impl,
	  const ref<listcontainerObj::implObj> &listcontainer_impl,
	  const new_listlayoutmanager &style)
	: peepholed_toplevel_listcontainer_layoutmanager_implObj
	  (container_impl, listcontainer_impl, style)
{
}

menulayoutmanagerObj::implObj::~implObj()=default;

layoutmanager menulayoutmanagerObj::implObj::create_public_object()
{
	return menulayoutmanager::create(ref<implObj>(this));
}

menuitemextrainfoptr menulayoutmanagerObj::implObj
::get_extrainfo(const listlayoutmanager &lm,
		size_t item_number)
{
	list_lock lock{lm};

	if (lm->impl->cols(item_number) != lm->impl->columns+1)
		return menuitemextrainfoptr(); // Probably a separator.

	// Last column is the extra_info we're looking for.

	return lm->item(item_number, lm->impl->columns-1);
}

LIBCXXW_NAMESPACE_END
