/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup_imagebutton_impl.H"
#include "popup/popup_showhide_element.H"
#include "pixmap_with_picture.H"
#include "icon.H"
#include "x/w/metrics/axis.H"
#include "always_visible_element.H"
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

// We initially set our metrics based on the first icon's width.

popup_imagebutton_implObj
::popup_imagebutton_implObj(const ref<containerObj::implObj> &container,
			    const std::vector<icon> &icon_images,
			    const ref<elementObj::implObj> &popup_element_impl)
	: popup_imagebutton_implObj{container,
		icon_images,
		popup_element_impl, icon_images.at(0)->image->get_width()}
{
}

popup_imagebutton_implObj
::popup_imagebutton_implObj(const ref<containerObj::implObj> &container,
			    const std::vector<icon> &icon_images,
			    const ref<elementObj::implObj> &popup_element_impl,
			    dim_t first_icon_width)
	: superclass_t{popup_element_impl, container, icon_images,
		metrics::axis{first_icon_width, first_icon_width,
			first_icon_width},
		metrics::axis{0, first_icon_width, dim_t::infinite()}}
{
}

popup_imagebutton_implObj::~popup_imagebutton_implObj()=default;

void popup_imagebutton_implObj
::temperature_changed(IN_THREAD_ONLY,
		      const callback_trigger_t &trigger)
{
	set_image_number(IN_THREAD,
			 trigger,
			 hotspot_temperature(IN_THREAD)
			 == temperature::hot ? 1:0);
}

bool popup_imagebutton_implObj::activate_on_key(const key_event &ke)
{
	return superclass_t::activate_on_key(ke)
		|| (ke.notspecial() &&
		    (ke.keysym == XK_Down || ke.keysym == XK_KP_Down
		     || ke.keysym == XK_Right || ke.keysym == XK_KP_Right));
}

void popup_imagebutton_implObj::initialize(IN_THREAD_ONLY)
{
	superclass_t::initialize(IN_THREAD);
	resize_button_icons(IN_THREAD);
}

void popup_imagebutton_implObj::current_position_updated(IN_THREAD_ONLY)
{
	superclass_t::current_position_updated(IN_THREAD);
	resize_button_icons(IN_THREAD);
}

void popup_imagebutton_implObj::resize_button_icons(IN_THREAD_ONLY)
{
	auto height=data(IN_THREAD).current_position.height;

	resize(IN_THREAD, 0, height, icon_scale::nomore);
	set_image_number(IN_THREAD, {}, get_image_number());
}

void popup_imagebutton_implObj::update_image_metrics(IN_THREAD_ONLY)
{
	auto w=current_icon(IN_THREAD)->image->get_width();

	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 {w, w, w},
		 {0, current_icon(IN_THREAD)->image->get_height(),
				 dim_t::infinite()});

}

LIBCXXW_NAMESPACE_END
