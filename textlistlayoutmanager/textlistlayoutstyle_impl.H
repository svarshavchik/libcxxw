/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutstyle_impl_h
#define x_w_textlistlayoutstyle_impl_h

#include "textlistlayoutmanager/textlistlayoutstyle_implfwd.H"
#include "textlistlayoutmanager/textlist_impl.H"
#include "layout_style.H"
#include "draw_infofwd.H"
#include "background_colorfwd.H"
#include "x/w/containerfwd.H"
#include "x/w/focusablefwd.H"
#include "x/w/text_paramfwd.H"
#include "focus/focusablefwd.H"
#include "connection_threadfwd.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! The \ref textlistlayoutmanager "textlistlayoutmanager" style.

//! Implements create(), and defines additional methods that specify the
//! list style.

class LIBCXX_HIDDEN textlistlayout_style_impl : public layout_style_t {

 public:

	//! Create a new list element.
	virtual std::tuple<container, peepholed, focusable, focusable_impl>
		create(const ref<peepholeObj::implObj> &,
		       const new_listlayoutmanager &) const
		override;

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

	virtual void set_selected_background(IN_THREAD_ONLY,
					     draw_info &di,
					     const background_color &bgcolor)
		const=0;

	//! Create internal cell elements based on the list_item_params.

	//! Mostly a straightforward conversion, for a highlighted list.
	//! The bulleted list style covertly inserts a bullet cell on every
	//! row.
	std::vector<textlist_cell>
		create_cells(const std::vector<list_item_param> &t,
			     textlistObj::implObj &textlist_element)
		const;

	//! Callback indicating that a list item has changed.

	//! Does nothing for the highlighted list. For the bulleted list this
	//! updates the bullet image cell accordingly.
	virtual void selected_changed(textlist_cell *row,
				      bool selected_flag) const=0;
 private:

	//! How many extra columns there are.

	//! Called by create_cells() to figure out what it's doing.
	virtual size_t extra_leading_columns() const=0;

	//! Create a leading column cell.

	//! Called by create_cells() to figure out what it's doing.

	virtual textlist_cell create_leading_column(textlistObj::implObj
						    &textlist_element,
						    size_t column_number)
		const=0;
};

LIBCXXW_NAMESPACE_END

#endif
