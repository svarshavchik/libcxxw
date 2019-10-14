/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_extra_list_row_infoobj_h
#define x_w_extra_list_row_infoobj_h

#include "listlayoutmanager/extra_list_row_infofwd.H"
#include "x/w/impl/popup/popupfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/shortcutfwd.H"
#include "x/w/rectangle.H"
#include <x/obj.H>
#include <x/functionalrefptrfwd.H>

#include <functional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see extra_list_row_info

class LIBCXX_HIDDEN extra_list_row_infoObj : virtual public obj {

	class shortcut_implObj;

	typedef ref<shortcut_implObj> shortcut_impl;

	typedef ptr<shortcut_implObj> shortcut_implptr;

	//! My row number.

	//! This get updated in recalculate().
	size_t current_row_number_thread_only;

	//! Information about this row.

	struct protected_data {
		//! Row type
		list_row_type_t row_type=list_row_type_t::enabled;

		//! Row is selected
		bool selected=false;

		//! This item's status change callback.

		list_item_status_change_callbackptr status_change_callback;

		//! Optional information associated with a menu_item.
		menu_item_t menu_item;

		//! An optional, installed shortcut for this list item.

		shortcut_implptr current_shortcut;
	};

	//! This information requires a list lock for access.

	protected_data data_under_lock;

 public:
	THREAD_DATA_ONLY(current_row_number);

	//! Constructor
	extra_list_row_infoObj();

	//! Destructor
	~extra_list_row_infoObj();

	//! Access locked data.
	inline auto &data(listimpl_info_t::lock &)
	{
		return data_under_lock;
	}

	//! Access locked data.
	inline const auto &data(listimpl_info_t::lock &) const
	{
		return data_under_lock;
	}

	//! This item can be selected via keyboard or pointer
	bool enabled(listimpl_info_t::lock &) const;

	//! Initialize this row's metadata.

	void set_meta(const listlayoutmanager &lm,
		      list_row_info_t &row_info,
		      listimpl_info_t::lock &lock,
		      const textlist_rowinfo &meta);

	//! Whether this list item has a popup.

	bool has_submenu(listimpl_info_t::lock &lock) const
	{
		return std::holds_alternative<menu_item_submenu>
			(data(lock).menu_item);
	}

	//! Whether this list item is an option

	bool is_option(listimpl_info_t::lock &lock) const
	{
		return std::holds_alternative<menu_item_option>
			(data(lock).menu_item);
	}

	//! Display the popup.

	//! The parameter specifies the absolute location of the item's row
	//! in screen coordinates.
	//!
	//! Invokes current_popup_attachedto_handler's
	//! update_attachedto_element_position(), then show_alls() the popup.

	void show_submenu(ONLY IN_THREAD,
			  listimpl_info_t::lock &lock,
			  const rectangle &);

	//! Hide or display the popup

	//! Like show_submenu(), but toggles the visibility.

	void toggle_submenu(ONLY IN_THREAD,
			    listimpl_info_t::lock &lock,
			    const rectangle &);

	//! Return the submenu's layoutmanager.

	listlayoutmanager submenu_layoutmanager(listimpl_info_t::lock &lock);
};

LIBCXXW_NAMESPACE_END

#endif
