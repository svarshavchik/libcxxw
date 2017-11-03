/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_extra_list_row_infoobj_h
#define x_w_extra_list_row_infoobj_h

#include "textlistlayoutmanager/extra_list_row_infofwd.H"
#include "connection_threadfwd.H"
#include "x/w/listlayoutmanagerbasefwd.H"
#include "x/w/textlistlayoutmanagerfwd.H"
#include "x/w/shortcutfwd.H"
#include <x/obj.H>

#include <functional>

LIBCXXW_NAMESPACE_START

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

	//! Set a shortcut for this list item.
	void set_shortcut(const textlistlayoutmanager &lm,
			  const shortcut &);

	//! This item's status change callback.

	std::function<list_item_status_change_callback_t
		      > status_change_callback=default_status_change_callback;

	//! Default no-op status_change_callback.

	static void default_status_change_callback(list_lock &, size_t, bool);
};

LIBCXXW_NAMESPACE_END

#endif
