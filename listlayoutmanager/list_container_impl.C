/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_container_impl.H"
#include "container_element.H"
#include "draw_info.H"

LIBCXXW_NAMESPACE_START

list_container_implObj
::list_container_implObj(const ref<containerObj::implObj> &parent)
	: superclass_t(parent)
{
}

list_container_implObj::~list_container_implObj()=default;

void list_container_implObj::do_draw(IN_THREAD_ONLY,
				     const draw_info &di,
				     const rectangle_set &areas)
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

///////////////////////////////////////////////////////////////////////////


LIBCXXW_NAMESPACE_END
