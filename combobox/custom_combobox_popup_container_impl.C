/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "peepholed_toplevel_listcontainer/impl_element.H"
#include "x/w/impl/always_visible.H"
#include "image_button_internal.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

custom_combobox_popup_containerObj::implObj
::implObj(const container_impl &parent,
	  const new_listlayoutmanager &style)
	: superclass_t(style, parent)
{
}

custom_combobox_popup_containerObj::implObj::~implObj()=default;

const char *custom_combobox_popup_containerObj::implObj::label_theme_font()
	const
{
	return "combobox";
}

void custom_combobox_popup_containerObj::implObj
::horizvert_updated(ONLY IN_THREAD)
{
	superclass_t::horizvert_updated(IN_THREAD);
	update_current_selection_metrics(IN_THREAD);
}

bool custom_combobox_popup_containerObj::implObj
::update_tallest_row_height(ONLY IN_THREAD,
			    const tallest_row_height_t &new_tallest_height)
{
	auto flag=superclass_t::update_tallest_row_height(IN_THREAD,
							  new_tallest_height);

	if (flag)
		update_current_selection_metrics(IN_THREAD);

	return flag;
}

void custom_combobox_popup_containerObj::implObj
::update_current_selection_metrics(ONLY IN_THREAD)
{
	auto e=current_combobox_selection_element(IN_THREAD).getptr();

	if (e)
		e->get_minimum_override_element_impl()
			->set_minimum_override(IN_THREAD,
					       get_horizvert(IN_THREAD)
					       ->horiz.preferred(),
					       tallest_row_height(IN_THREAD)
					       .without_padding);
}

void custom_combobox_popup_containerObj::implObj
::set_current_combobox_selection_element_and_button(const element &e)
{
	e->impl->THREAD->run_as
		([me=ref<implObj>(this), e]
		 (ONLY IN_THREAD)
		 {
			 me->current_combobox_selection_element(IN_THREAD)=e;

			 // If there's anything here, keep it updated.
			 me->update_current_selection_metrics(IN_THREAD);
		 });
}

LIBCXXW_NAMESPACE_END
