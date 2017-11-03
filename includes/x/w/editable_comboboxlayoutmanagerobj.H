/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editable_comboboxlayoutmanagerobj_h
#define x_w_editable_comboboxlayoutmanagerobj_h

#include <x/w/editable_comboboxlayoutmanagerfwd.H>
#include <x/w/standard_comboboxlayoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see editable_comboboxlayoutmanager

class editable_comboboxlayoutmanagerObj :
	public standard_comboboxlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	editable_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					  const ref<textlistlayoutmanagerObj
					  ::implObj>
					  &list_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~editable_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
