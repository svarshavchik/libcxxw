/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetab_impl.H"
#include "booklayoutmanager/pagetabgridcontainer_impl.H"
#include "x/w/impl/always_visible.H"
#include "hotspot_bgcolor_element.H"
#include "x/w/impl/container_element.H"
#include "singletonlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

pagetabObj::implObj
::implObj(const container_impl &parent_container,
	  const pagetabgridcontainer_impl &my_pagetabgridcontainer_impl,
	  const color_arg &warm_color,
	  const color_arg &hot_color)

	// Use warm_color for the cold_color temporarily.
	// create_new_tab() will take care of initializing the initial
	// cold color, for us.

	: superclass_t(warm_color, warm_color, hot_color,
		       parent_container),
	  my_pagetabgridcontainer_impl{my_pagetabgridcontainer_impl}
{
}

pagetabObj::implObj::~implObj()=default;

const char *pagetabObj::implObj::label_theme_font() const
{
	return "book_tab_font";
}

void pagetabObj::implObj::set_active(ONLY IN_THREAD, bool flag)
{
	background_color_element<hotspot_cold_color>::update
		(IN_THREAD,
		 flag ? my_pagetabgridcontainer_impl
		 ->background_color_element<tab_active_color_tag>
		 ::get(IN_THREAD)
		 : my_pagetabgridcontainer_impl
		 ->background_color_element<tab_inactive_color_tag>
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
