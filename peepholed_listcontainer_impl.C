/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_listcontainer_impl.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/impl/theme_font_element.H"
#include "x/w/impl/themedim.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container.H"

#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

peepholed_listcontainerObj::implObj
::implObj(const new_listlayoutmanager &style)
	: height{style.height_value}
{
}

peepholed_listcontainerObj::implObj::~implObj()=default;

void peepholed_listcontainerObj::implObj::initialize(ONLY IN_THREAD)
{
	update_peephole_metrics(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::horizvert_updated(ONLY IN_THREAD)
{
	update_peephole_metrics(IN_THREAD);
}

dim_t peepholed_listcontainerObj::implObj::rowsize(ONLY IN_THREAD) const
{
	return get_pseudo_impl().rowsize(IN_THREAD).with_padding;
}

size_t peepholed_listcontainerObj::implObj::rows(ONLY IN_THREAD) const
{
	return get_pseudo_impl().rows(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::theme_updated(ONLY IN_THREAD,
		const defaulttheme &new_theme)
{
	update_peephole_metrics(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::update_peephole_metrics(ONLY IN_THREAD)
{
	// If the list specified dim_axis_arg for the list's vertical
	// size, update the vertical metrics here.

	std::visit
		(visitor
		 {
		  [&, this](const std::tuple<size_t, size_t> &rows)
		  {
		  },
		  [&, this](const dim_axis_arg &arg)
		  {
			  auto v=get_height_metrics(IN_THREAD);

			  auto &peepholed_listcontainer=get_pseudo_impl();
			  auto &peephole=peepholed_listcontainer.child_container
				  ->container_element_impl();

			  auto hv=peepholed_listcontainer
				  .get_horizvert(IN_THREAD);

			  peephole.get_horizvert(IN_THREAD)
				  ->set_element_metrics(IN_THREAD, hv->horiz,
							v);
		  }},
		 height);
}

LIBCXXW_NAMESPACE_END
