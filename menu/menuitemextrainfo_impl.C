/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menuitemextrainfo_impl.H"
#include "shortcut/shortcut_activation_element.H"
#include "activated_in_thread.H"
#include "container_element.H"
#include "themedim_element.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

menuitemextrainfoObj::implObj::implObj(const ref<containerObj::implObj> &parent)
	: superclass_t("menu_shortcut_padding",
		       &defaultthemeObj::get_theme_width_dim_t, parent),
	  listlayoutmanager_container(parent)
{
}

menuitemextrainfoObj::implObj::~implObj()=default;

const char *menuitemextrainfoObj::implObj::label_theme_font() const
{
	return "menu_shortcut_font";
}

void menuitemextrainfoObj::implObj
::install_shortcut(const shortcut &new_shortcut,
		   const activated_in_thread &public_object)
{
	superclass_t::install_shortcut(new_shortcut, public_object);
}

LIBCXXW_NAMESPACE_END
