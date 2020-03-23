/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_comboboxlayoutmanagerobj_h
#define x_w_custom_comboboxlayoutmanagerobj_h

#include <x/w/custom_comboboxlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Custom combo-box layout manager object.

//! \see custom_comboboxlayoutmanager

class custom_comboboxlayoutmanagerObj : public listlayoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	custom_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					const ref<listlayoutmanagerObj
					::implObj>
					&list_layout_impl) LIBCXX_HIDDEN;

	//! Destructor
	~custom_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Return the widget that represents the current selection.
	virtual element current_selection();

	//! Return the widget that represents the current selection.
	virtual const_element current_selection() const;

	//! Install a new selection changed callback.
	void on_selection_changed(const custom_combobox_selection_changed_t &);

	//! Install a new selection changed callback.
	void on_selection_changed(ONLY IN_THREAD,
				  const custom_combobox_selection_changed_t &);

private:
	//! Throw an exception

	//! List layout manager method not allowed in combo-boxes.
	void selection_type(const list_selection_type_cb_t &selection_type)
		override LIBCXX_HIDDEN;

	//! Throw an exception

	//! List layout manager method not allowed in combo-boxes.
	void selection_type(ONLY IN_THREAD,
			    const list_selection_type_cb_t &selection_type)
		override LIBCXX_HIDDEN;

	//! Throw an exception

	//! List layout manager method not allowed in combo-boxes.
	void on_list_selection_changed(const list_selection_changed_cb_t
				       &selection_changed)
		override LIBCXX_HIDDEN;

	//! Throw an exception

	//! List layout manager method not allowed in combo-boxes.
	void on_list_selection_changed(ONLY IN_THREAD,
				       const list_selection_changed_cb_t
				       &selection_changed)
		override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
