/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_comboboxlayoutmanager_h
#define custom_comboboxlayoutmanager_h

#include "x/w/custom_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_containerobj.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START


//! Arguments to custom_comboboxlayoutmanagerObj::implObj's constructor

struct LIBCXX_HIDDEN new_custom_comboboxlayoutmanager::create_impl_info {

	const ref<custom_combobox_containerObj::implObj> &container_impl;
};

//! The custom combobox layout manager.

//! The container uses the grid layout manager to position the
//! combobox entry field and the dropdown button.

class LIBCXX_HIDDEN custom_comboboxlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

 public:
	//! Parent container

	const ref<custom_combobox_containerObj::implObj> container_impl;

	//! Current selection callback.
	mpobj<custom_combobox_selection_changed_t> selection_changed;

	//! Constructor
	implObj(const ref<custom_combobox_containerObj::implObj>
		&container_impl,
		const new_custom_comboboxlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the custom_comboboxlayoutmanager.

	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
