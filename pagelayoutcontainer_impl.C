/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pagelayoutcontainer_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/layoutmanager.H"

LIBCXXW_NAMESPACE_START

pagelayoutcontainerObj::implObj::implObj(const container_impl &parent)
	: superclass_t{parent}
{
}

pagelayoutcontainerObj::implObj::~implObj()=default;

void pagelayoutcontainerObj::implObj
::draw_after_visibility_updated(ONLY IN_THREAD,
				bool flag)
{
	superclass_t::draw_after_visibility_updated(IN_THREAD, flag);

	if (flag)
	{
		data(IN_THREAD).redraw_strategy=
			redraw_strategy_t::by_nesting_level;
		preclear_flag=true;
	}
}

void pagelayoutcontainerObj::implObj::do_draw(ONLY IN_THREAD,
					      const draw_info &di,
					      const rectarea &areas)
{
	if (!preclear_flag)
	{
		superclass_t::do_draw(IN_THREAD, di, areas);
		return;
	}

	preclear_flag=false;

	clear_to_color(IN_THREAD, di, areas);

	invoke_layoutmanager
		([&]
		 (const auto &lm)
		 {
			 lm->for_each_child
				 (IN_THREAD,
				  [&]
				  (const auto &e)
				  {
					  e->impl->schedule_redraw_recursively
						  (IN_THREAD);
				  });
		 });
}


LIBCXXW_NAMESPACE_END
