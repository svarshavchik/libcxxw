/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menu_impl_h
#define menu_impl_h

#include "x/w/menu.H"
#include "x/w/focus_border_appearancefwd.H"
#include "x/w/impl/nonrecursive_visibilityfwd.H"
#include "x/w/impl/focus/focusframecontainer_elementfwd.H"
#include "x/w/impl/popup/popupobj.H"
#include "x/w/impl/popup/popupfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN menuObj::implObj :
	public nonrecursive_visibilityObj<focusframecontainer_elementObj
					  <container_elementObj
					   <child_elementObj>>>
{
	typedef nonrecursive_visibilityObj<focusframecontainer_elementObj
					   <container_elementObj
					    <child_elementObj>>
					   > superclass_t;

	//! Whether the tabbing order of this new menu has been set.

	bool tabbing_order_set_thread_only=false;

 public:
	THREAD_DATA_ONLY(tabbing_order_set);

	//! The menu popup.
	const popup menu_popup;

	//! Constructor
	implObj(const popup &menu_popup,
		const const_focus_border_appearance &appearance,
		const container_impl &parent_container);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
