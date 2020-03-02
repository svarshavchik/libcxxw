/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_popup_containerobj_h
#define custom_combobox_popup_containerobj_h

#include "combobox/custom_combobox_popup_containerfwd.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"

LIBCXXW_NAMESPACE_START

//! Combo-box popup container object.

//! The combo-box popup is a top level window with a peephole. The combo-box
//! is placed in the peephole.
//!
//! \see custom_combobox_popup_container

class LIBCXX_HIDDEN custom_combobox_popup_containerObj : public p_t_l_t {

	//! Alias

	typedef p_t_l_t superclass_t;

public:
	class implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

	//! The combo-box popup container uses the list layout manager.

	const ref<listlayoutmanagerObj::implObj> listlayout_impl;

	//! Constructor
	custom_combobox_popup_containerObj(const ref<implObj> &impl,
					   const ref<listlayoutmanagerObj
					   ::implObj> &listlayout_impl,
					   const popup_attachedto_info
					   &attachedto_info)
		LIBCXX_HIDDEN;

	//! Destructor
	~custom_combobox_popup_containerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
