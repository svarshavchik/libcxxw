/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menubar_hotspot_implobj_h
#define menubar_hotspot_implobj_h

#include "hotspot_bgcolor_elementfwd.H"
#include "container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/popup/popupfwd.H"
#include "popup/popup_showhide_elementfwd.H"
#include "x/w/impl/background_colorfwd.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN menubar_hotspot_implObj
	: public popup_showhide_elementObj<
	hotspot_bgcolor_elementObj<container_elementObj
				   <child_elementObj>>> {

	typedef popup_showhide_elementObj<
		hotspot_bgcolor_elementObj<container_elementObj
					   <child_elementObj>>> superclass_t;

 public:
	const popup menu_popup;

	menubar_hotspot_implObj(const popup &menu_popup,
				const background_color &bg_color,
				const background_color &highlighted_color,
				const background_color &clicked_color,
				const ref<containerObj::implObj> &container_impl
				);

	~menubar_hotspot_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
