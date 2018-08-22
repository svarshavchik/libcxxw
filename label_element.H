/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_element_H
#define label_element_H

#include "label_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
label_elementObj<element_impl>
::label_elementObj(const container_impl &parent_container,
		   const text_param &text,
		   halign alignment,
		   double initial_width,
		   bool allow_links)
	: element_impl(parent_container,
		       child_element_init_params{"label@libcxx.com"}),
	  textlabelObj::implObj(text, alignment, initial_width, allow_links,
				parent_container->container_element_impl())
{
}

template<typename element_impl>
label_elementObj<element_impl>
::label_elementObj(const container_impl &parent_container,
		   halign alignment,
		   double initial_width,
		   richtextstring &&string,
		   const richtextmeta &default_meta,
		   bool allow_links,
		   const char *element_id,
		   const color_arg &initial_background_color)
	: element_impl(parent_container,
		       child_element_init_params{element_id, {},
				       initial_background_color}),
	  textlabelObj::implObj(alignment, initial_width,
				parent_container->container_element_impl(),
				std::move(string), default_meta, allow_links)
{
}

template<typename element_impl>
label_elementObj<element_impl>::~label_elementObj()=default;

template<typename element_impl>
elementObj::implObj &label_elementObj<element_impl>::get_label_element_impl()
{
	return *this;
}

template<typename element_impl>
void label_elementObj<element_impl>::initialize(ONLY IN_THREAD)
{
	element_impl::initialize(IN_THREAD);
	textlabelObj::implObj::initialize(IN_THREAD);
}


template<typename element_impl>
void label_elementObj<element_impl>
::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	textlabelObj::implObj::theme_updated(IN_THREAD, new_theme);
	element_impl::theme_updated(IN_THREAD, new_theme);
}

template<typename element_impl>
void label_elementObj<element_impl>
::do_draw(ONLY IN_THREAD, const draw_info &di, const rectangle_set &areas)
{
	textlabelObj::implObj::do_draw(IN_THREAD, di, areas);
}

template<typename element_impl>
void label_elementObj<element_impl>
::process_updated_position(ONLY IN_THREAD)
{
	element_impl::process_updated_position(IN_THREAD);
	textlabelObj::implObj::process_updated_position(IN_THREAD);
}

template<typename element_impl>
bool label_elementObj<element_impl>
::process_button_event(ONLY IN_THREAD,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	return textlabelObj::implObj::process_button_event(IN_THREAD, be,
							   timestamp)
		||
		element_impl::process_button_event(IN_THREAD, be, timestamp);
}

template<typename element_impl>
bool label_elementObj<element_impl>
::process_key_event(ONLY IN_THREAD, const key_event &ke)
{
	return textlabelObj::implObj::process_key_event(IN_THREAD, ke)
		||
		element_impl::process_key_event(IN_THREAD, ke);
}

template<typename element_impl>
void label_elementObj<element_impl>
::report_motion_event(ONLY IN_THREAD, const motion_event &me)
{
	element_impl::report_motion_event(IN_THREAD, me);
	textlabelObj::implObj::report_motion_event(IN_THREAD, me);
}

template<typename element_impl>
void label_elementObj<element_impl>
::pointer_focus(ONLY IN_THREAD,
		const callback_trigger_t &trigger)
{
	element_impl::pointer_focus(IN_THREAD, trigger);
	textlabelObj::implObj::pointer_focus(IN_THREAD, trigger);
}

LIBCXXW_NAMESPACE_END

#endif
