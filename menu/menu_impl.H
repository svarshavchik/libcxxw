/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menu_impl_h
#define menu_impl_h

#include "x/w/menu.H"
#include "nonrecursive_visibilityfwd.H"
#include "focus/focusframecontainer_elementfwd.H"
#include "popup/popup_attachedto_handler_elementfwd.H"
#include "container_elementfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN menuObj::implObj :
	public nonrecursive_visibilityObj<popup_attachedto_handler_elementObj<
						  focusframecontainer_elementObj
						  <container_elementObj
						   <child_elementObj>>>>
{
	typedef nonrecursive_visibilityObj<popup_attachedto_handler_elementObj
					   <focusframecontainer_elementObj
					    <container_elementObj
					     <child_elementObj>>>
					   > superclass_t;

	//! Whether the tabbing order of this new menu has been set.

	bool tabbing_order_set_thread_only=false;

 public:
	THREAD_DATA_ONLY(tabbing_order_set);

	//! Constructor
	implObj(const ref<popup_attachedto_handlerObj> &attachedto_handler,
		const ref<containerObj::implObj> &parent_container);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
