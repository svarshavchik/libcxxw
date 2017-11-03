/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef textlist_impl_h
#define textlist_impl_h

#include "textlistlayoutmanager/textlist.H"
#include "textlistlayoutmanager/textlistlayoutstyle_implfwd.H"
#include "textlistlayoutmanager/textlist_cellfwd.H"
#include "textlistlayoutmanager/extra_list_row_info.H"
#include "focus/focusable_elementfwd.H"
#include "background_color_elementfwd.H"
#include "iconfwd.H"
#include "current_border_implfwd.H"
#include "child_element.H"
#include "textlist_container_impl.H"
#include "x/w/alignment.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/textlistlayoutmanager.H"
#include "x/w/text_paramfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/types.H"
#include "richtext/richtextfwd.H"
#include "richtext/richtext_draw_boundariesfwd.H"
#include <x/mpobj.H>
#include <unordered_map>
#include <set>
#include <vector>
#include <optional>
#include <utility>

LIBCXXW_NAMESPACE_START

struct listcontainer_selected_color;
struct listcontainer_highlighted_color;
struct listcontainer_current_color;

//! Container that keeps track of minimum columns widths.

//! An entry for each text element in each row, for each column,
//! gets added to this container, making it easier to determine
//! the width of each column. The new element's iterator gets saved.
//! If the row is removed, each column's iterator gets removed from
//! this set.
//!
//! Using std::greater as the comparison results in the widest column
//! width being simply begin().

typedef std::multiset<dim_t, std::greater<dim_t>> list_column_widths_t;

//! Each row in a container with a \ref textlistlayoutmanager "textlistlayoutmanager".

struct LIBCXX_HIDDEN list_row_info_t {

	//! Height of this row.
	dim_t height;

	//! This row's position.
	coord_t y;

	//! Whether this row's height, and column widths, are computed.

	//! Each text's dimensions can only be obtained by the
	//! connection thread, so each new row gets inserted with the
	//! flag being cleared.
	//!
	//! This also indicates whether column_iterator was initialized.
	bool size_computed=false;

	//! Needs to be redrawn.

	//! This is set after enabled or selected is changed
	bool redraw_needed=false;

	//! Extra information about this row.

	//! Stored in discrete object, so that the object remains in place
	//! if rows are added or removed from the list.

	extra_list_row_info extra=extra_list_row_info::create();
};

//! A vector of list_row_info_t structures

//! Privately inherits from std::vector, and imports its methods publicly,
//! but overrides the methods that modify the vector to also set a modified
//! flag.
//!
//! This works in conjunction with textlist_info_lock to automatically
//! call recalculate() when the list is locked in the connection thread.

struct LIBCXX_HIDDEN list_row_info_vector :
	private std::vector<list_row_info_t> {

 public:
	using std::vector<list_row_info_t>::begin;
	using std::vector<list_row_info_t>::end;
	using std::vector<list_row_info_t>::at;
	using std::vector<list_row_info_t>::size;
	using std::vector<list_row_info_t>::empty;
	using std::vector<list_row_info_t>::reserve;

	//! This vector was modified.

	//! Instantiating a textlist_info_lock will invoke recalculate().

	bool modified=false;

	//! Vector insert(), and set the \c modified flag
	template<typename ...Args> auto inline insert(Args && ...args)
	{
		modified=true;
		return std::vector<list_row_info_t>::insert
			(std::forward<Args>(args)...);
	}

	//! Vector erase(), and set the \c modified flag
	template<typename ...Args> auto inline erase(Args && ...args)
	{
		modified=true;
		return std::vector<list_row_info_t>::erase
			(std::forward<Args>(args)...);
	}

	//! Vector clear(), and set the \c modified flag

	void clear()
	{
		modified=true;
		std::vector<list_row_info_t>::clear();
	}
};

//! Mutex-protected information stored in the \ref textlist "textlist".

//! Stored in \ref textlistObj::implObj "the implementation object", and
//! accessed by the \ref textlistlayoutmanager "textlistlayoutmanager".

struct LIBCXX_HIDDEN listimpl_info_s {

	//! Callback
	list_selection_type_cb_t selection_type;

	//! Callback
	list_selection_changed_cb_t selection_changed;

	//! The columns
	std::vector<list_column_widths_t> column_widths;

	//! The calculated column widths.

	//! begin()'s value, from each column_widths.

	std::vector<dim_t> calculated_column_widths;

	//! The sum total of calculated_column_widths

	dim_t total_column_width;

	//! Starting position and width of each column.

	//! Scaled to current element width, factoring in the padding.

	std::vector<std::pair<coord_t, dim_t>> columns_poswidths;

	//! The rows
	list_row_info_vector row_infos;

	//! The text, stored in row-major format.
	std::vector<textlist_cell> cells;

	//! Whether we need to redraw ourselves, after recalculating.
	bool full_redraw_needed=false;

	//! Whether we need to redraw because some item status changed.
	bool row_redraw_needed=false;
};

//! Internal implementation object.

