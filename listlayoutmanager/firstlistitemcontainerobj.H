/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef firstlistitemcontainerobj_h
#define firstlistitemcontainerobj_h

#include "listlayoutmanager/firstlistitemcontainerfwd.H"
#include "listlayoutmanager/listitemcontainerobj.H"
#include <functional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see firstlistitemcontainer

class LIBCXX_HIDDEN firstlistitemcontainerObj : public listitemcontainerObj {

public:

	//! Installed callback.
	const std::function<list_item_status_change_callback_t
			    > status_change_callback;

	//! Constructor
	firstlistitemcontainerObj(const ref<implObj> &impl,
				  const ref<listitemlayoutmanagerObj::implObj>
				  &l,
				  const std::function<
				  list_item_status_change_callback_t
				  > &status_change_callback);

	//! Destructor
	~firstlistitemcontainerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
