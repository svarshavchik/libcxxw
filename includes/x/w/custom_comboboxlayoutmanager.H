/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_comboboxlayoutmanager_h
#define x_w_custom_comboboxlayoutmanager_h

#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/combobox_appearance.H>
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

#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Information passed to the \ref custom_combobox_selection_changed_t "custom combo-box selection changed callback".

struct custom_combobox_selection_changed_info_t {

	//! The layout manager
	const custom_comboboxlayoutmanager &lm;

	//! The item widget that has been selected or unselected.
	const element &current_selection;

	//! The popup

	//! The callback would typically want to hide() it, after recording
	//! a new selection.
	const element &popup_element;

	//! Selected item status metadata

	//! Information from the underlying item selection.
	const list_item_status_info_t &list_item_status_info;

	//! The busy mcguffin factory, for the callback to use if it wants to.

	const busy &mcguffin;
};

//! Information passed to the \ref custom_combobox_selection_search_t "custom combo-box selection search callback".

struct custom_combobox_selection_search_info_t {

	//! A lock on combo-box's underlying list
	list_lock &lock;

	//! The layout manager
	const custom_comboboxlayoutmanager &lm;

	//! Text to search
	const std::u32string &text;

	//! Starting position to search
	size_t starting_index;

	//! The item widget that has been selected or unselected.
	const element &current_selection;

	//! The selection_required flag from new_custom_comboboxlayoutmanager
	bool selection_required;

	//! What triggered the search
	const callback_trigger_t &trigger;

	//! Available mcguffin for blocking all further processing.
	const busy &mcguffin;
};

//! Callback that gets invoked when a combo-box selection is made.

typedef functionref<void (ONLY IN_THREAD,
				  const custom_combobox_selection_search_info_t
				  &)
		    > custom_combobox_selection_search_t;


/*!

Create a custom combobox.

Passed to create_focusable_container() to create a focusable container
with a \ref custom_comboboxlayoutmanager "custom combo-box layout manager".

\code
class my_combobox : public INSERT_LIBX_NAMESPACE::w::new_custom_comboboxlayoutmanager {

    INSERT_LIBX_NAMESPACE::w::focusable selection_factory(const INSERT_LIBX_NAMESPACE::w::factory &f) override
    {
        return f->create_focusable_label("");
    }
};

my_combobox cclm;

auto new_container=factory->create_focusable_container(
    []
    (const INSERT_LIBX_NAMESPACE::w::focusable_container &new_container)
    {
    },
    cclm);
\endcode

The members/methods of this class are not thread-safe. It is intended that
an execution thread constructs an instance of this class in automatic scope,
then passes the class instance to create_focusable_container().

A subclass must override selection_factory() to create a widget that represents the current selection in the combo-box. The callback
must use the factory that it receives as a parameter to create exactly one
widget.

This current selection widget must be focusable, and the factory
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

This layout manager provides the ability to implement a combo-box with
custom functionality, a custom current selection element, and the code
to manually update it when a combo-box selection gets made.

\ref new_standard_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_standard_comboboxlayoutmanager"
and
\ref new_editable_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_editable_comboboxlayoutmanager"
provide layout managers that implement a more conventional combo-box widget.

\par Common settings

\c INSERT_LIBX_NAMESPACE::w::new_custom_comboboxlayoutmanager inherits from
\ref new_list_or_combobox_layoutmanager "INSERT_LIBX_NAMESPACE::w::new_list_or_combobox_layoutmanager"
that has settings which are common with
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager".

*/

class new_custom_comboboxlayoutmanager
	: public new_focusable_layoutmanager,
	  public new_list_or_combobox_layoutmanager {

public:

	//! Constructor
	new_custom_comboboxlayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_custom_comboboxlayoutmanager() LIBCXX_PUBLIC;

	//! Copy constructor
	new_custom_comboboxlayoutmanager(const new_custom_comboboxlayoutmanager
					 &) LIBCXX_PUBLIC;

	//! Assignment operator
	new_custom_comboboxlayoutmanager &operator=
	(const new_custom_comboboxlayoutmanager &) LIBCXX_PUBLIC;

	//! Current selection widget factory

	//! The subclass must override and implement it.
	virtual focusable selection_factory(const factory &) const=0;

	//! Callback that gets invoked when the combo-box selection changes.

	custom_combobox_selection_changed_t selection_changed;

	//! Search for a selection

	custom_combobox_selection_search_t selection_search;

	//! Whether a selection should always "stick"

	//! If reset to false, selecting the currently-deselected combo-box
	//! item deselects it.
	//!
	//! This is used mostly with standard combo-boxes. This has no
	//! tangible result with editable combo-boxes.

	bool selection_required=true;

	//! What other combo-box this combo-box's popup list get synchronized with.

	//! This allows several combo-boxes to have the same width.

	synchronized_axis synchronized_columns;

	//! Appearance of the combo-box

	const_combobox_appearance appearance;

	//! Create the focusable container.

	//! \internal
	focusable_container create(const container_impl &,
				   const function<void
				   (const focusable_container &)>
				   &creator)
		const override;

	struct LIBCXX_HIDDEN create_current_selection;

	//! Internal helper object

	//! \interal
	struct create_current_selection {

		const new_custom_comboboxlayoutmanager &me;

		create_current_selection(
			const new_custom_comboboxlayoutmanager &me
		);

		virtual focusable call_selection_factory(
			const factory &
		) const=0;
	};

protected:

	struct create_impl_info;

	//! Create the input field width that represents the current selection

	//! \internal
	focusable_container create_with_current_selection_factory(
		const container_impl &,
		const function<void (const focusable_container &)> &,
		const create_current_selection &
	) const;

	//! Internal constructor

	//! \internal
	virtual ref<custom_comboboxlayoutmanagerObj::implObj
		    > create_impl(const create_impl_info &) const;

private:
	//! Return the minimum width applicable to this combo-box

	//! The default implementation returns 0.
	//!
	//! new_editable_comboboxlayoutmanager returns
	//! editable_combobox_minimum_width.

	virtual dim_arg minimum_width() const;
public:
	//! Return the selection_changed callback.

	//! \internal
	virtual custom_combobox_selection_changed_t get_selection_changed()
		const;
};

LIBCXXW_NAMESPACE_END

#endif
