/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editable_comboboxlayoutmanager_impl_h
#define editable_comboboxlayoutmanager_impl_h

#include "x/w/editable_comboboxlayoutmanager.H"
#include "combobox/standard_comboboxlayoutmanager.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN editable_comboboxlayoutmanagerObj::implObj
	: public standard_comboboxlayoutmanagerObj::implObj {

 public:
	//! Constructor
	implObj(const ref<custom_combobox_containerObj::implObj>
		&container_impl,
		const new_editable_comboboxlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Current selection_changed callback

	mpobj<editable_combobox_selection_changed_t> selection_changed;

	//! Override create_public_object to create the editable_comboboxlayoutmanager.

	layoutmanager create_public_object() override;

	//! Override update_items_if_needed().
	void update_items_if_needed(std::vector<list_item_param> &items)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
