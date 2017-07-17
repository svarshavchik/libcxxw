/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_comboboxlayoutmanager.H"
#include "image_button_internal_impl.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

custom_combobox_containerObj
::custom_combobox_containerObj(const ref<implObj> &impl,
			       const ref<custom_comboboxlayoutmanagerObj
			       ::implObj> &layout_manager_impl,
			       const focusable &current_selection_focusable,
			       const image_button_internal
			       &combobox_button,
			       const popup &popup_window)
	: focusable_containerObj(impl, layout_manager_impl),
	  impl(impl),
	  layout_manager_impl(layout_manager_impl),
	  current_selection_focusable(current_selection_focusable),
	  combobox_button(combobox_button),
	  popup_window(popup_window)
{
}

custom_combobox_containerObj::~custom_combobox_containerObj()=default;

// This is a composite focusable, the current selection focusable, and the
// focusable combo-box button.
ref<focusableImplObj> custom_combobox_containerObj::get_impl() const
{
	return current_selection_focusable->get_impl();
}

size_t custom_combobox_containerObj::internal_impl_count() const
{
	return 1+current_selection_focusable->internal_impl_count();
}

ref<focusableImplObj> custom_combobox_containerObj::get_impl(size_t n) const
{
	if (n < current_selection_focusable->internal_impl_count())
		return current_selection_focusable->get_impl(n);

	return combobox_button->impl;
}

LIBCXXW_NAMESPACE_END
