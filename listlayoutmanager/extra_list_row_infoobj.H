/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_extra_list_row_infoobj_h
#define x_w_extra_list_row_infoobj_h

#include "listlayoutmanager/extra_list_row_infofwd.H"
#include "popup/popupfwd.H"
#include "connection_threadfwd.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "x/w/listlayoutmanagerbasefwd.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/shortcutfwd.H"
#include "x/w/rectangle.H"
#include <x/obj.H>

#include <functional>

LIBCXXW_NAMESPACE_START

struct textlist_rowinfo;

//! Implementation object.

//! \see extra_list_row_info

class LIBCXX_HIDDEN extra_list_row_infoObj : virtual public obj {

	class shortcut_implObj;

	typedef ref<shortcut_implObj> shortcut_impl;

	typedef ptr<shortcut_implObj> shortcut_implptr;

	//! An optional, installed shortcut for this list item.

	shortcut_implptr current_shortcut;

	//! My row number.

	//! This get updated in recalculate().
	size_t current_row_number_thread_only;

 public:
	THREAD_DATA_ONLY(current_row_number);

	//! Constructor
	extra_list_row_infoObj();

	//! Destructor
	~extra_list_row_infoObj();

	//! Row type
	list_row_type_t row_type=list_row_type_t::enabled;

	//! Row is selected
	bool selected=false;

	//! This item can be selected via keyboard or pointer
	bool enabled() const;

	//! Initialize this row's metadata.

	void set_meta(const listlayoutmanager &lm,
		      const textlist_rowinfo &meta);

	//! This item's status change callback.

	std::function<list_item_status_change_callback_t
		      > status_change_callback=default_status_change_callback;

	//! Default no-op status_change_callback.

	static void default_status_change_callback(list_lock &, size_t, bool);

 private:

	//! Optional information associated with a menu_item.
	menu_item_t menu_item;

 public:

	//! Whether this list item has a popup.

	bool has_submenu() const
	{
		return std::holds_alternative<menu_item_submenu>(menu_item);
	}

	//! Whether this list item is an option

	bool is_option() const
	{
		return std::holds_alternative<menu_item_option>(menu_item);
	}

	//! Display the popup.

	//! The parameter specifies the absolute location of the item's row
	//! in screen coordinates.
	//!
	//! Invokes current_popup_attachedto_handler's
	//! update_attachedto_element_position(), then show_alls() the popup.

	void show_submenu(IN_THREAD_ONLY, const rectangle &);

	//! Hide or display the popup

	//! Like show_submenu(), but toggles the visibility.

	void toggle_submenu(IN_THREAD_ONLY, const rectangle &);

	//! Return the submenu's layoutmanager.

	listlayoutmanager submenu_layoutmanager();
};

LIBCXXW_NAMESPACE_END

#endif
