/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_bordercontainer_element_h
#define x_w_impl_bordercontainer_element_h

#include <x/w/impl/bordercontainer_elementfwd.H>
#include <x/w/impl/current_border_impl.H>
#include <x/w/impl/themedim_element.H>
#include <x/w/impl/themeborder_element.H>
#include <x/w/impl/layoutmanager.H>
#include <x/w/impl/child_element.H>
#include <x/w/layoutmanager.H>
#include <x/w/grid_map_infofwd.H>

LIBCXXW_NAMESPACE_START

//! Constructor
template<typename element_impl>
template<typename ...Args>
bordercontainer_elementObj<element_impl>
::bordercontainer_elementObj(generic_windowObj::handlerObj &my_window,
			     const border_arg &left_border,
			     const border_arg &right_border,
			     const border_arg &top_border,
			     const border_arg &bottom_border,
			     const richtextptr &title,
			     const dim_arg &title_indent,
			     const dim_arg &hpad,
			     const dim_arg &vpad,
			     Args && ...args)
	: superclass_t{left_border, right_border,
		       top_border, bottom_border,
		       title_indent, themedimaxis::width,
		       hpad, themedimaxis::width,
		       vpad, themedimaxis::height,
		       std::forward<Args>(args)...},
	  bordercontainer_implObj{my_window},
	  title_thread_only{title}
{
}

//! Constructor
template<typename element_impl>
template<typename ...Args>
bordercontainer_elementObj<element_impl>
::bordercontainer_elementObj(child_elementObj &parent_element,
			     Args && ...args)
	: bordercontainer_elementObj{parent_element.get_window_handler(),
				     std::forward<Args>(args)...}
{
}

//! Destructor
template<typename element_impl>
bordercontainer_elementObj<element_impl>::~bordercontainer_elementObj()
=default;

//! Implement get_container_impl().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
containerObj::implObj &bordercontainer_elementObj<element_impl>
::get_container_impl()
{
	return *this;
}

//! Implement get_container_impl().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
const containerObj::implObj &bordercontainer_elementObj<element_impl>
::get_container_impl() const
{
	return *this;
}

//! Implement get_left_border().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
current_border_impl bordercontainer_elementObj<element_impl>
::get_left_border(ONLY IN_THREAD) const
{
	return this->themeborder_element_impl<left_border_tag>::current_border(IN_THREAD);
}

//! Implement get_right_border().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
current_border_impl bordercontainer_elementObj<element_impl>
::get_right_border(ONLY IN_THREAD) const
{
	return this->themeborder_element_impl<right_border_tag>::current_border(IN_THREAD);
}

//! Implement get_top_border().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
current_border_impl bordercontainer_elementObj<element_impl>
::get_top_border(ONLY IN_THREAD) const
{
	return this->themeborder_element_impl<top_border_tag>::current_border(IN_THREAD);
}

//! Implement get_bottom_border().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
current_border_impl bordercontainer_elementObj<element_impl>
::get_bottom_border(ONLY IN_THREAD) const
{
	return this->themeborder_element_impl<bottom_border_tag>::current_border(IN_THREAD);
}

//! Implement get_title().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
richtextptr bordercontainer_elementObj<element_impl>::get_title(ONLY IN_THREAD)
	const
{
	return this->title(IN_THREAD);
}

//! Implement set_title().

//! Inherited from bordercontainer_implObj.
template<typename element_impl> void bordercontainer_elementObj<element_impl>
::set_title(ONLY IN_THREAD,
	    const layoutmanager &layout,
	    const richtextptr &t)
{
	this->title(IN_THREAD)=t;
	layout->set_modified();
}


//! Implement do_set_border().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
bool bordercontainer_elementObj<element_impl>
::do_set_border(ONLY IN_THREAD,
		const border_arg &new_left_border,
		const border_arg &new_right_border,
		const border_arg &new_top_border,
		const border_arg &new_bottom_border)
{
	// First, save the existing borders.

	auto prev_left_border=
		themeborder_element_impl<left_border_tag>::current_border(IN_THREAD);
	auto prev_right_border=
		themeborder_element_impl<right_border_tag>::current_border(IN_THREAD);
	auto prev_top_border=
		themeborder_element_impl<top_border_tag>::current_border(IN_THREAD);
	auto prev_bottom_border=
		themeborder_element_impl<bottom_border_tag>::current_border(IN_THREAD);

	this->themeborder_element_impl<left_border_tag>::set_new_border(IN_THREAD,
									new_left_border);
	this->themeborder_element_impl<right_border_tag>::set_new_border(IN_THREAD,
									 new_right_border);
	this->themeborder_element_impl<top_border_tag>::set_new_border(IN_THREAD,
								       new_top_border);
	this->themeborder_element_impl<bottom_border_tag>::set_new_border(IN_THREAD,
									  new_bottom_border);

	// If the borders haven't really changed we don't need to do anything.
	if (prev_left_border==
	    themeborder_element_impl<left_border_tag>::current_border(IN_THREAD) &&
	    prev_right_border==
	    themeborder_element_impl<right_border_tag>::current_border(IN_THREAD) &&
	    prev_top_border==
	    themeborder_element_impl<top_border_tag>::current_border(IN_THREAD) &&
	    prev_bottom_border==
	    themeborder_element_impl<bottom_border_tag>::current_border(IN_THREAD))
		return false;

	return true;
}

//! Implement get_title_indent()

template<typename element_impl>
dim_t bordercontainer_elementObj<element_impl>
::get_title_indent(ONLY IN_THREAD) const
{
	return this->themedim_element<titleindent_tag>::pixels(IN_THREAD);
}

//! Implement get_border_hpad

template<typename element_impl>
dim_t bordercontainer_elementObj<element_impl>
::get_border_hpad(ONLY IN_THREAD) const
{
	return this->themedim_element<borderhpad_tag>::pixels(IN_THREAD);
}

//! Implement get_border_hpad

template<typename element_impl>
dim_t bordercontainer_elementObj<element_impl>
::get_border_vpad(ONLY IN_THREAD) const
{
	return this->themedim_element<bordervpad_tag>::pixels(IN_THREAD);
}

//! Override initialize().

//! If there's a title, initialize it, to make sure.
template<typename element_impl>
void bordercontainer_elementObj<element_impl>::initialize(ONLY IN_THREAD)
{
	bordercontainer_implObj::initialize(IN_THREAD);

	superclass_t::initialize(IN_THREAD);
}

//! Override initialize().

//! If there's a title, forward the call there.

template<typename element_impl>
void bordercontainer_elementObj<element_impl>
::theme_updated(ONLY IN_THREAD,
		const const_defaulttheme &new_theme)
{
	if (this->title(IN_THREAD))
		this->title(IN_THREAD)->theme_updated(IN_THREAD, new_theme);

	superclass_t::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
