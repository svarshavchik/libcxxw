/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "popup/popup_attachedto_info.H"
#include "x/w/element.H"
#include "screen.H"
#include "grid_map_info.H"
#include "grid_element.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

custom_combobox_popup_containerObj
::custom_combobox_popup_containerObj(const ref<implObj> &impl,
				     const ref<listlayoutmanagerObj
				     ::implObj> &layout_impl,
				     const popup_attachedto_info
				     &attachedto_info)
	: listcontainerObj(impl, layout_impl),
	  list_focusable_owner(ref<focusableObj::ownerObj>::create(impl)),
	  impl(impl),
	  layout_impl(layout_impl),
	  attachedto_info(attachedto_info)
{
}

custom_combobox_popup_containerObj::~custom_combobox_popup_containerObj()
=default;


void custom_combobox_popup_containerObj::recalculate_metrics(IN_THREAD_ONLY)
{
	auto s=get_screen();

	max_width_value=attachedto_info->max_peephole_width(IN_THREAD, s);
	max_height_value=attachedto_info->max_peephole_height(IN_THREAD, s);

	// TODO: we shouldn't need horizontal increment, but default it to
	// something reasonable.

	{
		current_theme_t::lock lock{s->impl->current_theme};

		horizontal_increment_value=(*lock)->compute_width(5);
	}
}

dim_t custom_combobox_popup_containerObj::max_width(IN_THREAD_ONLY) const
{
	return max_width_value;
}

dim_t custom_combobox_popup_containerObj::max_height(IN_THREAD_ONLY) const
{
	return max_height_value;
}

element custom_combobox_popup_containerObj::get_element()
{
	return element(this);
}

dim_t custom_combobox_popup_containerObj::horizontal_increment(IN_THREAD_ONLY)
	const
{
	return horizontal_increment_value;
}

dim_t custom_combobox_popup_containerObj::vertical_increment(IN_THREAD_ONLY)
	const
{
	return impl->tallest_row_height(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
