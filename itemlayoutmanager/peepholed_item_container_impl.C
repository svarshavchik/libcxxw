/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peepholed_item_container_impl.H"
#include "x/w/impl/theme_font_element.H"
#include "x/w/impl/themedim_element.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/itemlayoutmanager.H"
#include "x/w/item_layout_appearance.H"

LIBCXXW_NAMESPACE_START

peepholed_item_containerObj::implObj
::implObj(const container_impl &parent,
	  const focusable &horizontal_scrollbar,
	  const focusable &vertical_scrollbar,
	  const new_itemlayoutmanager &config)
	: superclass_t{
			// Use parent's default label font as a refernece font.

			parent->container_element_impl().label_theme_font(),

			std::forward_as_tuple
			(std::forward_as_tuple(config.appearance
					       ->itemlayout_h_padding,
					       themedimaxis::width),
			 std::forward_as_tuple(config.appearance
					       ->itemlayout_v_padding,
					       themedimaxis::height)),
			parent,
			child_element_init_params
			{
				"background@libcxx.com",
				// Initial metrics
				{{0, 0, dim_t::infinite()},
				 {0, 0, 0}}}},
	  horizontal_scrollbar{horizontal_scrollbar},
	  vertical_scrollbar{vertical_scrollbar}
{
}

peepholed_item_containerObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
