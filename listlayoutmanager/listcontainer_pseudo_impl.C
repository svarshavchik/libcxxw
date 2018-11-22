/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include "screen.H"
#include "defaulttheme.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/draw_info.H"

LIBCXXW_NAMESPACE_START

listcontainer_pseudo_implObj
::listcontainer_pseudo_implObj(const container_impl &parent)
	: superclass_t(parent)
{
}

listcontainer_pseudo_implObj::~listcontainer_pseudo_implObj()=default;

void listcontainer_pseudo_implObj::do_draw(ONLY IN_THREAD,
					 const draw_info &di,
					 const rectarea &areas)
{
	auto b=di.element_viewport.begin();
	auto e=di.element_viewport.end();

	if (b != e)
	{
		auto highest=b->y;
		auto lowest=b->y+b->height;

		while (++b != e)
		{
			if (b->y < highest)
				highest=b->y;

			auto l=b->y+b->height;

			if (l > lowest)
				lowest=l;
		}
		most_recent_visible_height(IN_THREAD)=
			dim_t::truncate( coord_t::truncate(lowest)-
					 coord_t::truncate(highest));
	}
	superclass_t::do_draw(IN_THREAD, di, areas);
}

size_t listcontainer_pseudo_implObj::rows(ONLY IN_THREAD) const
{
	size_t n=0;

	invoke_layoutmanager
		([&]
		 (const const_ref<listlayoutmanagerObj::implObj> &lm)
		 {
			 n=lm->list_element_singleton->impl->size();
		 });

	return n;
}

tallest_row_height_t listcontainer_pseudo_implObj::rowsize(ONLY IN_THREAD) const
{
	tallest_row_height_t h;

	invoke_layoutmanager
		([&]
		 (const const_ref<listlayoutmanagerObj::implObj> &lm)
		 {
			 h=lm->list_element_singleton->impl
				 ->tallest_row_height(IN_THREAD);
		 });

	return h;
}
///////////////////////////////////////////////////////////////////////////


LIBCXXW_NAMESPACE_END
