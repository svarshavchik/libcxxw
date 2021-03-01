/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include "messages.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

listlayoutmanagerObj::implObj
::implObj(const ref<listcontainer_pseudo_implObj> &container_impl,
	  const list_element &list_element_singleton)
	: singletonlayoutmanagerObj::implObj{container_impl,
					     list_element_singleton,
					     halign::fill, valign::fill},
	  container_impl{container_impl},
	  list_element_singleton(list_element_singleton)
{
}

listlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager listlayoutmanagerObj::implObj::create_public_object()
{
	return create_listlayoutmanager();
}

listlayoutmanager listlayoutmanagerObj::implObj::create_listlayoutmanager()
{
	return listlayoutmanager::create(ref{this});
}

void listlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	list_element_singleton->impl->recalculate(IN_THREAD);
	singletonlayoutmanagerObj::implObj::recalculate(IN_THREAD);

	update_tallest_row_height(IN_THREAD, list_element_singleton->impl
				  ->tallest_row_height(IN_THREAD));
}

void listlayoutmanagerObj::implObj::child_metrics_updated(ONLY IN_THREAD)
{
	// Always copy the child element metrics to our own...

	auto hv=list_element_singleton->impl->get_horizvert(IN_THREAD);

	get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD, hv->horiz, hv->vert);
}

// Override singletonlayoutmanager's recalculate() that attempts to
// compute the child element's size ...
void listlayoutmanagerObj::implObj
::recalculate(ONLY IN_THREAD, const element_impl &impl)
{
	// ... and then size the child element to match our size.
	impl->update_current_position(IN_THREAD, {
			0, 0,
			container_updated_position.width,
			container_updated_position.height});
}

void listlayoutmanagerObj::implObj
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	singletonlayoutmanagerObj::implObj::theme_updated(IN_THREAD, new_theme);

	update_tallest_row_height(IN_THREAD, list_element_singleton->impl
				  ->tallest_row_height(IN_THREAD));
}

void listlayoutmanagerObj::implObj
::update_tallest_row_height(ONLY IN_THREAD,
			    const tallest_row_height_t &)
{
}

LIBCXXW_NAMESPACE_END
