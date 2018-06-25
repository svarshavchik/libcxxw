/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_bordercontainer_element_h
#define x_w_impl_bordercontainer_element_h

#include <x/w/impl/bordercontainer_elementfwd.H>
#include <x/w/impl/current_border_impl.H>
#include <x/w/impl/themedim_element.H>
#include <x/w/impl/layoutmanager.H>
#include <x/w/grid_map_infofwd.H>

LIBCXXW_NAMESPACE_START

//! Constructor
template<typename element_impl>
template<typename ...Args>
bordercontainer_elementObj<element_impl>
::bordercontainer_elementObj(const current_border_impl &initial_border,
			     const dim_arg &hpad,
			     const dim_arg &vpad,
			     Args && ...args)
	: superclass_t{hpad, themedimaxis::width,
		       vpad, themedimaxis::height,
		       std::forward<Args>(args)...},
	  bordercontainer_implObj{this->child_container},
	  current_border_thread_only{initial_border}
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

//! Implement get_border().

//! Inherited from bordercontainer_implObj.

template<typename element_impl>
current_border_impl bordercontainer_elementObj<element_impl>
::get_border(ONLY IN_THREAD) const
{
	return this->current_border(IN_THREAD);
}

//! Implement set_border().

//! Inherited from bordercontainer_implObj.
//!
//! \note
//! Not
template<typename element_impl>
void bordercontainer_elementObj<element_impl>
::set_border(ONLY IN_THREAD, const current_border_impl &new_border)
{
	if (this->current_border(IN_THREAD)==new_border)
		return;

	this->current_border(IN_THREAD)=new_border;

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
