/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editable_comboboxlayoutmanager_h
#define x_w_editable_comboboxlayoutmanager_h

#include <x/w/editable_comboboxlayoutmanagerfwd.H>
#include <x/w/editable_comboboxlayoutmanagerobj.H>
#include <x/w/custom_comboboxlayoutmanager.H>

LIBCXXW_NAMESPACE_START

//! Information passed to the \ref editable_combobox_selection_changed_t "editable combo-box selection changed callback".

//! This callback gets invoked whenever an editable combo-box currently
//! selectem item changes as a result of its popup selection.

struct LIBCXX_HIDDEN editable_combobox_selection_changed_info_t {

	//! A lock on combo-box's underlying list
	standard_combobox_lock &lock;

	//! The combo-box's layout manager
	const editable_comboboxlayoutmanager &lm;

	//! Selected item status metadata

	//! Information from the underlying item selection.
	const list_item_status_info_t &list_item_status_info;
};

/*!

Callback that gets invoked when an editable combo-box's selection changes.

*/

typedef std::function<void (const editable_combobox_selection_changed_info_t &)
		      > editable_combobox_selection_changed_t;

/*! Create an editable combobox.

Passed to create_focusable_container() to create a focusable container
with a \ref editable_comboboxlayoutmanager "editable combo-box layout manager".

\code
INSERT_LIBX_NAMESPACE::w::new_editable_comboboxlayoutmanager cclm{
    []
    (const editable_combobox_selection_changed_info_t &info)
    {
    }
};
\endcode

The members/methods of this class are not thread-safe. It is intended that
an execution thread constructs an instance of this class in automatic scope,
then passes the class instance to create_focusable_container().

*/

class LIBCXX_HIDDEN new_editable_comboboxlayoutmanager
	: public new_custom_comboboxlayoutmanager {

 public:

	//! Constructor
	new_editable_comboboxlayoutmanager() LIBCXX_PUBLIC;

	//! Constructor
	new_editable_comboboxlayoutmanager
		(const editable_combobox_selection_changed_t
		 &selection_changed) LIBCXX_PUBLIC;

	//! Destructor
	~new_editable_comboboxlayoutmanager() LIBCXX_PUBLIC;

	//! Callback that gets invoked when the combo-box selection changes.

	editable_combobox_selection_changed_t selection_changed;

 protected:

	//! Internal constructor

	//! \internal
	virtual ref<custom_comboboxlayoutmanagerObj::implObj
		    > create_impl(const create_impl_info &) const;

	//! Return the selection_changed callback.

	//! \internal
	custom_combobox_selection_changed_t get_selection_changed()
		const override;
};

LIBCXXW_NAMESPACE_END

#endif
