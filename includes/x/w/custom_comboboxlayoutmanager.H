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

//! Callback that gets invoked when a combo-box selection is made.

//! The same callback as the list layout manager's selection changed
//! callback, with two additional parameters:
//!
//! - current_selection: additional element parameter which is the element
//! that was created by the custom_combobox_selection_factory_t.
//!
//! - popup_element: the internally created popup-element. The callback will
//! typically want to hide()it after making the new selection.

typedef std::function<void (list_lock &lock,
			    const listlayoutmanager &llm,
			    size_t item_index,
			    bool selected_flag,
			    const element &current_selection,
			    const element &popup_element,
			    const busy &mcguffin)
		      > combobox_selection_changed_t;

//! Create a custom combobox.

//! Passed to create_focusable_container() to create a focusable container
//! with a \ref custom_comboboxlayoutmanager "custom combo-box layout manager".
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
};

//! The default custom combo-box selection element factory.

std::tuple<element, ref<focusableImplObj>
	   > default_combobox_selection_factory(const factory &);

LIBCXXW_NAMESPACE_END

#endif
