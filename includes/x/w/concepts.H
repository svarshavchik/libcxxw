/*
** Copyright 2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_concepts_h
#define x_w_concepts_h

#include <x/w/factoryfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/focusable_containerfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/main_windowfwd.H>
#include <utility>

LIBCXXW_NAMESPACE_START

//! A factory creator takes a factory as a parameter.

template<typename F>
concept factory_processor = requires(const factory &f, F c)
{
	{ c(f) };
};

//! A container creator takes a container as a parameter.

template<typename F>
concept container_processor = requires(const container &c, F cc)
{
	{ cc(c) };
};

//! A focusable_container creator takes a container as a parameter.

template<typename F>
concept focusable_container_processor = requires(const focusable_container &c,
						 F cc)
{
	{ cc(c) };
};

//! A listlayoutmanager creator takes a container as a parameter.

template<typename F>
concept listlayoutmanager_processor = requires(const listlayoutmanager &ll,
					       F lc)
{
	{ lc(ll) };
};

//! Concept of a main window creator function.

template<typename functor>
concept main_window_processor=requires(const main_window &mw, functor f)
{
	{ f(mw) };
};

LIBCXXW_NAMESPACE_END

#endif
