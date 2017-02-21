/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanagerobj_h
#define x_w_gridlayoutmanagerobj_h

#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/gridfactoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/elementobj.H>
#include <x/w/grid_elementfwd.H>
#include <x/w/types.H>
#include <x/mpobj.H>
#include <x/refptr_hash.H>

#include <vector>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! The grid layout manager

//! \see gridlayoutmanager

class gridlayoutmanagerObj : public layoutmanagerObj {

public:

	class lookup_infoObj;

	//! Internal lookup object stored in grid_map_info_t

	typedef ref<lookup_infoObj> lookup_info;

	//! Mutex-protected container for elements in the grid.

	//! \internal
	//!
	//! Since it's not only accessible from the connection thread.
	//! The main application thread can insert or delete stuff here.
	//!
	//! get_layoutmanager() constructs this \c gridlayoutmanager
	//! which acquires a lock on this internal object. The lock gets
	//! released when the last reference to the \c gridlayoutmanager
	//! goes out of scope and this object gets destroyed.
	//!
	//! This lock may block the connection thread, so the
	//! \c gridlayoutmanager object MUST NOT persist!
	//!
	//! Inserting or deleting an element sets the modified flag.
	//! Inserting or deleting an element requires using the layout
	//! manager object, whose destructor invokes needs_recalculation()
	//! as a batched job, which will eventually call recalculate(), that
	//! picks up the ball and runs with it.
	//!
	//! When a child element's metrics have changed, child_metrics_updated
	//! is set to true. rebuild_elements() returns true if either modified
	//! or child_metrics_updated is set to true, but it skips rebuilding
	//! all_elements unless modified is true.

	struct grid_map_info_t {

		//! Container for the elements in the grid.
		typedef std::vector<std::vector<grid_element>> elements_t;

		//! The elements in the grid.

		elements_t elements;

	private:
		//! Internal flag: element modification processing is needed.
		bool modified_flag=false;

		//! Internal flag: element_lookup_table needs rebuilding
		bool lookup_table_is_current=true;

		//! Container for the element lookup table.

		typedef std::unordered_map<ref<elementObj::implObj>,
					   lookup_info> lookup_t;

		//! An internal lookup table.

		//! The elements in the grid are stored in the elements
		//! two-dimension vector, and we also need to look them up,
		//! by element.

		lookup_t lookup;

	public:

		//! The elements vector has been changed.

		//! Two separate things need to be done after elements are
		//! changed: the lookup table needs updating, and the
		//! background thread need to process the element modifications.
		//!
		//! Clear the lookup_table_is_current flag, and set the
		//! modified_flag.
		void elements_have_been_modified()
		{
			lookup_table_is_current=false;
			modified_flag=true;
		}

		//! Check the 'modified_flag'.

		bool element_modifications_need_processing() const
		{
			return modified_flag;
		}

		//! Turn off the 'modified_flag'

		void element_modifications_are_processed()
		{
			modified_flag=false;
		}

		//! Return the lookup table.

		//! Before returning it, if the lookup table needs updating
		//! it gets updated.
		const lookup_t &get_lookup_table();

		grid_map_info_t() LIBCXX_HIDDEN;

		~grid_map_info_t() LIBCXX_HIDDEN;
	};

	//! Mutex-protected grid map.

	typedef mpobj<grid_map_info_t, std::recursive_mutex> grid_map_t;

private:
	//! An internal lock.

	grid_map_t::lock lock;

public:
	friend class gridfactoryObj;

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	gridlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~gridlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append a row to the grid, and create elements in it.
	gridfactory append_row();

	//! Remove a display element from the given location.
	void erase(dim_t x, dim_t y);

	//! Get the display element at the given location.
	elementptr get(dim_t x, dim_t y);
};

LIBCXXW_NAMESPACE_END

#endif
