/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef standard_comboboxlayoutmanager_h
#define standard_comboboxlayoutmanager_h

#include "x/w/standard_comboboxlayoutmanager.H"
#include "combobox/custom_comboboxlayoutmanager.H"
#include "x/w/text_param.H"

#include <vector>

LIBCXXW_NAMESPACE_START

//! The standard combobox layout manager.

//! The container uses the grid layout manager to position the
//! combobox entry field and the dropdown button.

class LIBCXX_HIDDEN standard_comboboxlayoutmanagerObj::implObj
	: public custom_comboboxlayoutmanagerObj::implObj {

	//! The items. Accessed only while holding a list_lock

	std::vector<text_param> text_items_protected;

 public:

	//! The items are accessed only while holding a lock.
	auto inline &text_items(grid_map_t::lock &lock)
	{
		return text_items_protected;
	}

	//! Constructor
	implObj(const ref<custom_combobox_containerObj::implObj>
		&container_impl,
		const new_custom_comboboxlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the standard_comboboxlayoutmanager.

	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
