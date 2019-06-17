/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutstyle_impl_h
#define x_w_textlistlayoutstyle_impl_h

#include "listlayoutmanager/listlayoutstyle_implfwd.H"
#include "listlayoutmanager/menu_item.H"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/listcontainer_pseudo_implfwd.H"
#include "listlayoutmanager/extra_list_row_infofwd.H"
#include "listlayoutmanager/extra_list_row_infoobj.H"
#include "listlayoutmanager/list_cellfwd.H"
#include "listlayoutmanager/list_cellobj.H"
#include "peephole/peepholedfwd.H"
#include "peephole/peepholeobj.H"
#include "x/w/impl/draw_infofwd.H"
#include "x/w/impl/background_colorfwd.H"
#include "x/w/containerfwd.H"
#include "x/w/focusablefwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/impl/focus/focusablefwd.H"
#include "x/w/connection_threadfwd.H"
#include <vector>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Metadata for each list item returned by \ref listlayoutstyle_impl "create_newcell()

struct textlist_rowinfo {
	std::optional<shortcut>			listitem_shortcut;
	list_item_status_change_callbackptr	listitem_callback;

	size_t indent_level=0;

	bool inactive_shortcut=false;

	menu_item_t menu_item;

	void setting_menu_item();
};

//! Populated by create_cells().

struct new_cells_info {

	//! New list cell contents.
	std::vector<list_cell> newcells;

	//! New list rows.
	std::vector<std::tuple<extra_list_row_info, textlist_rowinfo>> rowmeta;
};

//! Specifies the style of the list created by a \ref new_listlayoutmanager.

//! An abstract class that implements create(), that takes a
//! opeepholeObj::implObj that new_listlayoutmanager creates, for the
//! parent element for the peepholed_listcontainer that create() creates.
//!
//! new_listlayoutmanager passes itself as the second parameter.
//!
//! Implements create(), and defines additional virtual methods that specify the
//! list style.

class LIBCXX_HIDDEN listlayoutstyle_impl {

 public:

	//! Create a new list element.
	std::tuple<container, peepholed, focusable, focusable_impl>
		create(const ref<peepholeObj::implObj> &,
		       const new_listlayoutmanager &,
		       const new_listlayoutmanager::list_create_info &,
		       const synchronized_axis &synchronized_columns) const;

	//! How many actual columns there are in the list.

	//! Bulleted-style lists add an additional column for the bullet.
	//! Looks at the \c columns, in the new_listlayoutmanager, and
	//! returns the actual number of columns for the new list (unchanged
	//! in case of a highlighted style list, and one more for the
	//! bulleted-style list).

	virtual size_t actual_columns(const new_listlayoutmanager &) const=0;

	//! Actual column width specification.

	//! Returns requested_col_widths as is, for a highlighted-style list.
	//! For a bulleted list returns a modified map, since column 0 is
	//! the bullet column, so all the requested_col_widths get shifted
	//! over by 1.
	virtual std::unordered_map<size_t, int>
		actual_col_widths(const new_listlayoutmanager &) const=0;

	//! Alignment of all columns.

	//! Returns col_alignments as is, for a highlighted-style list.
	//! For a bulleted list returns a modified map, since column 0 is
	//! the bullet column, so all the col_alignments get shifted
	//! over by 1.

	virtual std::unordered_map<size_t, halign>
		actual_col_alignments(const new_listlayoutmanager &) const=0;

	//! Alignment of all columns.

	//! Returns row_alignments as is, for a highlighted-style list.
	//! For a bulleted list returns a modified map, since column 0 is
	//! the bullet column, so all the row_alignments get shifted
	//! over by 1.

	virtual std::unordered_map<size_t, valign>
		actual_row_alignments(const new_listlayoutmanager &) const=0;

	//! Install the highlighted background color, before drawing.

	//! If this is the highlighted list style. Does nothing, for the
	//! bulleted list style.

	virtual void set_selected_background(ONLY IN_THREAD,
					     draw_info &di,
					     const background_color &bgcolor)
		const=0;

	//! Create internal cell elements based on the list_item_params.

	//! The newcells and rowmeta parameters must refer to empty vectors.
	//! create_cells() populates them.
	//!
	//! Mostly a straightforward conversion, for a highlighted list.
	//! The bulleted list style covertly inserts a bullet cell on every
	//! row.
	//!
	//! The created cells are placed into newcells.
	//!
	//! A record gets placed into rowmeta for each list item.
	//!
	//! The size of newcells will be the number of rowmeta records
	//! multiplied by the number of real columns in the list.
	//!
	//! The pointers from rowmeta point back to the list_item_param
	//! vector.
	//!

	void create_cells(const std::vector<list_item_param> &t,
			  const ref<listlayoutmanagerObj::implObj> &lilm,
			  new_cells_info &new_info)
		const;

	//! Callback indicating that a list item has changed.

	//! Does nothing for the highlighted list. For the bulleted list this
	//! updates the bullet image cell accordingly.
	virtual void selected_changed(list_cell *row,
				      bool selected_flag) const=0;
 private:

	//! Whether a menuoption or a submenu can be specified.

	virtual void menu_attribute_requested() const;

	//! Whether a hierindent can be specified.

	virtual void nonmenu_attribute_requested() const;

 public:
	//! How many extra leading columns there are.

	//! Called by create_cells() to figure out what it's doing.
	virtual size_t extra_leading_columns() const=0;

	//! How many extra trailing columns there are.

	//! Called by create_cells() to figure out what it's doing.
	virtual size_t extra_trailing_columns() const=0;

	//! Create a leading column cell.

	//! Called by create_cells() to figure out what it's doing.

	virtual list_cell create_leading_column(list_elementObj::implObj
						    &textlist_element,
						    size_t column_number)
		const=0;

	//! Create a trailing column cell.

	//! Called by create_cells() to figure out what it's doing.

	virtual list_cell create_trailing_column(list_elementObj::implObj
						     &textlist_element,
						     size_t column_number,
						     const textlist_rowinfo &)
		const=0;
};

//! Parameters to list_elementObj::implObj's constructor

//! Encapsulated in one place.

struct LIBCXX_HIDDEN list_element_impl_init_args {
	ref<listcontainer_pseudo_implObj> textlist_container;
	const new_listlayoutmanager &style;

	//! Which other columns we synchronize with

	//! This defaults to style.synchronized_columns. The list
	//! layout manager will provide a custom object here for
	//! an adjustable table.
	synchronized_axis synchronized_columns;
};

//! Additional parameters used by new_listlayoutmanager::create_impl

struct LIBCXX_HIDDEN new_listlayoutmanager::list_create_info {

	//! Create list_elementObj::implObj

	//! new_listlayoutmanager::create() provides a default implementation.
	//! new_tablelayoutmanager::create() provides an implementation that
	//! constructs a subclass.

	const function< ref<list_elementObj::implObj>
			(const list_element_impl_init_args &)
			> &create_list_element_impl;

	//! Create the list layout manager implementation object.


	//! new_listlayoutmanager::create() provides a default implementation
	//! that creates a listlayoutmanagerObj::implObj.
	//!
	//! new_tablelayoutmanager::create() provides an implementation that
	//! constructs a tablelayoutmanagerObj::implObj.
	const function< ref<listlayoutmanagerObj::implObj>
			(const ref<listcontainer_pseudo_implObj> &,
			 const list_element &)> &create_listlayoutmanager_impl;
};

LIBCXXW_NAMESPACE_END

#endif
