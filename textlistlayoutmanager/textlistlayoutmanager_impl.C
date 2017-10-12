/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlistlayoutmanager/textlistlayoutmanager_impl.H"
#include "textlistlayoutmanager/textlist_impl.H"

LIBCXXW_NAMESPACE_START

textlistlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl,
	  const textlist &textlist_element)
	: singletonlayoutmanagerObj::implObj(container_impl,
					     textlist_element),
	textlist_element(textlist_element)
{
}

textlistlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager textlistlayoutmanagerObj::implObj::create_public_object()
{
	return textlistlayoutmanager::create(ref(this));
}

void textlistlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
{
	textlist_element->impl->recalculate(IN_THREAD);
	singletonlayoutmanagerObj::implObj::recalculate(IN_THREAD);
}

void textlistlayoutmanagerObj::implObj::child_metrics_updated(IN_THREAD_ONLY)
{
	// Always copy the child element metrics to our own...

	auto hv=textlist_element->impl->get_horizvert(IN_THREAD);

	container_impl->get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD, hv->horiz, hv->vert);
}

void textlistlayoutmanagerObj::implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	// ... and then size the child element to match our size.
	textlist_element->impl->update_current_position(IN_THREAD, {
			0, 0, position.width, position.height});
}

LIBCXXW_NAMESPACE_END
