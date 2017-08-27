/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "element.H"

LIBCXXW_NAMESPACE_START

listitemcontainerObj::implObj
::implObj(const ref<listcontainerObj::implObj> &parent_container,
	  const child_element_init_params &params)
	: superclass_t(parent_container, params),
	  listitem_sharedstate(ref<listitem_sharedstateObj>::create()),
	  parent_container(parent_container)
{
}

listitemcontainerObj::implObj::~implObj()=default;

void listitemcontainerObj::implObj::pointer_focus(IN_THREAD_ONLY)
{
	superclass_t::pointer_focus(IN_THREAD);

	parent_container->pointer_focus(IN_THREAD,
					ref<elementObj::implObj>(this));
}

bool listitemcontainerObj::implObj::selected() const
{
	return (*listitem_shared_state_t::lock{listitem_sharedstate})
		->selected_flag;
}

void listitemcontainerObj::implObj::selected(bool v)
{
	(*listitem_shared_state_t::lock{listitem_sharedstate})->selected_flag=v;
}

bool listitemcontainerObj::implObj::enabled(IN_THREAD_ONLY)
{
	return enabled();
}

bool listitemcontainerObj::implObj::enabled() const
{
	return (*listitem_shared_state_t::lock{listitem_sharedstate})
		->state != listitem_sharedstateObj::state_t::disabled;
}

void listitemcontainerObj::implObj::enabled(bool v)
{
	listitem_shared_state_t::lock lock{listitem_sharedstate};

	if ((*lock)->state != listitem_sharedstateObj::state_t::unavailable)
		(*lock)->state=v ? listitem_sharedstateObj::state_t::enabled
			: listitem_sharedstateObj::state_t::disabled;
}

bool listitemcontainerObj::implObj::selectable() const
{
	return (*listitem_shared_state_t::lock{listitem_sharedstate})
		->state != listitem_sharedstateObj::state_t::unavailable;
}

void listitemcontainerObj::implObj
::set_status_change_callback(const std::function<
			     list_item_status_change_callback_t
			     > &status_change_callback)
{
	(*listitem_shared_state_t::lock{listitem_sharedstate})
		->status_change_callback=status_change_callback;
}

std::function<list_item_status_change_callback_t>
listitemcontainerObj::implObj::get_status_change_callback() const
{
	return (*listitem_shared_state_t::lock{listitem_sharedstate})
		->status_change_callback;
}

ref<listitem_sharedstateObj> listitemcontainerObj::implObj::get_shared_state()
	const
{
	return *listitem_shared_state_t::lock{listitem_sharedstate};
}

void listitemcontainerObj::implObj
::set_shared_state(const ref<listitem_sharedstateObj> &s)
{
	*listitem_shared_state_t::lock{listitem_sharedstate}=s;
}

LIBCXXW_NAMESPACE_END