class LIBCXX_HIDDEN textlistObj::implObj :
	public background_color_elementObj<focusable_elementObj
					   <child_elementObj>,
					   listcontainer_selected_color,
					   listcontainer_highlighted_color,
					   listcontainer_current_color> {

 public:

	//! Parent container, with a singleton layout manager.

	const ref<textlist_container_implObj> textlist_container;

	//! Alias
	typedef background_color_elementObj<focusable_elementObj
					   <child_elementObj>,
					   listcontainer_selected_color,
					   listcontainer_highlighted_color,
					   listcontainer_current_color
					    > superclass_t;

	//! Constructor
	implObj(const ref<textlist_container_implObj> &textlist_container,
		const new_listlayoutmanager &style,
		const textlistlayout_style_impl &list_style);

 private:
	//! Internal constructor

	implObj(const ref<textlist_container_implObj> &textlist_container,
		const new_listlayoutmanager &style,
		elementObj::implObj &container_element_impl,
		const textlistlayout_style_impl &list_style);

	//! Internal constructor

	implObj(const ref<textlist_container_implObj> &textlist_container,
		const new_listlayoutmanager &style,
		elementObj::implObj &container_element_impl,
		const screen &container_screen,
		const textlistlayout_style_impl &list_style);

 public:
	//! Destructor
	~implObj();

	//! Override initialize()
	void initialize(IN_THREAD_ONLY);

	//! Override theme_updated()
	void theme_updated(IN_THREAD_ONLY,
			   const defaulttheme &new_theme);

	//! Which list style we're supposed to use.

	const textlistlayout_style_impl &list_style;

	//! Number of columns in the list.
	const size_t columns;

	//! Number of visible rows in the list.
	const size_t rows;

	//! Relative width of columns.

	//! Specified in new_listlayoutmanager.

	const std::unordered_map<size_t, int> requested_col_widths;

	//! Alignment of all columns.

	//! Specified in new_listlayoutmanager.
	const std::unordered_map<size_t, halign> col_alignments;

	//! Remove a row.
	void remove_row(const textlistlayoutmanager &ll,
			size_t row_number);

	//! Add rows to the end.
	void append_rows(const textlistlayoutmanager &ll,
			 const std::vector<list_item_param> &items);

	//! Insert rows.
	void insert_rows(const textlistlayoutmanager &ll,
			 size_t row_number,
			 const std::vector<list_item_param> &items);

	//! Replace rows.
	void replace_rows(const textlistlayoutmanager &ll,
			  size_t row_number,
			  const std::vector<list_item_param> &items);

	//! Replace all rows.
	void replace_all_rows(const textlistlayoutmanager &ll,
			      const std::vector<list_item_param> &items);

	//! Recalculate after a theme update.

	void recalculate_with_new_theme(IN_THREAD_ONLY,
					listimpl_info_t::lock &lock);

	//! Recalculate our metrics, and everything.

	//! Invoked from textlistlayoutmanagerObj::implObj.
	void recalculate(IN_THREAD_ONLY);

	//! Override process_updated_position().

	//! Calls recalculate().
	void process_updated_position(IN_THREAD_ONLY) override;

	//! Override do_draw.

	//! Draw ourselves.
	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas) override;

	//! Handle motion events.

	void report_motion_event(IN_THREAD_ONLY, const motion_event &) override;

	//! Handle keyboard events.

	bool process_key_event(IN_THREAD_ONLY, const key_event &ke) override;

	//! Handle button events.
	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Handle pointer focus events.
	void pointer_focus(IN_THREAD_ONLY) override;

	//! Handle keyboard focus events.
	void keyboard_focus(IN_THREAD_ONLY) override;

	//! Invoked from the layout manager.
	void autoselect(const textlistlayoutmanager &, size_t,
		      const callback_trigger_t &);

	//! Invoked from the layout manager.
	void selected(const textlistlayoutmanager &, size_t, bool,
		      const callback_trigger_t &);

	//! Number of items in the list

	size_t size();

	//! Whether list item \#i is selected right now.
	bool selected(size_t i);

	//! Return the currently selected item.

	//! For convenience of a single_selection_type() list, returns
	//! the index of the currently selected item.

	std::optional<size_t> selected();

	//! Return a list of all selected items.
	std::vector<size_t> all_selected();

	//! Unselect all().
	void unselect(const textlistlayoutmanager &);

	//! Whether the given list item is enabled().

	bool enabled(size_t i);

	//! Enable or disable the list item.

	void enabled(size_t i, bool flag);

 private:
	//! Redraw only those rows whose status has changed.

	void redraw_needed_rows(IN_THREAD_ONLY);

	//! Redraw all visible rows.
	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas,
		     bool only_whats_needed);

	//! Unselect all rows, after acquiring a lock.

	//! Returns true if at least one row was unselected, and
	//! schedule_row_redraw() must be called.
	//!
	//! unselect() is also called from remove_rows(), which is going
	//! to nuke everything from high orbit, and doesn't need to call
	//! schedule_row_redraw().

	bool unselect(const textlistlayoutmanager &,
		      list_lock &);

 public:
	//! Our text data.
	listimpl_info_t textlist_info;

 private:

	//! A mutex lock on textlist_info

	//! listimpl_info_t is an mpobj. This is a subclass of
	//! listimpl_info_t::lock that acquires the lock, then checks
	//! if list_row_info_vector.modified if set, then if so calls
	//! recalculate().

	struct textlist_info_lock;
 public:

	//! Scratch buffer for drawing separators
	const scratch_buffer scratch_buffer_for_separator;

	//! Border that represents the separator
	const current_border_impl separator_border;

	//! Precreate an icon that we might need.
	const icon bullet1;

	//! Precreate an icon that we might need.
	const icon bullet2;

 private:
	//! Schedule running redraw_needed_rows() in the connection thread.

	void schedule_row_redraw(listimpl_info_t::lock &lock);

	//! Current list element (element under pointer, or tabbed to)
	std::optional<size_t> current_element_under_lock;

	//! Current list element that was navigated to via keyboard
	std::optional<size_t> current_keyed_element_under_lock;

	//! Whether the enter/space key or button 1 is down.
	bool is_key_or_button_down=false;

	//! This can be accessed only while holding the lock.
	inline auto &current_element(const listimpl_info_t::lock &lock)
	{
		return current_element_under_lock;
	}

	//! This can be accessed only while holding the lock.
	inline auto &current_keyed_element(const listimpl_info_t::lock &lock)
	{
		return current_keyed_element_under_lock;
	}

	//! Insert rows.
	void insert_rows(const textlistlayoutmanager &ll,
			 list_lock &lock,
			 size_t row_number,
			 const std::vector<textlist_cell> &texts);

	//! Remove the given rows.

	void remove_rows(const textlistlayoutmanager &ll,
			 listimpl_info_t::lock &lock,
			 size_t row_number,
			 size_t count);

	//! The given rows are about to be removed.

	//! Unlink the rows' cells from column_widths

	void removing_rows(const textlistlayoutmanager &ll,
			   listimpl_info_t::lock &lock,
			   size_t row,
			   size_t count);

	void recalculate(IN_THREAD_ONLY, listimpl_info_t::lock &lock);

	//! Compute calculate_column_widths and total_column_width

	void calculate_column_widths(IN_THREAD_ONLY,
				     listimpl_info_t::lock &lock);

	//! Compute column_poswidths.

	//! Based on calculated_column_widths and padding. Returns total
	//! width.
	void calculate_column_poswidths(IN_THREAD_ONLY, listimpl_info_t::lock
					&lock);

	//! Draw an individual row.
	rectangle do_draw_row(IN_THREAD_ONLY,
			      const draw_info &di,
			      richtext_draw_boundaries &bounds,
			      listimpl_info_t::lock &lock,
			      size_t row_number,
			      bool make_sure_row_is_visible);

	//! Draw an individual row.
	rectangle do_draw_row(IN_THREAD_ONLY,
			      const draw_info &di,
			      richtext_draw_boundaries &bounds,
			      listimpl_info_t::lock &lock,
			      size_t row_number,
			      const list_row_info_t &r,
			      bool make_sure_row_is_visible);

	//! Explicitly redraw one row.

	void redraw_rows(IN_THREAD_ONLY,
			 listimpl_info_t::lock &lock,
			 size_t row_number1);

	//! Explicitly redraw two row.

	void redraw_rows(IN_THREAD_ONLY,
			 listimpl_info_t::lock &lock,
			 size_t row_number1,
			 size_t row_number2,
			 bool make_sure_row2_is_visible);

	//! Nothing is selected

	//! Pointer moved out of the window/lost keyboard focus, etc...
	//! Unset current_element.

	void unset_current_element(IN_THREAD_ONLY,
				   listimpl_info_t::lock &lock);

	//! Set current element.

	void set_current_element(IN_THREAD_ONLY,
				 listimpl_info_t::lock &lock,
				 size_t row_number,
				 bool make_sure_row_is_visible);

	//! Process keyboard event.

	bool process_key_event(IN_THREAD_ONLY, const key_event &ke,
			       listimpl_info_t::lock &lock);

	//! Clicked on the list item.
	void click(IN_THREAD_ONLY, listimpl_info_t::lock &lock,
		   const callback_trigger_t &trigger);

	//! Advance currently selected item.
	std::optional<size_t> move_up_by(listimpl_info_t::lock &lock,
					 size_t howmuch);

	//! Advance currently selected item.
	std::optional<size_t> move_down_by(listimpl_info_t::lock &lock,
					   size_t howmuch);

	//! Invoke callbacks after list item's selection status changed.
	void notify_callbacks(const textlistlayoutmanager &,
			      list_lock &,
			      const list_row_info_t &,
			      size_t,
			      bool,
			      const callback_trigger_t &);
};

LIBCXXW_NAMESPACE_END

#endif
