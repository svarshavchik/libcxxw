/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menubar_hotspot_implobj_h
#define menubar_hotspot_implobj_h

#include "hotspot_bgcolor_elementfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/popup/popupfwd.H"
#include "popup/popup_showhide_elementfwd.H"
#include "x/w/impl/background_colorfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object for a menubar button that opens a popup

class LIBCXX_HIDDEN menubar_hotspot_implObj
	: public popup_showhide_elementObj<
	hotspot_bgcolor_elementObj<container_elementObj
				   <child_elementObj>>> {

	typedef popup_showhide_elementObj<
		hotspot_bgcolor_elementObj<container_elementObj
					   <child_elementObj>>> superclass_t;

 public:
	//! My popup
	const popup menu_popup;

	//! Constructor
	menubar_hotspot_implObj(const popup &menu_popup,
				const popup_attachedto_handler &menu_popup_handler,
				const background_color &bg_color,
				const background_color &highlighted_color,
				const background_color &clicked_color,
				const container_impl &container_impl
				);
	//! Destructor
	~menubar_hotspot_implObj();

	//! Override focus_autorestorable()

	//! Ignore menubar buttons when changing focus, for the purposes of
	//! restoring input focus to the previous element, when closing
	//! a menu popup.

	bool focus_autorestorable(ONLY IN_THREAD) override;
};

LIBCXXW_NAMESPACE_END

#endif
