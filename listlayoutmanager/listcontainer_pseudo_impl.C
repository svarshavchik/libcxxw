/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "listlayoutmanager/listcontainer_dim_elementfwd.H"
#include "screen.H"
#include "defaulttheme.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/draw_info.H"

LIBCXXW_NAMESPACE_START

listcontainer_cell_paddingsObj
::listcontainer_cell_paddingsObj(const new_listlayoutmanager &nlm)
	: lr_paddings{nlm.lr_paddings}
{
}

listcontainer_cell_paddingsObj::~listcontainer_cell_paddingsObj()=default;

void listcontainer_cell_paddingsObj::initialize(ONLY IN_THREAD,
						elementObj::implObj
						&element_impl)
{
	lr_padding_pixels(IN_THREAD).clear();

	auto screen_impl=element_impl.get_screen()->impl;

	current_theme_t::lock lock{screen_impl->current_theme};

	theme_updated(IN_THREAD, *lock);
}

std::tuple<dim_t, dim_t>
listcontainer_cell_paddingsObj::get_paddings(ONLY IN_THREAD,
					     size_t n,
					     dim_t default_value) const
{
	auto iter=lr_padding_pixels(IN_THREAD).find(n);

	if (iter == lr_padding_pixels(IN_THREAD).end())
		return {default_value, default_value};

	return iter->second;
}

void listcontainer_cell_paddingsObj::theme_updated(ONLY IN_THREAD,
						   const defaulttheme
						   &new_theme)
{
	for (const auto &dim_args:lr_paddings)
	{
		auto &[l,r]=dim_args.second;

		auto lp=new_theme->get_theme_dim_t(l, themedimaxis::width);
		auto rp=new_theme->get_theme_dim_t(r, themedimaxis::width);

		lr_padding_pixels(IN_THREAD)[dim_args.first]={lp, rp};
	}
}

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

///////////////////////////////////////////////////////////////////////////


LIBCXXW_NAMESPACE_END
