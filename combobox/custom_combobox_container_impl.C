/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "x/w/generic_window_appearance.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "x/w/custom_comboboxlayoutmanager.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

static inline child_element_init_params
create_init_params(const popup &my_popup)
{
	child_element_init_params init_params;

	init_params.attached_popup=my_popup;

	return init_params;
}

custom_combobox_containerObj::implObj
::implObj(const container_impl &parent_container,
	  const new_custom_comboboxlayoutmanager &nlm,
	  const custom_combobox_popup_container &popup_container,
	  const popup &attached_popup)
	: superclass_t{parent_container, create_init_params(attached_popup)},
	  popup_container{popup_container},
	  label_font{nlm.appearance->contents_appearance->label_font}
{
}

custom_combobox_containerObj::implObj::~implObj()=default;

font_arg custom_combobox_containerObj::implObj::label_theme_font() const
{
	return label_font;
}

element_impl custom_combobox_containerObj::implObj::hover_element_impl()
{
	auto hover_impl=element_impl{this};

	invoke_layoutmanager
		([&]
		 (const ref<gridlayoutmanagerObj::implObj> &lm)
		 {
			 auto e=lm->get(0, 0);

			 if (e)
				 hover_impl=e->impl;
		 });

	return hover_impl;
}

LIBCXXW_NAMESPACE_END
