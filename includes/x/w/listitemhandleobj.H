/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listitemhandleobj_h
#define x_w_listitemhandleobj_h

#include <x/w/listitemhandlefwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see listitemhandle

class listitemhandleObj: virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Constructor
	listitemhandleObj() LIBCXX_HIDDEN;

	//! Destructor
	~listitemhandleObj() LIBCXX_HIDDEN;

	//! Whether this list item is enabled.
	virtual bool enabled() const LIBCXX_HIDDEN =0;

	//! Enable or disable this list item.
	virtual void enabled(bool) LIBCXX_HIDDEN =0;

	//! Return the menu item's layout manager.
	virtual listlayoutmanagerptr submenu_listlayout()
		const LIBCXX_HIDDEN =0;

	//! Enable or disable this list item.
	virtual void enabled(ONLY IN_THREAD, bool) LIBCXX_HIDDEN =0;

	//! Install a new status update callback.
	virtual void on_status_update(const
				      list_item_status_change_callback &)
		LIBCXX_HIDDEN =0;

	//! Install a new status update callback.
	virtual void on_status_update(ONLY IN_THREAD,
				      const
				      list_item_status_change_callback &)
		LIBCXX_HIDDEN =0;
};

LIBCXXW_NAMESPACE_END

#endif
