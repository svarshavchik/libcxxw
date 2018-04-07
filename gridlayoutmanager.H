/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_h
#define gridlayoutmanager_h

#include "x/w/gridlayoutmanager.H"
#include "x/w/border_infommfwd.H"
#include "x/w/grid_map_infofwd.H"
#include "layoutmanager.H"
#include "gridfactory.H"
#include "metrics_grid_posfwd.H"
#include "x/w/connection_threadfwd.H"
#include "current_border_implfwd.H"
#include "grid_elementobj.H"
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
	implObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~implObj();

	//! Set requested column width.

	//! \see gridlayoutmanager
	void requested_col_width(size_t col, int percentage);

	//! Set requested row height.

	//! \see gridlayoutmanager
	void requested_row_height(size_t row, int percentage);

	//! Create a gridfactory for me.

	gridfactory create_gridfactory(//! The public layout manager object.

				       //! This is the gridlayoutmanager of
				       //! which we are the internal
				       //! implementation object. Or,
				       //! perhaps the listlayoutmanager
				       // facade.
				       layoutmanagerObj *public_object,
				       size_t row, size_t col);

	//! Append a row, then call create_gridfactory()
	gridfactory append_row(layoutmanagerObj *public_object);

	//! Insert a row, then call create_gridfactory()
	gridfactory insert_row(layoutmanagerObj *public_object, size_t row);

	//! Append elements at the end of an existing row.
	gridfactory append_columns(layoutmanagerObj *public_object,
				   size_t row_number);

	//! Insert elements before an existing element on an existing row.
	gridfactory insert_columns(layoutmanagerObj *public_object,
				   size_t row_number, size_t col_number);

	//! Remove one or more elements from a row.

	static void remove(grid_map_t::lock &lock, size_t row,
			   size_t col, size_t n_elements=1);

	//! Remove the entire row.
	void remove_row(size_t row);

	//! Remove the entire row.
	void remove_rows(size_t row, size_t n);

	//! Remove everything.
	void remove_all_rows(grid_map_t::lock &lock);

	//! Set default row alignment.
	void row_alignment(size_t row, valign alignment);

	//! Set default column alignment.
	void col_alignment(size_t col, halign alignment);

	//! Set default padding.
	void row_top_padding_set(size_t row,
			     const dim_arg &padding);

	//! Set default padding.
	void row_bottom_padding_set(size_t row,
				const dim_arg &padding);

	//! Set default padding.
	void col_left_padding_set(size_t col,
			      const dim_arg &padding);

	//! Set default padding.
	void col_right_padding_set(size_t col,
			       const dim_arg &padding);

	//! How many rows there are here.
	size_t rows();

	//! How many defined cells there are in the given row.

	//! This does not include spanned cells.
	size_t cols(size_t row);

	//! Remove all existing elements in the row, then call create_gridfactory()
	gridfactory replace_row(layoutmanagerObj *public_object, size_t row);

	/////////////////////////////////////////////////////////////////////
	//
	// Overridden methods.

	void child_metrics_updated(ONLY IN_THREAD) override;

	void recalculate(ONLY IN_THREAD) override;

	void do_for_each_child(ONLY IN_THREAD,
			       const function<void
			       (const element &e)> &callback) override;

	layoutmanager create_public_object() override;

	// create_public_object() simply calls this.
	gridlayoutmanager create_gridlayoutmanager();

	//! Get the display element at the given location.
	elementptr get(size_t row, size_t col);

	//! Look up which row and column this element is in.

	//! Returns [row, col] if the element was found.

	std::optional<std::tuple<size_t, size_t>>
		lookup_row_col(const ref<elementObj::implObj> &e);

	//! Look up which row and column this element is in.

	//! Returns [row, col] if the element was found.

	static std::optional<std::tuple<size_t, size_t>>
		lookup_row_col(grid_map_t::lock &lock,
			       const ref<elementObj::implObj> &e);

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
	void insert(grid_map_t::lock &lock,
		    const element &new_element,
		    new_grid_element_info &info);

	//! Construct a new border object.

	current_border_impl get_current_border(const border_arg &arg);

	//! Implement default_row_border()

	void default_row_border(size_t row, const border_arg &arg);

	//! Implement default_col_border()

	void default_col_border(size_t col, const border_arg &arg);

	//! Remove all defaults.

	void remove_all_defaults();

 private:

	//! Store the grid calculation data out of harm's way.

	class elementsObj;

	const ref<elementsObj> grid_elements_thread_only;

	//! Rebuild elements, if needed.

	bool rebuild_elements(ONLY IN_THREAD);

	//! Invoked from rebuild_elements() after the grid map gets locked.

	//! A hook for a subclass to perform any processing on the grid
	//! map nodes. The default implementation does nothing.

	virtual void rebuild_elements_start(ONLY IN_THREAD,
					    grid_map_t::lock &lock);

	//! After rebuilding the elements initialize() the new ones.

	void initialize_new_elements(ONLY IN_THREAD);

 public:
	THREAD_DATA_ONLY(grid_elements);

	//! The container's position has been updated.

	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;

	//! Redraw the borders around the child element.

	void child_background_color_changed(ONLY IN_THREAD,
					    const elementimpl &child) override;

	//! If the child element is remove_when_hidden we need to recalculate.

	void requested_child_visibility_changed
		(ONLY IN_THREAD,
		 const elementimpl &child, bool) override;

	//! Redraw the borders around the child element, unless it is remove_when_hidden
	void inherited_child_visibility_changed
		(ONLY IN_THREAD,
		 const elementimpl &child,
		 inherited_visibility_info &info) override;
 private:
	//! Redraw the borders around the child element.

	void redraw_child_borders_and_padding(ONLY IN_THREAD,
					      const elementimpl &child);
 public:


	//! Recalculate paddings.

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;

	//! Return the display element's position with padding.

	rectangle padded_position(ONLY IN_THREAD,
				  const elementimpl &e)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
