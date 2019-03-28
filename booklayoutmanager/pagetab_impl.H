/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pagetab_impl_h
#define pagetab_impl_h

#include "booklayoutmanager/pagetab.H"
#include "booklayoutmanager/pagetabgridcontainer_implfwd.H"
#include "x/w/impl/always_visiblefwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/impl/themedim_elementfwd.H"
#include "hotspot_bgcolor_elementfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/bookpage_appearancefwd.H"
#include "x/w/elementfwd.H"
#include "x/w/font_arg.H"
#include "x/w/rgb.H"

LIBCXXW_NAMESPACE_START

struct h_padding_tag;
struct v_padding_tag;
struct current_color_tag;
struct notcurrent_color_tag;

//! Internal implementation object.

//! This is a color hotspot container with the pagetabsingleton layout manager,
//! containing a single element that defines the page's tab.
//!
//! The pagetabgridcontainer_impl is a grid with one row. Each grid element
//! is a container that uses the stock gridlayoutmanager, with one element.
//! This internal grid element is used to draw the borders around each tab
//! element.
//!
//! The internal grid container's element is this element, which consists
//! of:
//!
//! - always_visibleObj - this element is always visible.
//!
//! - themedim_elementObj - the padding for this tab.
//!
//! - background_color_elementObj - the active and inactive "cold" colors
//!   for the individual hotspots that make up the tab.
//!
//! - hotspot_bgcolorObj - a hotspot, highlighted with a background color.
//!
//! This container uses the singleton layout manager for the actual element
//! that comprises the tab's label.

class LIBCXX_HIDDEN pagetabObj::implObj
	: public always_visibleObj<themedim_elementObj<
					   background_color_elementObj<
						   hotspot_bgcolor_elementObj
						   <container_elementObj
						    <child_elementObj>>,
						   current_color_tag,
						   notcurrent_color_tag>,
					   h_padding_tag, v_padding_tag>>
{

	//! Alias.

	typedef always_visibleObj<
		themedim_elementObj<
			background_color_elementObj<
				hotspot_bgcolor_elementObj<container_elementObj
							   <child_elementObj>>,
				current_color_tag, notcurrent_color_tag>,
			h_padding_tag, v_padding_tag>> superclass_t;

 public:

	//! The grandparent.

	const pagetabgridcontainer_impl my_pagetabgridcontainer_impl;

	//! Default font
	const font_arg tab_font;

	//! Default font color
	const color_arg tab_font_color;

	//! Constructor

	implObj(const container_impl &parent_container,
		const pagetabgridcontainer_impl &my_pagetabgridcontainer_impl,
		const const_bookpage_appearance &appearance);

	//! Destructor
	~implObj();

	//! Override label_theme_font.

	font_arg label_theme_font() const override;

	//! Override label_theme_color.

	color_arg label_theme_color() const override;

	//! Set the inactive or the active background color.

	//! If it is active, ensure_entire_visibility().

	void set_active(ONLY IN_THREAD, bool);

	//! Get the element.

	elementptr get();
};

LIBCXXW_NAMESPACE_END

#endif
