/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_h
#define x_w_tooltip_h

#include <x/w/tooltipfwd.H>
#include <x/w/container.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/tooltip_appearancefwd.H>
#include <x/w/concepts.H>
#include <x/functional.H>
#include <x/optional_args.H>

LIBCXXW_NAMESPACE_START

/*! A factory used by a custom tooltip creator.

A \ref element "widget"'s create_custom_tooltip() arranges for its
callback to get invoked and receive this factory for creating the actual
tooltip.
*/

class tooltip_factory {

public:

	//! Constructor
	tooltip_factory();

	//! Constructor
	~tooltip_factory();

	//! Appearance
	const_tooltip_appearance appearance;

	//! The tooltip creator's callback.

	//! The tooltip creator calls this object, passing the
	//! content creator and the layout manager for the tooltip.

	template<container_processor functor>
	inline void operator()(functor &&creator,
			const new_layoutmanager &layout_manager)
		const
	{
		create(make_function<void (const container &)>
		       (std::forward<functor>(creator)), layout_manager);
	}

	//! Create the tooltip

	virtual void create(const function<void (const container &)> &creator,
			    const new_layoutmanager &layout_manager)
		const LIBCXX_PUBLIC=0;
};

//! How a static tooltip, or a popup, gets attached to its widget

//! Specifies how a static tooltip gets positioned with respect to its
//! widget.
//!
//! below_or_above, above_or_below: the first preference if there's enough
//! room between the widget and the edge of the screen, else the other side.
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

//! Optional settings for a static tooltip.

//! \see element

struct static_tooltip_config {

	//! Appearance
	const_tooltip_appearance appearance;

	//! Static tooltip's position
	attached_to affinity=attached_to::right_or_left;

	//! Constructor
	static_tooltip_config();

	//! Destructor
	~static_tooltip_config();

	//! Copy constructor
	static_tooltip_config(const static_tooltip_config &);

	//! Assignment operator
	static_tooltip_config &operator=(const static_tooltip_config &);
};

LIBCXXW_NAMESPACE_END

#endif
