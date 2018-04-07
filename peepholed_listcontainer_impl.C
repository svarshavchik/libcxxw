/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_listcontainer_impl.H"
#include "x/w/listlayoutmanager.H"
#include "reference_font_element.H"
#include "x/w/impl/themedim.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container.H"

#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

peepholed_listcontainerObj::implObj
::implObj(const new_listlayoutmanager &style)
	: height{style.height}
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
	return dim_t::truncate
		(list_reference_font().font_height(IN_THREAD)
		 + list_v_padding()->pixels(IN_THREAD)
		 + list_v_padding()->pixels(IN_THREAD));
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
	// We keep our horizontal metrics, and override the vertical
	// metrics to the fixed height.
	//
	// Multiple rowsize by # of rows if the height got specified as rows.
	//
	// If the height got specified as a dim_axis_arg, retrieve the current
	// dimensions from the mixin-specified get_height_metrics().

	auto v=std::visit(visitor{
			[&, this](size_t rows)
			{
				auto h=dim_t::truncate(rowsize(IN_THREAD) *
						       dim_t{dim_t::truncate
								       (rows)}
						       );

				return metrics::axis{h, h, h};
			},
			[&, this](const dim_axis_arg &arg)
			{
				return get_height_metrics(IN_THREAD);

			}},
		height);

	auto hv=get_child_elementObj().get_horizvert(IN_THREAD);

	get_child_elementObj().child_container->container_element_impl()
		.get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD, hv->horiz, v);
}

LIBCXXW_NAMESPACE_END
