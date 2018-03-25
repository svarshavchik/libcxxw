/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pagetab_impl_h
#define pagetab_impl_h

#include "booklayoutmanager/pagetab.H"
#include "booklayoutmanager/pagetabgridcontainer_implfwd.H"
#include "always_visiblefwd.H"
#include "hotspot_bgcolor_elementfwd.H"
#include "container_elementfwd.H"
#include "child_element.H"
#include "x/w/elementfwd.H"

LIBCXXW_NAMESPACE_START

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
//! - alwaysvisibleObj - this element is always visible.
//!
//! - hotspot_bgcolorObj - a hotspot, highlighted with a background color.
//!
//! The cold colors for the hotspot come from the pagetabgridcontainer_impl.
//!
//! This container uses the singleton layout manager for the actual element
//! that comprises the tab's label.

class LIBCXX_HIDDEN pagetabObj::implObj : public always_visibleObj<
	hotspot_bgcolor_elementObj<container_elementObj<child_elementObj>>> {

	//! Alias.

	typedef always_visibleObj<
		hotspot_bgcolor_elementObj<container_elementObj
					   <child_elementObj>>> superclass_t;

 public:

	//! The grandparent.

	const pagetabgridcontainer_impl my_pagetabgridcontainer_impl;

	//! Constructor

	implObj(const ref<containerObj::implObj> &parent_container,
		const pagetabgridcontainer_impl &my_pagetabgridcontainer_impl,
		const color_arg &warm_color,
		const color_arg &hot_color);

	//! Destructor
	~implObj();

	//! Override label_theme_font.

	//! Default label font is "book_tab_font".

	const char *label_theme_font() const override;

	//! Set the inactive or the active background color.

	//! If it is active, ensure_entire_visibility().

	void set_active(ONLY IN_THREAD, bool);

	//! Get the element.

	elementptr get();
};

LIBCXXW_NAMESPACE_END

#endif
