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
#include "connection_threadfwd.H"
#include "current_border_implfwd.H"
#include "grid_elementobj.H"
#include <x/vector.H>
#include <tuple>
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

	//! Remove the entire row.
	void remove_row(size_t row);

	//! Remove everything.
	void remove_all_rows(grid_map_t::lock &lock);

	//! Set default row alignment.
	void row_alignment(size_t row, valign alignment);

	//! Set default column alignment
	void col_alignment(size_t col, halign alignment);

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

	void child_metrics_updated(IN_THREAD_ONLY) override;

	void recalculate(IN_THREAD_ONLY) override;

	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void
			       (const element &e)> &callback) override;

	layoutmanager create_public_object() override;

	// create_public_object() simply calls this.
	gridlayoutmanager create_gridlayoutmanager();

	//! Get the display element at the given location.
	elementptr get(size_t row, size_t col);
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

	current_border_impl get_custom_border(const border_infomm &info);

	//! Construct a new border object.

	current_border_impl get_theme_border(const std::string_view &id);

 private:

	//! Store the grid calculation data out of harm's way.

	class elementsObj;

	const ref<elementsObj> grid_elements_thread_only;

	//! Rebuild elements, if needed.

	bool rebuild_elements(IN_THREAD_ONLY);

	//! Invoked from rebuild_elements() after the grid map gets locked.

	//! A hook for a subclass to perform any processing on the grid
	//! map nodes. The default implementation does nothing.

	virtual void rebuild_elements_start(IN_THREAD_ONLY,
					    grid_map_t::lock &lock);

	//! After rebuilding the elements initialize() the new ones.

	void initialize_new_elements(IN_THREAD_ONLY);

 public:
	THREAD_DATA_ONLY(grid_elements);

	//! The container's position has been updated.

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;

	//! Redraw the borders around the child element.

	void child_background_color_changed(IN_THREAD_ONLY,
					    const elementimpl &child) override;

	//! Redraw the borders around the child element.
	void child_visibility_changed(IN_THREAD_ONLY,
				      const elementimpl &child) override;
 private:
	//! Redraw the borders around the child element.

	void redraw_child_borders_and_padding(IN_THREAD_ONLY,
					      const elementimpl &child);
 public:


	//! Recalculate paddings.

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;

	//! Return the display element's position with padding.

	rectangle padded_position(IN_THREAD_ONLY,
				  const elementimpl &e)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
