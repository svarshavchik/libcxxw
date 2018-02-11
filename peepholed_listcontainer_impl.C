/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_listcontainer_impl.H"
#include "x/w/listlayoutmanager.H"
#include "reference_font_element.H"
#include "themedim.H"
#include "child_element.H"

#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

peepholed_listcontainerObj::implObj
::implObj(const new_listlayoutmanager &style)
	: height{style.height}
{
}

peepholed_listcontainerObj::implObj::~implObj()=default;

void peepholed_listcontainerObj::implObj::initialize(IN_THREAD_ONLY)
{
	update_peephole_metrics(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::horizvert_updated(IN_THREAD_ONLY)
{
	update_peephole_metrics(IN_THREAD);
}

dim_t peepholed_listcontainerObj::implObj::rowsize(IN_THREAD_ONLY) const
{
	return dim_t::truncate
		(list_reference_font().font_height(IN_THREAD)
		 + list_v_padding()->pixels(IN_THREAD)
		 + list_v_padding()->pixels(IN_THREAD));
}

void peepholed_listcontainerObj::implObj
::theme_updated(IN_THREAD_ONLY,
		const defaulttheme &new_theme)
{
	update_peephole_metrics(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::update_peephole_metrics(IN_THREAD_ONLY)
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
