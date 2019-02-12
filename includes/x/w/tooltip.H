/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_h
#define x_w_tooltip_h

#include <x/w/tooltipfwd.H>
#include <x/w/container.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Configuration for a tooltip border

//! A tooltip border, and inner padding

struct tooltip_border {

	border_arg border;
	dim_arg hpad;
	dim_arg vpad;

	//! Initialize border, hpad, and vpad, using theme naming convention.

	//! Set "border" to the parent, and the paddings to
	//! "border_padding_h" and "border_padding_v";

	void set_theme_border(const std::string &border);
};

//! A factory that gets passed to the tooltip creator.

class tooltip_factory {

public:

	//! Border for the new tooltip

	//! Used when the display has an alpha channel.
	//! Typically a slightly rounded border.

	static tooltip_border default_alpha_border();

	//! Border for the new tooltip

	//! Used when the display does not have an alpha channel.
	//! Typically a square border.

	static tooltip_border default_nonalpha_border();

	//! The tooltip creator's callback.

	//! The tooltip creator calls this object, passing the
	//! content creator and the layout manager for the tooltip.
	//!
	//! Two optional parameters specify a custom tooltip border.
	//! The first parameter specifies the border for displays with
	//! an alpha channel, and the second parameter specifies the border
	//! for displays without an alpha channel.

	template<typename functor>
	void operator()(functor &&creator,
			const new_layoutmanager &layout_manager,
			const tooltip_border &alpha_border=
			default_alpha_border(),
			const tooltip_border &nonalpha_border=
			default_nonalpha_border()) const
	{
		create(make_function<void (const container &)>
		       (std::forward<functor>(creator)),
		       layout_manager, alpha_border, nonalpha_border);
	}

	//! Create the tooltip

	virtual void create(const function<void (const container &)> &creator,
			    const new_layoutmanager &layout_manager,
			    const tooltip_border &alpha_border,
			    const tooltip_border &nonalpha_border) const=0;
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

//! Optional settings for a static tooltip.

//! \see element

struct static_tooltip_config {

	//! Static tooltip's position
	attached_to affinity=attached_to::right_or_left;

	//! Tooltip's border on displays with an alpha channel.

	//! Used when the display has an alpha channel.
	//! Typically a slightly rounded border.

	tooltip_border alpha_border=
		tooltip_factory::default_alpha_border();

	//! Tooltip's border on displays without an alpha channel

	//! Used when the display does not have an alpha channel.
	//! Typically a square border.
	tooltip_border nonalpha_border=
		tooltip_factory::default_nonalpha_border();

	//! Tooltip's background color.
	color_arg background_color="static_tooltip_background_color";

	~static_tooltip_config();
};

LIBCXXW_NAMESPACE_END

#endif
