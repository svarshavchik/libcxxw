/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedim_element_H
#define x_w_impl_themedim_element_H

#include <x/w/impl/themedim_elementfwd.H>
#include <x/w/impl/themedim.H>
#include <x/w/impl/get_window_handler.H>
#include <x/refptr_traits.H>
#include <type_traits>

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


template<typename ...args>
template<typename ...TupleTypes, size_t ...indexes>
themedims_elements<args...>
::themedims_elements(std::index_sequence<indexes...>,
		     const ref<screenObj::implObj> &screen_impl,
		     const std::tuple<TupleTypes...> &dimArgs)
	: themedim_element<args>{std::get<0>(std::get<indexes>(dimArgs)),
		std::get<1>(std::get<indexes>(dimArgs)),
		screen_impl}...
	{
	}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themedimObj::theme_updated().
//!
//! Addresses the potential race condition with the theme changing before
//! initialization.

template<typename base, typename ...args>
void themedims_elementObj<base, args...>::initialize(ONLY IN_THREAD)
{
	auto s=this->get_screen();

	(themedims_elements<args...>::template themedim_element<args>
	 ::initialize(IN_THREAD, s->impl), ...);

	base::initialize(IN_THREAD);
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the themedimObj::theme_updated().

template<typename base, typename ...args>
void themedims_elementObj<base, args...>
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	(themedims_elements<args...>::template themedim_element<args>
	 ::theme_updated(IN_THREAD, new_theme), ...);

	base::theme_updated(IN_THREAD, new_theme);
}

template<typename base, typename ...args>
template<typename ...TupleTypes, typename ...ExtraArgs>
themedims_elementObj<base, args...>
::themedims_elementObj(const std::tuple<TupleTypes...> &dimArgs,
		       ExtraArgs && ...Args)
	: base{std::forward<ExtraArgs>(Args)...},
	  themedims_elements<args...>{std::index_sequence_for<TupleTypes...>{},
		this->get_screen()->impl, dimArgs}
{
}

template<typename base, typename ...args>
themedims_elementObj<base, args...>
::~themedims_elementObj()=default;

LIBCXXW_NAMESPACE_END

#endif
