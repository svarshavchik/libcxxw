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
		       child_element_init_params{"label@libcxx"}),
	  textlabelObj::implObj(text, alignment, initial_width, allow_links,
				parent_container->get_element_impl())
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
::set_inherited_visibility(IN_THREAD_ONLY,
			   inherited_visibility_info &visibility_info)
{
	element_impl::set_inherited_visibility(IN_THREAD, visibility_info);
	textlabelObj::implObj::set_inherited_visibility(IN_THREAD,
							visibility_info);
}

LIBCXXW_NAMESPACE_END

#endif
