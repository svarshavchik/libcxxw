/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_h
#define gridlayoutmanager_h

#include "x/w/gridlayoutmanager.H"
#include "layoutmanager.H"
#include "metrics_grid_posfwd.H"
#include "connection_threadfwd.H"
#include <x/vector.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

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

	//! Grid element map key.

	//! We store the grid elements in a map, keyed by x and y coordinates.

	typedef std::tuple<dim_t, dim_t> grid_map_key_t;
 private:
	//! Grid element map value

	//! We store the grid elements in a map, together with precalculated
	//! grid_pos object for the element.

	struct grid_map_value_t {

		//! The element in the grid
		element child_element;

		//! The element's complete position.
		metrics::grid_pos pos;
	};

	//! Mutex-protected container for elements in the grid.

	//! Since it's not only accessible from the connection thread.
	//! The main application thread can insert or delete stuff here.
	//!
	//! Inserting or deleting an element sets the modified flag.
	//! Inserting or deleting an element requires using the layout
	//! manager object, whose destructor invokes needs_recalculation
	//! as a batched job, which will eventually call recalculate(), that
	//! picks up the ball and runs with it.
	//!
	//! When a child element's metrics have changed, child_metrics_updated
	//! is set to true. rebuild_elements() returns true if either modified
	//! or child_metrics_updated is set to true, but it skips rebuilding
	//! all_elements unless modified is true.

	struct grid_map_info_t {

		bool modified=false;

		bool child_metrics_updated=false;
		std::map<grid_map_key_t, grid_map_value_t> elements;
	};

	typedef mpobj<grid_map_info_t> grid_map_t;

	//! Contents of the grid are added or removed to this map.

	grid_map_t grid_map;

 public:

	//! Insert a new grid element.
	void insert(const element &new_element,
		    dim_t x, dim_t y, dim_t width, dim_t height);

 private:

	//! Store the grid calculation data out of harm's way.

	class elementsObj;

	const ref<elementsObj> grid_elements_thread_only;

	//! Rebuild elements, if needed.

	bool rebuild_elements(IN_THREAD_ONLY);

 public:
	THREAD_DATA_ONLY(grid_elements);

	//! The container's position has been updated.

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;
};

LIBCXXW_NAMESPACE_END

#endif
