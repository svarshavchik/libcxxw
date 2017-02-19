/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_h
#define gridlayoutmanager_h

#include "x/w/gridlayoutmanager.H"
#include "x/w/border_infommfwd.H"
#include "layoutmanager.H"
#include "gridfactory.H"
#include "metrics_grid_posfwd.H"
#include "connection_threadfwd.H"
#include "current_border_implfwd.H"
#include "grid_elementobj.H"
#include <x/vector.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Look up where the element is.

//! The value in the lookup table keyed by an element in the grid.
//! Gives the element's row and column, which can be used to locate the
//! grid_element object in the two-dimensional elements vector.

class LIBCXX_HIDDEN gridlayoutmanagerObj::lookup_infoObj : virtual public obj {

 public:

	//! Row in the elements vector
	size_t row;

	//! Column in the elements vector
	size_t col;

	//! When updating the lookup table, a flag that's used to purge removed elements.
	bool seen;
};

//! The grid layout manager implementation object.

class LIBCXX_HIDDEN gridlayoutmanagerObj::implObj
	: public layoutmanagerObj::implObj {

 public:

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~implObj();

	/////////////////////////////////////////////////////////////////////
	//
	// Overridden methods.

	void child_metrics_updated(IN_THREAD_ONLY) override;

	void recalculate(IN_THREAD_ONLY) override;

	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void
			       (const element &e)> &callback) override;

	layoutmanager create_public_object() override;

 protected:
	/////////////////////////////////////////////////////////////////////
	//
	// The official elements in the grid are stored in this mutex-protected
	// map.

	//! Contents of the grid are added or removed to this map.

 public:
	grid_map_t grid_map;

	//! Insert a new grid element.
	void insert(grid_map_t::lock &lock,
		    const element &new_element,
		    const new_grid_element_info &info);

	//! Construct a new border object.

	current_border_impl get_custom_border(const border_infomm &info);

 private:

	//! Store the grid calculation data out of harm's way.

	class elementsObj;

	const ref<elementsObj> grid_elements_thread_only;

	//! Rebuild elements, if needed.

	bool rebuild_elements(IN_THREAD_ONLY);

	//! After rebuilding the elements initialize() the new ones.

	void initialize_new_elements(IN_THREAD_ONLY);

 public:
	THREAD_DATA_ONLY(grid_elements);

	//! The container's position has been updated.

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;
};

LIBCXXW_NAMESPACE_END

#endif
