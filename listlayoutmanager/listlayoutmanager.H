/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutmanager_h
#define listlayoutmanager_h

#include "x/w/listlayoutmanager.H"
#include "x/w/hotspotfwd.H"
#include "gridlayoutmanager.H"
#include "listlayoutmanager/listcontainer.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "listlayoutmanager/listitemfactoryfwd.H"
#include "listlayoutmanager/listitemfactoryobj.H"
#include <x/functional.H>

#include <tuple>

LIBCXXW_NAMESPACE_START

//! The list layout manager implementation object.

//! The list layout manager is really the grid layout manager in disguise.
//!
//! \see listlayoutmanager

class LIBCXX_HIDDEN listlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

	//! Keep track of which row is currently highlighted.

	size_t currently_highlighted_row=-1;

	//! We also keep track of the individual column too

	//! We highlight all columns in the row, but we track of each individual
	//! column whose focus is being reported as such.

	size_t currently_highlighted_col=-1;

	//! Which row was highlighted via keyboard focus.
	size_t previously_highlighted_keyboard_focus_row=-1;

	//! Most recently reported hotspot temperature
	temperature current_temperature=temperature::cold;

 public:
	//! Parent container

	const ref<listcontainerObj::implObj> container_impl;

	//! This list layout manager's style.
	const listlayoutstyle &style;

	//! Number of columns
	const size_t columns;

 private:
	//! Selection type
	std::function<void (list_lock &, const listlayoutmanager &,
			    size_t)
		      > selection_type_thread_only;

	std::function<void (list_lock &, const listlayoutmanager &,
			    size_t, bool, const x::w::busy &)
		      > selection_changed_thread_only;
 public:

	THREAD_DATA_ONLY(selection_type);
	THREAD_DATA_ONLY(selection_changed);

	//! Constructor
	implObj(const ref<listcontainerObj::implObj> &container_impl,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the listlayoutmanager.

	layoutmanager create_public_object() override;

	//! One of my elements gained or lost pointer focus

	void pointer_focus(IN_THREAD_ONLY,
			   const ref<elementObj::implObj> &e);

	//! List container's hotspot temperature changed

	//! Indicates that the pointer button was pressed or released.
	void temperature_changed(IN_THREAD_ONLY);

	//! I just lost or gained keyboard focus.
	void keyboard_focus(IN_THREAD_ONLY, bool flag);

	//! Process cursor down/up events.
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! An list item has been selected.

	void activated(IN_THREAD_ONLY,
		       const listlayoutmanager &my_public_object);

 private:
	//! Look up which row and column this element is in.

	std::tuple<size_t, size_t>
		lookup_row_col(grid_map_t::lock &lock,
			       const ref<elementObj::implObj> &e);

	//! Refresh the visual representation of a list item.

	//! The list item has been selected or unselected. Update its
	//! visual appearance accordingly.
	void refresh(IN_THREAD_ONLY, grid_map_t::lock &lock, const element &e);

	//! If currently_highlighted_row, unhighlight it.

	void unhighlight_current_row(IN_THREAD_ONLY,
				     grid_map_t::lock &l);

	//! Set the appropriate background color for the currently highlighted row
	void highlight_current_row(IN_THREAD_ONLY,
				   grid_map_t::lock &l);

	//! Make sure the current row is visible.
	void ensure_current_row_is_visible(IN_THREAD_ONLY,
					   grid_map_t::lock &l);

 public:

	//////////////////////////////////////////////////////////////////////


	//! Number of items in the list
	size_t size(grid_map_t::lock &lock) const;

	//! Whether list item #i is selected right now.
	bool selected(grid_map_t::lock &lock, size_t i) const;

	//! Select or unselect item #i
	void selected(const listlayoutmanager &me,
		      grid_map_t::lock &lock, size_t i, bool selected_flag);

	void selected(IN_THREAD_ONLY,
		      const listlayoutmanager &me,
		      grid_map_t::lock &lock, size_t i,
		      bool selected_flag);

	//! Autoselect, according to selection_type.

	void autoselect(const listlayoutmanager &me,
			grid_map_t::lock &lock, size_t i);

	//! Remove an item
	void remove_item(const listlayoutmanager &me,
			 grid_map_t::lock &lock, size_t i);

	//! Implement append_item()

	void append_item(const listlayoutmanager &me,
			 const listlayoutstyle::new_list_items_t &new_item);

	//! Implement insert_item()

	void insert_item(const listlayoutmanager &me,
			 grid_map_t::lock &lock,
			 const listlayoutstyle::new_list_items_t &new_item,
			 size_t item_number);

	//! Implement replace_item()

	void replace_item(const listlayoutmanager &me,
			  grid_map_t::lock &lock,
			  const listlayoutstyle::new_list_items_t &new_item,
			  size_t item_number);

	//! Called from the public object to remove all existing items in the list.
	void remove_all_items(const listlayoutmanager &me);

 private:

	//! Callback used by connection_thread_op_t

	typedef void connection_thread_op_t(implObj *, IN_THREAD_ONLY,
					    const listlayoutmanager &,
					    list_lock &,
					    size_t);

	//! Signature for a class member that can be a connection_thread_op_t

	typedef void (implObj::*connection_thread_method_t)
		(IN_THREAD_ONLY,
		 const listlayoutmanager &,
		 list_lock &,
		 size_t);

	//! Execute an operation on a list item in the connection thread.

	//! This is used by the methods that get invoked from the public
	//! layout manager object which specify an item number.
	//!
	//! The publicly-invoked method passes in a reference to the
	//! public object, an acquired lock, and the item number.
	//!
	//! connection_thread_op() invokes the callback in the connection
	//! thread, with an acquired lock and the item number, so that the
	//! real work can be done.

	void connection_thread_op(const listlayoutmanager &me,
				  grid_map_t::lock &lock, size_t i,
				  const std::function<connection_thread_op_t>
				  &callback);

	//! Autoselect, according to selection_type.

	void autoselect(IN_THREAD_ONLY, const listlayoutmanager &me,
			list_lock &lock, size_t item_number);

	//! Remove an item

	void remove_item(IN_THREAD_ONLY, const listlayoutmanager &me,
			 list_lock &lock, size_t item_number);

	//! Insert an item

	void insert_item(IN_THREAD_ONLY, const listlayoutmanager &me,
			 list_lock &lock, size_t item_number,
			 const listlayoutstyle::new_list_items_t &new_item);

	//! Replace an item

	void replace_item(IN_THREAD_ONLY, const listlayoutmanager &me,
			 list_lock &lock, size_t item_number,
			 const listlayoutstyle::new_list_items_t &new_item);

	//! Called from the public object to remove all existing items in the list.
	void remove_all_items(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
