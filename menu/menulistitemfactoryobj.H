/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menulistitemfactoryobj_h
#define menulistitemfactoryobj_h

#include "listlayoutmanager/listitemfactoryobj.H"
#include "x/w/menulayoutmanager.H"

#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Subclass of listitemfactoryObj used by menu popups.

//! Menu popups are two-column lists. The second column shows an optional
//! shortcut, or a sub-menu indicator.
//!
//! Intercepts the call to created_element, the new menu item, then also
//! creates the second element, for the completed list item.

class LIBCXX_HIDDEN menulistitemfactoryObj : public listitemfactoryObj {

	//! Container for the next menu item's type.

	typedef mpobj<menuitem_type_t> next_menuitem_type_t;

	//! Next menu item's type.

	next_menuitem_type_t next_menuitem_type;
public:

	using listitemfactoryObj::listitemfactoryObj;

	//! Destructor

	~menulistitemfactoryObj();

	//! Set the type for the next menu item.
	void set_type(const menuitem_type_t &);

	//! Override created_element().

	//! The various listlayoutmanagerObj::implObj mixins (append_factoryObj
	//! and others) end up calling this instead.

	void created_element(const element &e);
};

LIBCXXW_NAMESPACE_END

#endif
