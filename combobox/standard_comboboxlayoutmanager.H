/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef standard_comboboxlayoutmanager_h
#define standard_comboboxlayoutmanager_h

#include "x/w/standard_comboboxlayoutmanager.H"
#include "combobox/custom_comboboxlayoutmanager.H"
#include "x/w/text_param.H"

#include <x/mpobj.H>
#include <vector>
#include <string>

LIBCXXW_NAMESPACE_START

//! The standard combobox layout manager.

//! The container uses the grid layout manager to position the
//! combobox entry field and the dropdown button.

class LIBCXX_HIDDEN standard_comboboxlayoutmanagerObj::implObj
	: public custom_comboboxlayoutmanagerObj::implObj {

	//! The items. Accessed only while holding a list_lock

	std::vector<text_param> text_items_protected;

 public:

	//! Current selection changed callback.
	mpobj<standard_combobox_selection_changed_t> selection_changed;

	//! The items are accessed only while holding a lock.
	friend class standard_combobox_lock;
	friend class const_standard_combobox_lock;

	//! Constructor
	implObj(const ref<custom_combobox_containerObj::implObj>
		&container_impl,
		const new_custom_comboboxlayoutmanager &style,
		const standard_combobox_selection_changed_t &selection_changed);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the standard_comboboxlayoutmanager.

	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
