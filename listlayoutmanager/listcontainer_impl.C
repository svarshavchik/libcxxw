/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "hotspot_element.H"
#include "container_element.H"
#include "background_color_element.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/rgb.H"
#include "messages.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

listcontainerObj::implObj::implObj(const ref<containerObj::implObj> &parent,
				   const new_listlayoutmanager &style)
	: listcontainer_impl_superclass_t
	  (

	   // Initialize the background colors
	   parent->get_element_impl()
	   .create_background_color(style.selected_color),
	   parent->get_element_impl()
	   .create_background_color(style.highlighted_color),
	   parent->get_element_impl()
	   .create_background_color(style.current_color),
	   parent)
{
	if (style.columns < 1)
		throw EXCEPTION(_("Cannot create a list with 0 columns"));
}

listcontainerObj::implObj::~implObj()=default;


ref<containerObj::implObj>
listcontainerObj::implObj::parent_for_new_child(const ref<containerObj::implObj>
						&me)
{
	child_element_init_params init_params;

	init_params.container_override=true;

	return ref<listitemcontainerObj::implObj>
		::create(ref<implObj>(this), init_params);
}

void listcontainerObj::implObj
::pointer_focus(IN_THREAD_ONLY, const ref<elementObj::implObj> &e)
{
	invoke_layoutmanager([&]
			     (const ref<listlayoutmanagerObj::implObj> &lilm)
			     {
				     lilm->pointer_focus(IN_THREAD, e);
			     });
}

void listcontainerObj::implObj::temperature_changed(IN_THREAD_ONLY)
{
	invoke_layoutmanager([&]
			     (const ref<listlayoutmanagerObj::implObj> &lilm)
			     {
				     lilm->temperature_changed(IN_THREAD);
			     });
}

void listcontainerObj::implObj::activated(IN_THREAD_ONLY)
{
	listcontainer_impl_superclass_t::activated(IN_THREAD);

	listlayoutmanagerptr llmptr;

	invoke_layoutmanager([&]
			     (const auto &layout_impl)
			     {
				     llmptr=layout_impl->create_public_object();
			     });

	if (!llmptr)
		return; // Being destroyed, probably.

	listlayoutmanager llm=llmptr;

	llm->impl->activated(IN_THREAD, llm);
}

bool listcontainerObj::implObj::process_key_event(IN_THREAD_ONLY,
						  const key_event &ke)
{
	bool processed=false;

	invoke_layoutmanager([&]
			     (const ref<listlayoutmanagerObj::implObj>
			      &layout_impl)
			     {
				     processed=layout_impl->process_key_event
					     (IN_THREAD, ke);
			     });

	if (!processed)
		processed=listcontainer_impl_superclass_t::process_key_event
			(IN_THREAD, ke);
	return processed;
}

void listcontainerObj::implObj::set_focus_and_ensure_visibility(IN_THREAD_ONLY)
{
	// pointer_focus() will request visibility only for the list item
	// that's currently highlighted. Do not request visibility for this
	// entire container. If this is a selection list element, which uses
	// a peephole, this will scroll to the top of the list, and override
	// this.

	set_focus_only(IN_THREAD);
}

void listcontainerObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	invoke_layoutmanager([&, this]
			     (const ref<listlayoutmanagerObj::implObj>
			      &layout_impl)
			     {
				     layout_impl->keyboard_focus
					     (IN_THREAD,
					      current_keyboard_focus(IN_THREAD))
					     ;
			     });
}
LIBCXXW_NAMESPACE_END
