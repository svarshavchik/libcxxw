/*
** Copyright 2017-2021 Double Precision, Inc.
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

	//! Turned off in editable combo-boxes.
	const bool selection_changed_enabled;

	//! The items are accessed only while holding a lock.
	friend class standard_combobox_lock;
	friend class const_standard_combobox_lock;

	//! Constructor
	implObj(const ref<custom_combobox_containerObj::implObj>
		&container_impl,
		const new_custom_comboboxlayoutmanager &style,
		const standard_combobox_selection_changed_t &selection_changed,
		bool selection_changed_enabled);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the standard_comboboxlayoutmanager.

	layoutmanager create_public_object() override;

	//! Modify items before they get added to combo-box's dropdown list.

	//! Editable combo-box's input field needs to be sized wide enough
	//! for the largest combo-box list item, plus a space for the trailing
	//! cursor, so that the input field does not scroll when the cursor
	//! is after the last item in the combo-box.

	virtual void update_items_if_needed(std::vector<list_item_param>
					    &items);
};

LIBCXXW_NAMESPACE_END

#endif
