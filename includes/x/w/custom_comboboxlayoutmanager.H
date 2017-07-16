/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_comboboxlayoutmanager_h
#define x_w_custom_comboboxlayoutmanager_h

#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/custom_comboboxlayoutmanagerfwd.H>
#include <x/w/custom_comboboxlayoutmanagerobj.H>
#include <x/w/containerobj.H>
#include <x/w/focusable_container.H>
#include <x/w/factoryfwd.H>
#include <x/w/element.H>
#include <x/w/focusable.H>
#include <x/ref.H>

#include <functional>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Signature for a custom combo-box selection factory.

typedef std::function<void (const factory &)
		      > custom_combobox_selection_factory_t;

//! Information passed to the \ref combobox_selection_changed_t "combo-box selection changed callback".

struct LIBCXX_HIDDEN custom_combobox_selection_changed_info_t {

	//! A lock on combo-box's underlying list
	list_lock &lock;

	//! The combo-box's underlying list layout manager
	const listlayoutmanager &llm;

	//! Which combo-box item has been selected or unselected
	size_t item_index;

	//! Whether the specified item has been selected or unselected.
	bool selected_flag;

	//! The item element that has been selected or unselected.
	const element &current_selection;

	//! The popup

	//! The callback would typically want to hide() it, after recording
	//! a new selection.
	const element &popup_element;

	//! Available mcguffin for blocking all further processing.
	const busy &mcguffin;
};

//! Callback that gets invoked when a combo-box selection is made.

//! The same callback as the list layout manager's selection changed
//! callback, with two additional parameters:
//!
//! - current_selection: additional element parameter which is the element
//! that was created by the custom_combobox_selection_factory_t.
//!
//! - popup_element: the internally created popup-element. The callback will
//! typically want to hide()it after making the new selection.

typedef std::function<void (const custom_combobox_selection_changed_info_t &)
		      > combobox_selection_changed_t;

/*!

Create a custom combobox.

Passed to create_focusable_container() to create a focusable container
with a \ref custom_comboboxlayoutmanager "custom combo-box layout manager".

\code
INSERT_LIBX_NAMESPACE::w::new_custom_comboboxlayoutmanager cclm{
    []
    (const INSERT_LIBX_NAMESPACE::w::factory &f)
    {
    },
    []
    (const custom_combobox_selection_info_t &info)
    {
    }
};

\endcode

*/

class LIBCXX_HIDDEN new_custom_comboboxlayoutmanager
	: public new_focusable_layoutmanager {

 public:

	//! Constructor
	new_custom_comboboxlayoutmanager(const
					 custom_combobox_selection_factory_t
					 &selection_factory,

					 const combobox_selection_changed_t
					 &selection_changed) LIBCXX_PUBLIC;

	//! Destructor
	~new_custom_comboboxlayoutmanager() LIBCXX_PUBLIC;

	//! Current selection element factory

	custom_combobox_selection_factory_t selection_factory;

	//! Callback that gets invoked when the combo-box selection changes.

	combobox_selection_changed_t selection_changed;

	//! \internal
	//! Create the focusable container.

	focusable_container create(const ref<containerObj::implObj> &)
		const override;

 protected:

	struct create_impl_info;

	//! Internal constructor

	virtual ref<custom_comboboxlayoutmanagerObj::implObj
		    > create_impl(const create_impl_info &) const;
};

LIBCXXW_NAMESPACE_END

#endif
