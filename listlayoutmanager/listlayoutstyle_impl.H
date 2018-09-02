/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutstyle_impl_h
#define x_w_textlistlayoutstyle_impl_h

#include "listlayoutmanager/listlayoutstyle_implfwd.H"
#include "listlayoutmanager/menu_item.H"
#include "listlayoutmanager/list_element_impl.H"
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

struct LIBCXX_HIDDEN textlist_rowinfo {
	const shortcut                         *listitem_shortcut=nullptr;
	const list_item_status_change_callback *listitem_callback=nullptr;

	menu_item_t menu_item;

	void setting_menu_item();
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
			  list_elementObj::implObj &textlist_element,

			  std::vector<list_cell> &newcells,
			  std::vector<textlist_rowinfo> &rowmeta)
		const;

	//! Callback indicating that a list item has changed.

	//! Does nothing for the highlighted list. For the bulleted list this
	//! updates the bullet image cell accordingly.
	virtual void selected_changed(list_cell *row,
				      bool selected_flag) const=0;
 private:

	//! Whether a menuoption or a submenu can be separated.

	virtual void menu_attribute_requested() const;

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

	//! Helper function for converting a list_item_param.

	//! Processes the next list_item_param. Invokes item_callback if
	//! it specifies new cell content. Invokes separator_callback for a
	//! separator.
	//!
	//! Otherwise updates next_rowinfo.

	template<typename Item_callback,
		 typename Separator_callback> void process_list_item_param
		(const list_item_param &item,
		 list_elementObj::implObj &textlist_element,
		 halign alignment,
		 textlist_rowinfo &next_rowinfo,
		 Item_callback &&item_callback,
		 Separator_callback &&separator_callback) const
	{
		do_process_list_item_param(item, textlist_element,
					   alignment,
					   next_rowinfo,
					   make_function
					   <void (const list_cell &)>
					   (std::forward<Item_callback>
					    (item_callback)),
					   make_function<void ()>
					   (std::forward<Separator_callback>
					    (separator_callback)));

	}

	//! Type-erased process_list_item_param().
	void do_process_list_item_param
		(const list_item_param::variant_t &,
		 list_elementObj::implObj &,
		 halign,
		 textlist_rowinfo &,
		 const function<void (const list_cell &)> &,
		 const function<void ()> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
