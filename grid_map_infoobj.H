/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grid_map_infoobj_h
#define grid_map_infoobj_h

#include "x/w/namespace.H"
#include "x/w/grid_map_infofwd.H"
#include "elementfwd.H"
#include "grid_elementfwd.H"
#include "current_border_implfwd.H"
#include "metrics_grid_xy.H"

#include <x/refptr_hash.H>
#include <vector>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! Default grid column info

struct grid_map_column_defaults {

	//! Constructor
	grid_map_column_defaults();

	//! Destructor
	~grid_map_column_defaults();

	//! Copy constructor

	grid_map_column_defaults(const grid_map_column_defaults &);

	//! Move constructor

	grid_map_column_defaults(grid_map_column_defaults &&);

	//! Assignment operator
	grid_map_column_defaults &operator=(const grid_map_column_defaults &);

	//! Assignment operator
	grid_map_column_defaults &operator=(grid_map_column_defaults &&);

	//! Default border above this row
	current_border_implptr default_border;

	//! Default column width (percentage, negative means unspecified).
	int axis_size=-1;

	//! Default column alignment

	halign horizontal_alignment=halign::left;
};

//! Default grid column info

struct grid_map_row_defaults {

	//! Constructor
	grid_map_row_defaults();

	//! Destructor
	~grid_map_row_defaults();

	//! Copy constructor

	grid_map_row_defaults(const grid_map_row_defaults &);

	//! Move constructor

	grid_map_row_defaults(grid_map_row_defaults &&);

	//! Assignment operator
	grid_map_row_defaults &operator=(const grid_map_row_defaults &);

	//! Assignment operator
	grid_map_row_defaults &operator=(grid_map_row_defaults &&);

	//! Default border to the left of this column
	current_border_implptr default_border;

	//! Default row height (percentage, negative means unspecified).
	int axis_size=-1;

	//! Default vertical alignment
	valign vertical_alignment=valign::top;
};

//! Mutex-protected container for elements in the grid.

//! \internal
//!
//! Since it's not only accessible from the connection thread.
//! The main application thread can insert or delete stuff here.
//!
//! So we store a ref to this object, as a mutex-protected object.
//!
//! get_layoutmanager() constructs a \ref gridlayoutmanager "gridlayoutmanager"
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

class LIBCXX_HIDDEN grid_map_infoObj : virtual public obj {

public:

	//! Look up where the element is.

	//! The value in the lookup table keyed by an element in the grid.
	//! Gives the element's row and column, which can be used to locate the
	//! grid_element object in the two-dimensional elements vector.

	class lookup_infoObj : virtual public obj {

	public:

		//! Row in the elements vector
		size_t row;

		//! Column in the elements vector
		size_t col;

		//! When updating the lookup table, a flag that's used to purge removed elements.
		bool seen;

		//! All the border elements, on each side of this element.

		std::vector<element> border_elements;
	};


	//! Internal lookup object.

	typedef ref<lookup_infoObj> lookup_info;

	//! Container for the elements in the grid.
	typedef std::vector<std::vector<grid_element>> elements_t;

	//! The elements in the grid.

	elements_t elements;

	//! Column defaults

	std::unordered_map<metrics::grid_xy, grid_map_column_defaults> column_defaults;

	//! row defaults

	std::unordered_map<metrics::grid_xy, grid_map_row_defaults> row_defaults;

 private:
	//! Internal flag: element modification processing is needed.
	bool modified_flag=false;

	//! Internal flag: element_lookup_table needs rebuilding
	bool lookup_table_is_current=true;

	//! Container for the element lookup table.

	typedef std::unordered_map<elementimpl, lookup_info> lookup_t;

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

	//! Possible change in padding.
	void padding_recalculated()
	{
		modified_flag=true;
	}

	//! Defaults have been removed.
	void defaults_changed()
	{
		borders_changed();
	}

	//! Possible change in borders.
	void borders_changed()
	{
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

	grid_map_infoObj();

	~grid_map_infoObj();
};

LIBCXXW_NAMESPACE_END

#endif
