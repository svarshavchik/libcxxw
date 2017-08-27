/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef listitemcontainer_h
#define listitemcontainer_h

#include "listlayoutmanager/listitemcontainerfwd.H"
#include "listlayoutmanager/listitemcontainerobj.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! All elements that make up a single list item have this shared state.

struct LIBCXX_HIDDEN listitem_sharedstateObj : virtual public obj {

 public:
	//! This item's status change callback.

	std::function<list_item_status_change_callback_t
		      > status_change_callback;

	//! This list item is selected.

	bool selected_flag=false;

	//! This list item's state.

	enum class state_t {

		//! Enabled, everything is cool.

		enabled,

		//! Disabled.

		//! enabled() returns false.

		disabled,

		//! Unavailable.

		//! For our purposes this is same as disabled(), except
		//! that enabled() still returns true. This is used for
		//! list separator, so that they're drawn normally, but
		//! are treated as disabled in the list container.

		unavailable
	};

	state_t state=state_t::enabled;
};

LIBCXXW_NAMESPACE_END

#endif
