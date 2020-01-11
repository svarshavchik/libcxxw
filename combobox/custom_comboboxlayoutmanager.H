/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_comboboxlayoutmanager_h
#define custom_comboboxlayoutmanager_h

#include "x/w/custom_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_containerobj.H"
#include "combobox/custom_combobox_popup_container.H"
#include "popup/popup_attachedto_info.H"
#include "gridlayoutmanager.H"
#include "peepholed_toplevel_listcontainer/create_popupfwd.H"

LIBCXXW_NAMESPACE_START

//! Create a new_listlayoutmanager for a combo-box dropdown list.

new_listlayoutmanager
combobox_new_listlayoutmanager(bool selection_required,
			       const const_list_appearance &) LIBCXX_HIDDEN;

//! Create parameters for create_peepholed_toplevel_listcontainer_popup()

//! Constructs the first parameter for
//! create_peepholed_toplevel_listcontainer_popup, for creating the
//! combo-box's popup.

create_peepholed_toplevel_listcontainer_popup_args
combobox_listcontainer_popup_args(const element_impl &parent_element,
				  const new_listlayoutmanager &style,
				  const const_popup_list_appearance &appearance,
				  unsigned nesting_level)
	LIBCXX_HIDDEN;

//! Create the peepholed combo-box list.

//! This is passed in as the first callback to
// !create_peepholed_toplevel_listcontainer_popup(), in order to create
//! a list in the peepholed popup.
//!
//! The first two parameters the callback receives gets passed through to
//! combobox_create_list().
//!
//! style is the return value from combobox_new_listlayouamanager().
//!
//! Returns what the create_peepholed_toplevel_listcontainer_popup()
//! should return, and sets popup_containerptr to the
//! custom_combobox_popup_container.

create_popup_factory_ret_t
combobox_create_list(const container_impl &peephole_container,
		     const popup_attachedto_info &attachedto_info,
		     const new_listlayoutmanager &style,
		     const const_popup_list_appearance &appearance,
		     custom_combobox_popup_containerptr &popup_containerptr)
	LIBCXX_HIDDEN;

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

	const ref<custom_combobox_containerObj::implObj> combo_container_impl;

	//! Current selection callback.
	mpobj<custom_combobox_selection_changed_t> selection_changed;

	//! Whether selection_changed callback changing is enabled

	//! Turned off in standard combo-boxes.
	const bool selection_changed_enabled;

	//! Constructor
	implObj(const ref<custom_combobox_containerObj::implObj>
		&combo_container_impl,
		const new_custom_comboboxlayoutmanager &style,
		bool selection_changed_enabled);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the custom_comboboxlayoutmanager.

	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
