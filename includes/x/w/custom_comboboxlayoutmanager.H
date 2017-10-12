/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_comboboxlayoutmanager_h
#define x_w_custom_comboboxlayoutmanager_h

#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/custom_comboboxlayoutmanagerfwd.H>
#include <x/w/custom_comboboxlayoutmanagerobj.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/containerobj.H>
#include <x/w/focusable_container.H>
#include <x/w/factoryfwd.H>
#include <x/w/element.H>
#include <x/w/container.H>
#include <x/w/focusable.H>
#include <x/ref.H>

#include <functional>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Signature for a custom combo-box selection factory.

typedef std::function<focusable (const factory &,
				 const ref<layoutmanagerObj::implObj> &)
		      > custom_combobox_selection_factory_t;

//! Information passed to the \ref custom_combobox_selection_changed_t "custom combo-box selection changed callback".

struct LIBCXX_HIDDEN custom_combobox_selection_changed_info_t {

	//! The layout manager
	const custom_comboboxlayoutmanager &lm;

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

typedef std::function<void (const custom_combobox_selection_changed_info_t &)
		      > custom_combobox_selection_changed_t;




//! Information passed to the \ref custom_combobox_selection_search_t "custom combo-box selection search callback".

struct LIBCXX_HIDDEN custom_combobox_selection_search_info_t {

	//! A lock on combo-box's underlying list
	list_lock &lock;

	//! The layout manager
	const custom_comboboxlayoutmanager &lm;

	//! Text to search
	const std::u32string &text;

	//! Starting position to search
	size_t starting_index;

	//! The item element that has been selected or unselected.
	const element &current_selection;

	//! Available mcguffin for blocking all further processing.
	const busy &mcguffin;
};

//! Callback that gets invoked when a combo-box selection is made.

typedef std::function<void (const custom_combobox_selection_search_info_t &)
		      > custom_combobox_selection_search_t;


/*!

Create a custom combobox.

Passed to create_focusable_container() to create a focusable container
with a \ref custom_comboboxlayoutmanager "custom combo-box layout manager".

\code
INSERT_LIBX_NAMESPACE::w::new_custom_comboboxlayoutmanager cclm{
    []
    (const INSERT_LIBX_NAMESPACE::w::factory &f)
    {
        return f->create_focusable_label("");
    }
};
};
\endcode

The members/methods of this class are not thread-safe. It is intended that
an execution thread constructs an instance of this class in automatic scope,
then passes the class instance to create_focusable_container().

The argument to the constructor is a factory for creating a display
element that represents the current selection in the combo-box. The callback
must use the factory that it receives as a parameter to create exactly one
display element.

This current selection display element must be focusable, and the factory
must return the focusable element as well, so for a basic, simple
focusable element the element created by the factory gets returned.

\code
cclm.selection_changed=
    []
    (const custom_combobox_selection_changed_info_t &info)
    {
    };

cclm.selection_search=
    []
    (const custom_combobox_selection_search_info_t &search)
    {
    };
\endcode

Sets additional callback for the new custom combo-box. The selection_changed
callback gets invoked when a combo-box item is selected or unselected.
The selection_search callback gets invoked when the keyboard focus is on the
current selection focusable and text is typed.

*/

class LIBCXX_HIDDEN new_custom_comboboxlayoutmanager
	: public new_focusable_layoutmanager {

 public:

	//! Constructor
	new_custom_comboboxlayoutmanager(const
					 custom_combobox_selection_factory_t
					 &selection_factory) LIBCXX_PUBLIC;

	//! Destructor
	~new_custom_comboboxlayoutmanager() LIBCXX_PUBLIC;

	//! Current selection element factory

	custom_combobox_selection_factory_t selection_factory;

	//! Callback that gets invoked when the combo-box selection changes.

	custom_combobox_selection_changed_t selection_changed;

	//! Search for a selection

	custom_combobox_selection_search_t selection_search;

	//! Create the focusable container.

	//! \internal
	focusable_container create(const ref<containerObj::implObj> &)
		const override;

 protected:

	struct create_impl_info;

	//! Internal constructor

	//! \internal
	virtual ref<custom_comboboxlayoutmanagerObj::implObj
		    > create_impl(const create_impl_info &) const;

	//! Return the selection_changed callback.

	//! \internal
	virtual custom_combobox_selection_changed_t get_selection_changed()
		const;
};

LIBCXXW_NAMESPACE_END

#endif
