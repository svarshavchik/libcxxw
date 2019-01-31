/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_h
#define x_w_tooltip_h

#include <x/w/tooltipfwd.H>
#include <x/w/container.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! A factory that gets passed to the tooltip creator.

class tooltip_factory {

public:

	//! The tooltip creator's callback.

	//! The tooltip creator calls this object, passing the
	//! content creator and the layout manager for the tooltip.

	template<typename functor>
	void operator()(functor &&creator,
			const new_layoutmanager &layout_manager) const
	{
		create(make_function<void (const container &)>
		       (std::forward<functor>(creator)),
		       layout_manager);
	}

	//! Create the tooltip

	virtual void create(const function<void (const container &)> &creator,
			    const new_layoutmanager &layout_manager) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
