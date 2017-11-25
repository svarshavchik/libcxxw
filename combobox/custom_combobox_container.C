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

void custom_combobox_containerObj
::do_get_impl(const function<internal_focusable_cb> &cb) const
{
	// Obtain the current selection's focusable implementation, tack
	// on combobox_button's focusable at the end, and return it as the
	// composite list.

	current_selection_focusable->get_impl
		([&]
		 (const auto &current_selection_group)
		 {
			 std::vector<ref<focusableImplObj>> composite;

			 composite.reserve(current_selection_group
					   .internal_impl_count+1);
			 composite.insert(composite.end(),
					  current_selection_group.impls,
					  current_selection_group.impls+
					  current_selection_group
					  .internal_impl_count);
			 composite.push_back(combobox_button->impl);

			 internal_focusable_group composite_group{
				 composite.size(),
					 &composite.at(0)};

			 cb(composite_group);
		 });
}

LIBCXXW_NAMESPACE_END
