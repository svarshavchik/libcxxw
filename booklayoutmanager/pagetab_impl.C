/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetab_impl.H"
#include "booklayoutmanager/pagetabgridcontainer_impl.H"
#include "x/w/bookpage_appearance.H"
#include "x/w/impl/always_visible.H"
#include "hotspot_bgcolor_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/singletonlayoutmanager.H"
#include "x/w/impl/background_color_element.H"
#include "x/w/impl/themedim_element.H"

LIBCXXW_NAMESPACE_START

pagetabObj::implObj
::implObj(const container_impl &parent_container,
	  const pagetabgridcontainer_impl &my_pagetabgridcontainer_impl,
	  const const_bookpage_appearance &appearance)

// Use warm_color for the cold_color temporarily.
// create_new_tab() will take care of initializing the initial
// cold color, for us.

	: superclass_t{
		       appearance->horiz_padding, themedimaxis::width,
		       appearance->vert_padding, themedimaxis::height,
		       appearance->current_color,
		       appearance->noncurrent_color,
		       appearance->noncurrent_color,
		       appearance->warm_color,
		       appearance->active_color,
		       parent_container},
	  my_pagetabgridcontainer_impl{my_pagetabgridcontainer_impl},
	  tab_font{appearance->label_font},
	  tab_font_color{appearance->label_foreground_color}
{
}

pagetabObj::implObj::~implObj()=default;

font_arg pagetabObj::implObj::label_theme_font() const
{
	return tab_font;
}

color_arg pagetabObj::implObj::label_theme_color() const
{
	return tab_font_color;
}

void pagetabObj::implObj::set_active(ONLY IN_THREAD, bool flag)
{
	background_color_element<hotspot_cold_color>::update
		(IN_THREAD,
		 flag ? background_color_element<current_color_tag>
		 ::get(IN_THREAD)
		 : background_color_element<notcurrent_color_tag>
		 ::get(IN_THREAD));
	temperature_changed(IN_THREAD, {});
}

elementptr pagetabObj::implObj::get()
{
	elementptr e;

	invoke_layoutmanager([&]
			     (const ref<singletonlayoutmanagerObj::implObj> &lm)
			     {
				     e=lm->get();
			     });

	return e;
}

LIBCXXW_NAMESPACE_END
