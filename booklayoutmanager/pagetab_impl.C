/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetab_impl.H"
#include "booklayoutmanager/pagetabgridcontainer_impl.H"
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
	  const dim_arg &h_padding,
	  const dim_arg &v_padding,
	  const color_arg &current_color,
	  const color_arg &notcurrent_color,
	  const color_arg &warm_color,
	  const color_arg &hot_color,
	  const font_arg &tab_font,
	  const color_arg &tab_font_color)

// Use warm_color for the cold_color temporarily.
// create_new_tab() will take care of initializing the initial
// cold color, for us.

: superclass_t{
	h_padding, themedimaxis::width,
		v_padding, themedimaxis::height,
		current_color, notcurrent_color,
		notcurrent_color, warm_color, hot_color,
		parent_container},
  my_pagetabgridcontainer_impl{my_pagetabgridcontainer_impl},
  tab_font{tab_font},
  tab_font_color{tab_font_color}
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
