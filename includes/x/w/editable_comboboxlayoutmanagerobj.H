/*
** Copyright 2017-2019 Double Precision, Inc.
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
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	editable_comboboxlayoutmanagerObj(const ref<implObj> &impl,
					  const ref<listlayoutmanagerObj
					  ::implObj>
					  &list_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~editable_comboboxlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Install a new selection changed callback.
	void selection_changed(const editable_combobox_selection_changed_t &);

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using standard_comboboxlayoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
