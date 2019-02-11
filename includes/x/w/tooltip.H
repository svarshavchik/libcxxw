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

//! How a static tooltip, or a popup, gets attached to its element

//! Specifies how a static tooltip gets positioned with respect to its
//! element.
//!
//! below_or_above, above_or_below: the first preference if there's enough
//! room between the element and the edge of the screen, else the other side.
//! Furthermore, the popup's minimum width is set to be its element's width.
//! This is the style used by combo-box popups.
//!
//! right_or_left: to the right of the element, else to the left of it. This
//! is the style used by sub-menus.

enum class attached_to
	{
	 below_or_above, //!< Below, if there's room, else above.
	 above_or_below, //!< Above, if there's room, else below
	 right_or_left, //!< To the right, if there's room else to the left.

	 //! Tooltip displayed at pointer position.

	 //! Used internally by regular tooltips. Do not specify when creating
	 //! a static tooltip.

	 tooltip,
};

LIBCXXW_NAMESPACE_END

#endif
