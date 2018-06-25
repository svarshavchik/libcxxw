/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_bordercontainer_element_h
#define x_w_impl_bordercontainer_element_h

#include <x/w/impl/bordercontainer_elementfwd.H>
#include <x/w/impl/current_border_impl.H>
#include <x/w/impl/themedim_element.H>
#include <x/w/impl/themeborder_element.H>
#include <x/w/impl/layoutmanager.H>
#include <x/w/grid_map_infofwd.H>

LIBCXXW_NAMESPACE_START

//! Constructor
template<typename element_impl>
template<typename ...Args>
bordercontainer_elementObj<element_impl>
::bordercontainer_elementObj(const border_arg &left_border,
			     const border_arg &right_border,
			     const border_arg &top_border,
			     const border_arg &bottom_border,
			     const dim_arg &hpad,
			     const dim_arg &vpad,
			     Args && ...args)
	: superclass_t{left_border, right_border,
		       top_border, bottom_border,
		       hpad, themedimaxis::width,
		       vpad, themedimaxis::height,
		       std::forward<Args>(args)...},
	  bordercontainer_implObj{this->child_container}
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

//! Implement set_border().

//! Inherited from bordercontainer_implObj.
//!
//! \note
//! Not
template<typename element_impl>
void bordercontainer_elementObj<element_impl>
::set_border(ONLY IN_THREAD,
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
		return;

	// Make sure that recalculate() gets called to clear the cached border info.

	this->get_container_impl().invoke_layoutmanager
		([&]
		 (const auto &manager)
		 {
			 manager->needs_recalculation(IN_THREAD);
		 });
}

template<typename element_impl>
dim_t bordercontainer_elementObj<element_impl>
::get_border_hpad(ONLY IN_THREAD) const
{
	return this->themedim_element<borderhpad_tag>::pixels(IN_THREAD);
}

template<typename element_impl>
dim_t bordercontainer_elementObj<element_impl>
::get_border_vpad(ONLY IN_THREAD) const
{
	return this->themedim_element<bordervpad_tag>::pixels(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
