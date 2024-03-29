/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_h
#define gridlayoutmanager_h

#include "x/w/gridlayoutmanager.H"
#include "x/w/border_infommfwd.H"
#include "x/w/grid_map_infofwd.H"
#include "x/w/impl/layoutmanager.H"
#include "gridfactory.H"
#include "metrics_grid_posfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/impl/current_border_implfwd.H"
#include "grid_elementobj.H"
#include "synchronized_axis_valuefwd.H"
#include <x/vector.H>
#include <tuple>
#include <utility>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! The grid layout manager implementation object.

class LIBCXX_HIDDEN gridlayoutmanagerObj::implObj
	: public layoutmanagerObj::implObj {

 public:

	//! Constructor
	implObj(const container_impl &container_impl,
		const new_gridlayoutmanager &glm);

	//! Destructor
	~implObj();

	//! Preparing to uninstall.

	void uninstalling(ONLY IN_THREAD) override;

	//! Set requested column width.

	//! \see gridlayoutmanager
	static void requested_col_width(grid_map_t::lock &grid_lock,
					size_t col, int percentage);

	//! Set requested row height.

	//! \see gridlayoutmanager
	static void requested_row_height(grid_map_t::lock &grid_lock,
					 size_t row, int percentage);

	//! Create a gridfactory for me.

	static gridfactory
		create_gridfactory(//! The public layout manager object.

				   //! This is the gridlayoutmanager of
				   //! which we are the internal
				   //! implementation object. Or,
				   //! perhaps the listlayoutmanager
				   // facade.
				   gridlayoutmanagerObj *public_object,
				   size_t row, size_t col,
				   bool replace_existing=false);

	//! Append a row, then call create_gridfactory()
	static gridfactory append_row(gridlayoutmanagerObj *public_object);

	//! Insert a row, then call create_gridfactory()
	static gridfactory insert_row(gridlayoutmanagerObj *public_object,
				      size_t row);

	//! Append elements at the end of an existing row.
	static gridfactory append_columns(gridlayoutmanagerObj *public_object,
					  size_t row_number);

	//! Insert elements before an existing element on an existing row.
	static gridfactory insert_columns(gridlayoutmanagerObj *public_object,
					  size_t row_number, size_t col_number);

	//! Replace an existing element

	static gridfactory replace_cell(gridlayoutmanagerObj *public_object,
					size_t row_number, size_t col_number);

	//! Remove one or more elements from a row.

	static void remove(grid_map_t::lock &grid_lock,
			   size_t row,
			   size_t col, size_t n_elements=1);

	//! Remove the entire row.
	static void remove_row(grid_map_t::lock &grid_lock,
			       size_t row);

	//! Remove the entire row.
	static void remove_rows(grid_map_t::lock &grid_lock,
				size_t row, size_t n);

	//! Remove everything.
	static void remove_all_rows(grid_map_t::lock &grid_lock);

	//! Set default row alignment.
	static void row_alignment(grid_map_t::lock &grid_lock,
				  size_t row, valign alignment);

	//! Set default column alignment.
	static void col_alignment(grid_map_t::lock &grid_lock,
				  size_t col, halign alignment);

	//! Set default padding.
	static void row_top_padding_set(grid_map_t::lock &grid_lock,
					size_t row,
					const dim_arg &padding);

	//! Set default padding.
	static void row_bottom_padding_set(grid_map_t::lock &grid_lock,
					   size_t row,
					   const dim_arg &padding);

	//! Set default padding.
	static void col_left_padding_set(grid_map_t::lock &grid_lock,
					 size_t col,
					 const dim_arg &padding);

	//! Set default padding.
	static void col_right_padding_set(grid_map_t::lock &grid_lock,
					  size_t col,
					  const dim_arg &padding);

	//! Remove all existing elements in the row, then call create_gridfactory()
	static gridfactory replace_row(gridlayoutmanagerObj *public_object,
				       size_t row);

	//! Called from recalculate() to recompute our metrics.

	//! The passed-in flag indicates whether our container is already
	//! sized, for informational purposes. Returns a flag indicating whether
	//! the elements in the container have been changed in some way.
	//!
	//! If so, recalculate() makes sure that the container is scheduled
	//! for positioning processing. The metrics could've been unchanged,
	//! but the child elements may need to be moved. This is handled by
	//! position update processing, so this makes sure this happens even
	//! if this container's metrics are unchanged, and its parent container
	//! does not reposition it.
	//!
	//! The passed in already-sized flag indicates whether the position
	//! processing will happen, in this case, and its false if this
	//! container's size is empty (not yet sized), so position processing
	//! gets skipped. rebuild_elements_and_update_metrics does not do
	//! anything with this flag, that recalculate() passes in, per se.
	//! Overridden subclasses can use this flag to indicate whether they
	//! should care to do anything, themselves.

	virtual bool rebuild_elements_and_update_metrics
		(ONLY IN_THREAD,
		 grid_map_t::lock &grid_lock,
		 bool already_sized);

	/////////////////////////////////////////////////////////////////////
	//
	// Overridden methods.

	void child_metrics_updated(ONLY IN_THREAD) override;

	void recalculate(ONLY IN_THREAD) override;

	void do_for_each_child(ONLY IN_THREAD,
			       const function<void
			       (const element &e)> &callback) override;

	size_t num_children(ONLY IN_THREAD) override;

	layoutmanager create_public_object() override;

	// create_public_object() simply calls this.
	gridlayoutmanager create_gridlayoutmanager();

	//! Acquire a grid map lock and call get().
	elementptr lock_and_get(size_t row, size_t col);

	//! Get the display element at the given location.
	static elementptr get(const grid_map_t::lock &grid_lock,
			      size_t row, size_t col);

	//! Look up which row and column this element is in.

	//! Returns [row, col] if the element was found.

	static std::optional<std::tuple<size_t, size_t>>
		lookup_row_col(const grid_map_t::lock &grid_lock,
			       const ref<elementObj::implObj> &e);

#if 0
	//! Look up which row and column this element is in.

	//! Returns [row, col] if the element was found.

	static std::optional<std::tuple<size_t, size_t>>
		lookup_row_col(grid_map_t::lock &lock,
			       const ref<elementObj::implObj> &e);
#endif
	//! New container metrics have been calculated.

	//! Call set_element_metrics() on the container, accordingly.

	virtual void set_element_metrics(ONLY IN_THREAD,
					 const metrics::axis &h,
					 const metrics::axis &v);

 protected:
	/////////////////////////////////////////////////////////////////////
	//
	// The official elements in the grid are stored in this mutex-protected
	// map.

	//! Contents of the grid are added or removed to this map.

 public:

	//! The elements in the grid.

	grid_map_t grid_map;

	//! Insert a new grid element.
	static void insert(grid_map_t::lock &lock,
			   const container_impl &layout_container_impl,
			   const element &new_element,
			   new_grid_element_info &info,
			   bool replace_existing);

	//! Construct a new border object.

	current_border_impl get_current_border(const border_arg &arg);

	//! Implement default_row_border()

	void default_row_border(gridlayoutmanagerObj *public_object,
				size_t row, const border_arg &arg);

	//! Implement default_col_border()

	void default_col_border(gridlayoutmanagerObj *public_object,
				size_t col, const border_arg &arg);

	class elementsObj;

 private:

	//! Store the grid calculation data out of harm's way.

	const ref<elementsObj> grid_elements_thread_only;

	//! What other columns are this grid's columns synchronized with.

	my_synchronized_axis synchronized_columns;

	//! Rebuild elements, if needed.

	bool rebuild_elements(ONLY IN_THREAD, grid_map_t::lock &lock);

	//! Invoked from rebuild_elements() after the grid map gets locked.

	//! A hook for a subclass to perform any processing on the grid
	//! map nodes. The default implementation does nothing.

	virtual void rebuild_elements_start(ONLY IN_THREAD,
					    grid_map_t::lock &lock);

	//! After rebuilding the elements initialize() the new ones.

	void initialize_new_elements(ONLY IN_THREAD, grid_map_t::lock &lock);

 public:
	THREAD_DATA_ONLY(grid_elements);

	//! The container's position has been updated.

	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;

	//! The container's position has not been updated.

	//! But we still reposition the child elements.

	void process_same_position(ONLY IN_THREAD,
				   const rectangle &position) override;

	//! Reposition child elements.

	//! Called by process_updated_position() and process_same_position().

	virtual bool reposition_child_elements(ONLY IN_THREAD,
					       const rectangle &position,
					       grid_map_t::lock &grid_lock);

	//! Redraw the borders around the child element.

	void child_background_color_changed(ONLY IN_THREAD,
					    const element_impl &child) override;

	//! If the child element is remove_when_hidden we need to recalculate.

	void requested_child_visibility_changed
		(ONLY IN_THREAD,
		 const element_impl &child, bool) override;

	//! Redraw the borders around the child element, unless it is remove_when_hidden
	void inherited_child_visibility_changed
		(ONLY IN_THREAD,
		 const element_impl &child,
		 inherited_visibility_info &info) override;
 private:
	//! Redraw the borders around the child element.

	void redraw_child_borders_and_padding(ONLY IN_THREAD,
					      const element_impl &child);
 public:


	//! Recalculate paddings.

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme) override;

	//! Return the display element's position with padding.

	rectangle padded_position(ONLY IN_THREAD,
				  const element_impl &e)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
