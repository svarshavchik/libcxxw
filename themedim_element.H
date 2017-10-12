/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedim_element_H
#define themedim_element_H

#include "themedim_elementfwd.H"
#include "themedim.H"
#include "element_screen.H"

LIBCXXW_NAMESPACE_START

template<typename tag>
inline themedim themedim_element<tag>::getref() { return themedim(this); }

template<typename tag>
inline const_themedim themedim_element<tag>::getref() const
{
	return const_themedim(this);
}

//! Destructor
template<typename tag>
themedim_element<tag>::~themedim_element()=default;

//! Constructor

//! The first parameter specifies the dimension's, the next either
//! get_theme_height/width_dim_t, and the remaining
//! parameters get forwarded to the superclass.

template<typename element_impl, typename tag>
template<typename ...Args>
themedim1_elementObj<element_impl, tag>
::themedim1_elementObj(const std::string &dimname,
		       theme_width_or_height width_or_height,
		       Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  themedim_element<tag>(dimname, this->get_screen()->impl,
				width_or_height)
{
}

//! Default destructor
template<typename element_impl, typename tag>
themedim1_elementObj<element_impl, tag>
::~themedim1_elementObj()=default;

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themedimObj::theme_updated().
//!
//! Addresses the potential race condition with the theme changing before
//! initialization.
template<typename element_impl, typename tag>
void themedim1_elementObj<element_impl, tag>
::initialize(IN_THREAD_ONLY)
{
	element_impl::initialize(IN_THREAD);
	themedim_element<tag>::initialize(IN_THREAD,
					  this->get_screen()->impl);
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themedimObj::theme_updated().

template<typename element_impl, typename tag>
void themedim1_elementObj<element_impl, tag>
::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	element_impl::theme_updated(IN_THREAD, new_theme);
	themedim_element<tag>::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
