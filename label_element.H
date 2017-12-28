/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_element_H
#define label_element_H

#include "label_elementfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
label_elementObj<element_impl>
::label_elementObj(const ref<containerObj::implObj> &parent_container,
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
::label_elementObj(const ref<containerObj::implObj> &parent_container,
		   halign alignment,
		   double initial_width,
		   richtextstring &&string,
		   const richtextmeta &default_meta,
		   bool allow_links,
		   const char *element_id)
	: element_impl(parent_container,
		       child_element_init_params{element_id}),
	  textlabelObj::implObj(alignment, initial_width,
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
void label_elementObj<element_impl>::initialize(IN_THREAD_ONLY)
{
	element_impl::initialize(IN_THREAD);
	textlabelObj::implObj::initialize(IN_THREAD);
}


template<typename element_impl>
void label_elementObj<element_impl>
::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	textlabelObj::implObj::theme_updated(IN_THREAD, new_theme);
	element_impl::theme_updated(IN_THREAD, new_theme);
}

template<typename element_impl>
void label_elementObj<element_impl>
::do_draw(IN_THREAD_ONLY, const draw_info &di, const rectangle_set &areas)
{
	textlabelObj::implObj::do_draw(IN_THREAD, di, areas);
}

template<typename element_impl>
void label_elementObj<element_impl>
::process_updated_position(IN_THREAD_ONLY)
{
	element_impl::process_updated_position(IN_THREAD);
	textlabelObj::implObj::process_updated_position(IN_THREAD);
}

template<typename element_impl>
void label_elementObj<element_impl>
::update_current_position(IN_THREAD_ONLY,
			  const rectangle &r)
{
	element_impl::update_current_position(IN_THREAD, r);
	textlabelObj::implObj::position_set(IN_THREAD);
}

template<typename element_impl>
bool label_elementObj<element_impl>
::process_button_event(IN_THREAD_ONLY,
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
::process_key_event(IN_THREAD_ONLY, const key_event &ke)
{
	return textlabelObj::implObj::process_key_event(IN_THREAD, ke)
		||
		element_impl::process_key_event(IN_THREAD, ke);
}

template<typename element_impl>
void label_elementObj<element_impl>
::report_motion_event(IN_THREAD_ONLY, const motion_event &me)
{
	element_impl::report_motion_event(IN_THREAD, me);
	textlabelObj::implObj::report_motion_event(IN_THREAD, me);
}

template<typename element_impl>
void label_elementObj<element_impl>
::pointer_focus(IN_THREAD_ONLY)
{
	element_impl::pointer_focus(IN_THREAD);
	textlabelObj::implObj::pointer_focus(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
