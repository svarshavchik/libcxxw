/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editable_comboboxlayoutmanager_h
#define x_w_editable_comboboxlayoutmanager_h

#include <x/w/editable_comboboxlayoutmanagerfwd.H>
#include <x/w/editable_comboboxlayoutmanagerobj.H>
#include <x/w/standard_comboboxlayoutmanager.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Information passed to the \ref editable_combobox_selection_changed_t "editable combo-box selection changed callback".

//! This callback gets invoked whenever an editable combo-box currently
//! selectem item changes as a result of its popup selection.

struct editable_combobox_selection_changed_info_t
	: public standard_combobox_selection_changed_info_t {

	//! The combo-box's layout manager
	const editable_comboboxlayoutmanager &lm;
};

/*! Create an editable combobox.

Passed to create_focusable_container() to create a focusable container
with a \ref editable_comboboxlayoutmanager "editable combo-box layout manager".

\code
INSERT_LIBX_NAMESPACE::w::new_editable_comboboxlayoutmanager eclm{
    []
    (const editable_combobox_selection_changed_info_t &info)
    {
    }
};

auto new_container=factory->create_focusable_container(
    []
    (const INSERT_LIBX_NAMESPACE::w::focusable_container &new_container)
    {
    },
    eclm);
\endcode

The members/methods of this class are not thread-safe. It is intended that
an execution thread constructs an instance of this class in automatic scope,
then passes the class instance to create_focusable_container().

\par Synchronized combo-boxes

The width of each individual combo-box gets sized to be just wide enough to
show the widest combo-box item. It's possible to create several combo-boxes
with the same width, sizing all combo-boxes's width based on the wide item
in all the combo-boxes, combined together.

This is simply done by using the same \c new_editable_comboboxlayoutmanager
object. See
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager"
for more information.

*/

class new_editable_comboboxlayoutmanager
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

	//! Implement selection_factory()
	focusable selection_factory(const factory &) const override;

	//! Callback that gets invoked when the combo-box selection changes.

	editable_combobox_selection_changed_t selection_changed;

 protected:

	//! Internal constructor

	//! \internal
	ref<custom_comboboxlayoutmanagerObj::implObj>
		create_impl(const create_impl_info &) const override;

	//! Return the selection_changed callback.

	//! \internal
	custom_combobox_selection_changed_t get_selection_changed()
		const override;
};

LIBCXXW_NAMESPACE_END

#endif
