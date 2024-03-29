/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef list_element_impl_h
#define list_element_impl_h

#include "listlayoutmanager/list_element.H"
#include "listlayoutmanager/listlayoutstyle_implfwd.H"
#include "listlayoutmanager/list_cellfwd.H"
#include "listlayoutmanager/extra_list_row_infofwd.H"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "x/w/impl/focus/focusable_elementfwd.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/impl/themedim_elementfwd.H"
#include "x/w/impl/iconfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/themeborder_elementfwd.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/alignment.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/text_paramfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/types.H"
#include "x/w/metrics/axis.H"
#include "synchronized_axis_impl.H"
#include "synchronized_axis_valuefwd.H"
#include "x/w/richtext/richtextfwd.H"
#include "x/w/impl/richtext/richtext_draw_boundariesfwd.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "richtext/richtext_alteration_config.H"
#include <x/mpobj.H>
#include <x/derivedvaluefwd.H>
#include <unordered_map>
#include <set>
#include <vector>
#include <optional>
#include <utility>
#include <tuple>

LIBCXXW_NAMESPACE_START

struct listcontainer_selected_color;
struct listcontainer_highlighted_color;
struct listcontainer_current_color;
struct listcontainer_dim_v;
struct listcontainer_dim_h;
struct listcontainer_indent;

struct list_font_tag;
struct shortcut_font_tag;

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

//! Each row in a container element that uses a \ref listlayoutmanager "listlayoutmanager".

//! \see list_element

struct list_row_info_t {

	//! Constructor
	list_row_info_t(const std::tuple<extra_list_row_info,
			textlist_rowinfo> &rowmeta);
	//! Destructor
	~list_row_info_t();

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

	//! This item's indentation level
	size_t indent=0;

	//! Extra information about this row.

	//! Stored in discrete object, so that the object remains in place
	//! if rows are added or removed from the list.

	extra_list_row_info extra;
};

//! A vector of list_row_info_t structures

//! Privately inherits from std::vector, and imports its methods publicly,
//! but overrides the methods that modify the vector to also set the \c modified
//! flag.
//!
//! This works in conjunction with textlist_info_lock to automatically
//! call recalculate() when the list gets locked in the connection thread.

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

	//! Instantiating a textlist_info_lock invokes recalculate().

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

//! Mutex-protected information stored in the \ref list_element "list_element".

//! Stored in \ref list_elementObj::implObj "the implementation object", and
//! accessed by the \ref listlayoutmanager "listlayoutmanager".

struct listimpl_info_s {

	//! Callback
	list_selection_type_cb_t selection_type;

	//! Callback
	list_selection_changed_cb_t selection_changed;

	//! The columns
	std::vector<list_column_widths_t> list_column_widths;

	//! The calculated column widths.

	//! begin()'s value, from each column_widths.

	std::vector<dim_t> calculated_column_widths;

	//! Starting position and width of each column.

	//! Scaled to current element width, factoring in the padding.

	std::vector<std::tuple<coord_t, dim_t>> columns_poswidths;

	//! Starting position and width of any borders

	std::unordered_map<size_t, std::tuple<coord_t, dim_t>> border_positions;

	//! The rows
	list_row_info_vector row_infos;

	//! The text, stored in row-major format.
	std::vector<list_cell> cells;

	//! Whether we need to redraw ourselves, after recalculating.
	bool full_redraw_needed=false;
};

//! list_elementObj superclass

//! The list_elementObj::implObj uses the following mixins:
//!
//! - Three background colors, for selected, highlighted, and current items
//!
//! - Default vertical and horizontal paddings, and indentation level.
//!
//! - The border for the list item separator element.
//!
//! - It is a focusable element.

typedef background_color_elementObj<themedim_elementObj<themeborder_elementObj
							<focusable_elementObj
							 <child_elementObj>>,
							listcontainer_dim_h,
							listcontainer_dim_v,
							listcontainer_indent>,
				    listcontainer_selected_color,
				    listcontainer_highlighted_color,
				    listcontainer_current_color
				    > list_element_impl_superclass_t;

struct list_element_impl_init_args;

//! Internal implementation object.

//! \see list_element

class list_elementObj::implObj :
	public list_element_impl_superclass_t,
	public richtext_alteration_config {

public:

	//! Parent container, with a singleton layout manager.

	const ref<listcontainer_pseudo_implObj> textlist_container;

	//! Alias
	typedef list_element_impl_superclass_t superclass_t;

	//! Constructor
	implObj(const list_element_impl_init_args &init_args);

private:
	//! Internal constructor

	implObj(const list_element_impl_init_args &init_args,
		elementObj::implObj &container_element_impl);

	//! Internal constructor

	implObj(const list_element_impl_init_args &init_args,
		elementObj::implObj &container_element_impl,
		const screen &container_screen);

	//! Cell borders, in millimeters
	const std::unordered_map<size_t, std::tuple<dim_arg, dim_arg>
				 > lr_paddings;

	//! Cell borders, in pixels
	std::unordered_map<size_t, std::tuple<dim_t, dim_t>
			   > lr_padding_pixels_thread_only;
	THREAD_DATA_ONLY(lr_padding_pixels);

	//! Tallest row height

	//! Statistics kept by recalculate().
	tallest_row_height_t tallest_row_height_thread_only;

public:
	THREAD_DATA_ONLY(tallest_row_height);

	//! Destructor
	~implObj();

	//! Override removed_from_container
	void removed_from_container(ONLY IN_THREAD) override;

	//! Override initialize()
	void initialize(ONLY IN_THREAD) override;

	//! Override theme_updated()
	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;

	//! Which list style we're supposed to use.

	const listlayoutstyle_impl &list_style;

	//! Number of columns in the list.
	const size_t columns;

	//! Relative width of columns.

	//! Specified in new_listlayoutmanager.

	const std::unordered_map<size_t, int> requested_col_widths;

	//! Alignment of all columns.

	//! Specified in new_listlayoutmanager.
	const std::unordered_map<size_t, halign> col_alignments;

	//! Alignment of all columns.

	//! Specified in new_listlayoutmanager.
	const std::unordered_map<size_t, valign> row_alignments;

	//! Borders

	//! Specified in new_listlayoutmanager.
	const std::unordered_map<size_t, current_border_impl> column_borders;

private:
	//! Synchronization info

	my_synchronized_axis synchronized_info;

	//! Minimum column widths, in pixels
	std::unordered_map<size_t, std::tuple<double, dim_t>
			   > minimum_column_widths_thread_only;

	//! Recalculate minimum_column_width_pixels

	void recalculate_minimum_column_width_pixels(ONLY IN_THREAD,
						     const const_defaulttheme &)
		;

public:
	THREAD_DATA_ONLY(minimum_column_widths);

	//! Remove a row.
	void remove_rows(ONLY IN_THREAD,
			 const listlayoutmanager &ll,
			 size_t row_number,
			 size_t n_items);

	//! Add rows to the end.
	void append_rows(ONLY IN_THREAD,
			 const listlayoutmanager &lm,
			 new_cells_info &info);

	//! Insert rows.
	void insert_rows(ONLY IN_THREAD,
			 const listlayoutmanager &ll,
			 size_t row_number,
			 new_cells_info &info);

	//! Replace rows.
	void replace_rows(ONLY IN_THREAD,
			  const listlayoutmanager &ll,
			  size_t row_number,
			  new_cells_info &info);

	//! Replace all rows.
	void replace_all_rows(ONLY IN_THREAD,
			      const listlayoutmanager &ll,
			      new_cells_info &info);

	//! Resort rows.
	void resort_rows(ONLY IN_THREAD,
			 const listlayoutmanager &lm,
			 std::vector<size_t> &indexes);

	//! Recalculate after a theme update.

	void recalculate_with_new_theme(ONLY IN_THREAD,
					textlist_info_lock &lock);

	//! Recalculate our metrics, and everything.

	//! Invoked from listlayoutmanagerObj::implObj.
	void recalculate(ONLY IN_THREAD);

	//! New metrics computed, so set_element_metrics().

	//! Overridden by the table layout manager.

	virtual void update_metrics(ONLY IN_THREAD,
				    const metrics::axis &new_horiz,
				    const metrics::axis &new_vert);

	//! Override process_updated_position().

	//! Calls recalculate().
	void process_updated_position(ONLY IN_THREAD) override;

	//! Override do_draw.

	//! Draw ourselves.
	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;

	//! Handle motion events.

	void report_motion_event(ONLY IN_THREAD, const motion_event &) override;

	//! Handle keyboard events.

	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

	//! Handle button events.
	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Handle pointer focus events.
	void pointer_focus(ONLY IN_THREAD,
			   const callback_trigger_t &trigger) override;

	//! Handle keyboard focus events.
	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger) override;

	//! Invoked from the layout manager.
	void autoselect(ONLY IN_THREAD,
			const listlayoutmanager &, size_t,
			const callback_trigger_t &);

	//! Invoked from the layout manager.
	void selected(ONLY IN_THREAD,
		      const listlayoutmanager &, size_t, bool,
		      const callback_trigger_t &);

	//! Invoked from the layout manager.
	void menuitem_selected(ONLY IN_THREAD,
			       const listlayoutmanager &, size_t,
			       const callback_trigger_t &,
			       const busy &);
private:

	//! Common internal selection processing.

	//! \internal
	void selected_common(ONLY IN_THREAD,
			     const listlayoutmanager &,
			     list_lock &,
			     listimpl_info_t::lock &,
			     size_t,
			     bool,
			     const callback_trigger_t &);

	void selected_changed(listimpl_info_t::lock &lock, size_t row,
			      bool flag);

public:
	//! Number of items in the list

	size_t size();

	//! Whether list item \#i is selected right now.
	bool selected(size_t i);

	//! Return list item \#i's hierarchical indentation level.
	size_t hierindent(size_t i);

	//! Return the currently selected item.

	//! For convenience of a single_selection_type() list, returns
	//! the index of the currently selected item.

	std::optional<size_t> selected();

	//! Return a list of all selected items.
	std::vector<size_t> all_selected();

	//! Unselect all().
	void unselect(THREAD_CALLBACK,
		      const listlayoutmanager &);

	//! Whether the given list item is enabled().

	bool enabled(size_t i);

	//! Enable or disable the list item.

	void enabled(ONLY IN_THREAD, size_t i, bool flag);

	//! Enable or disable the list item.

	void enabled(ONLY IN_THREAD,
		     const extra_list_row_info &extra,
		     bool flag);

private:
	using list_element_impl_superclass_t::enabled;

	//! Enable or disable the list item.

	void enabled(ONLY IN_THREAD,
		     textlist_info_lock &lock,
		     const extra_list_row_info &extra,
		     bool flag);
public:
	//! Install a new on_status_update callback.

	void on_status_update(ONLY IN_THREAD,
			      const listlayoutmanager &lm,
			      const extra_list_row_info &extra,
			      const list_item_status_change_callback &);

	//! Install a new on_status_update callback.

	void on_status_update(ONLY IN_THREAD,
			      const listlayoutmanager &lm,
			      size_t i,
			      const list_item_status_change_callback &);

	//! Install a new on_status_update callback.
	void on_status_update(ONLY IN_THREAD,
			      const listlayoutmanager &lm,
			      list_lock &ll,
			      textlist_info_lock &lock,
			      size_t i,
			      const list_item_status_change_callback &);

private:

	//! Redraw all visible rows.
	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas,
		     textlist_info_lock &lock);

	//! Unselect all rows, after acquiring a lock.

	void unselect(THREAD_CALLBACK,
		      const listlayoutmanager &,
		      list_lock &);

public:
	//! Our text data.
	listimpl_info_t textlist_info;

	friend class extra_list_row_infoObj;
	friend struct textlist_info_lock;

	//! Scratch buffer for drawing separators
	const scratch_buffer scratch_buffer_for_separator;

	//! Scratch buffer for drawing borders
	const scratch_buffer scratch_buffer_for_borders;

	//! Precreate an icon that we might need.
	const icon bullet1;

	//! Precreate an icon that we might need.
	const icon bullet2;

	//! Precreate an icon that we might need.
	const icon submenu;

	//! Default label metadata for list item element.
	const richtextmeta itemlabel_meta;

	//! Default metadata for list element.
	const richtextmeta itemshortcut_meta;

 private:
	//! Callback when the current list item changes.
	list_item_status_change_callbackptr current_list_item_changed_thread_only;
public:
	THREAD_DATA_ONLY(current_list_item_changed);

	//! Return the vertical padding of each row.
	dim_t list_v_padding(ONLY IN_THREAD) const;

	//! Return the left and the right padding of the given column
	std::tuple<dim_t, dim_t> get_paddings(ONLY IN_THREAD, size_t) const;

	//! Schedule running redraw_needed_rows() in the connection thread.

	void schedule_row_redraw(listimpl_info_t::lock &lock);

	//! Current list element (element under pointer, or tabbed to)
	std::optional<size_t> current_element_under_lock;

	//! What the current list element should be.

	//! In cases where the current element is updated not IN_THREAD,
	//! only this is set, and recalculate() will double-check this.
	std::optional<size_t> should_be_current_element_under_lock;


	//! Whether the most recent pointer or keyboard focus was gain or loss

	//! This is used to determine whether the "current" list item is
	//! highlighted.

	bool combined_focus_thread_only=false;

	THREAD_DATA_ONLY(combined_focus);

	//! Whether the enter/space key or button 1 is down.
	bool is_key_or_button_down=false;

public:
	//! This can be accessed only while holding the lock.
	inline const auto &current_element(const listimpl_info_t::lock &lock)
	{
		return current_element_under_lock;
	}

	//! This can be accessed only while holding the lock.
	inline auto &should_be_current_element
		(const listimpl_info_t::lock &lock)
	{
		return should_be_current_element_under_lock;
	}

	//! Check whether should_be_current_element() actually is.

	//! If not, invoke current_element().
	void should_be_current_element_actually_is(ONLY IN_THREAD,
						   const listimpl_info_t::lock
						   &lock);

	//! This can be accessed only while holding the lock.
	template<typename V>
		inline void current_element(ONLY IN_THREAD,
					    const listimpl_info_t::lock &lock,
					    V &&v,
					    const callback_trigger_t &trigger)
	{
		auto previous=current_element_under_lock;

		should_be_current_element_under_lock=
			current_element_under_lock=std::forward<V>(v);

		report_new_current_element(IN_THREAD,
					   lock,
					   previous,
					   current_element_under_lock,
					   trigger);
	}

private:
	//! Currently highlighted list item changed.

	void report_new_current_element(ONLY IN_THREAD,
					const listimpl_info_t::lock &lock,
					const std::optional<size_t> &original,
					const std::optional<size_t> &current,
					const callback_trigger_t &trigger);

	//! Insert rows.
	void insert_rows(ONLY IN_THREAD,
			 const listlayoutmanager &ll,
			 list_lock &lock,
			 size_t row_number,
			 new_cells_info &info);

	//! Remove the given rows.

	void remove_rows(ONLY IN_THREAD,
			 const listlayoutmanager &ll,
			 listimpl_info_t::lock &lock,
			 size_t row_number,
			 size_t count);

	//! The given rows are about to be removed.

	//! Unlink the rows' cells from column_widths

	void removing_rows(ONLY IN_THREAD,
			   const listlayoutmanager &ll,
			   listimpl_info_t::lock &lock,
			   size_t row,
			   size_t count);

	void recalculate(ONLY IN_THREAD, textlist_info_lock &lock);

	//! Compute calculate_column_widths and total_column_width

	void calculate_column_widths(ONLY IN_THREAD,
				     listimpl_info_t::lock &lock);

	//! Compute column_poswidths.

	//! Based on calculated_column_widths and padding. Returns total
	//! width.
	dim_t calculate_column_poswidths(ONLY IN_THREAD, listimpl_info_t::lock
					 &lock);

	//! Draw an individual row.
	rectangle do_draw_row(ONLY IN_THREAD,
			      const draw_info &di,
			      clip_region_set &clipped,
			      richtext_draw_boundaries &bounds,
			      listimpl_info_t::lock &lock,
			      size_t row_number);

	//! Draw an individual row.
	rectangle do_draw_row(ONLY IN_THREAD,
			      const draw_info &di,
			      clip_region_set &clipped,
			      richtext_draw_boundaries &bounds,
			      listimpl_info_t::lock &lock,
			      size_t row_number,
			      const list_row_info_t &r);

	// Draw the borders in this row.

	void do_draw_row_borders(ONLY IN_THREAD,
				 const draw_info &di,
				 clip_region_set &clipped,
				 listimpl_info_t::lock &lock,
				 const rectangle &entire_row,
				 rectarea &drawn_area);

	//! Explicitly redraw one row.

	void redraw_rows(ONLY IN_THREAD,
			 listimpl_info_t::lock &lock,
			 size_t row_number1);

	//! Explicitly redraw two row.

	void redraw_rows(ONLY IN_THREAD,
			 listimpl_info_t::lock &lock,
			 size_t row_number1,
			 size_t row_number2,
			 bool make_sure_row2_is_visible);

	//! Nothing is selected

	//! Pointer moved out of the window/lost keyboard focus, etc...
	//! Unset current_element.

	void unset_current_element(ONLY IN_THREAD,
				   listimpl_info_t::lock &lock,
				   const callback_trigger_t &trigger);

	//! Set current element.

	void set_current_element(ONLY IN_THREAD,
				 listimpl_info_t::lock &lock,
				 size_t row_number,
				 bool make_sure_row_is_visible,
				 const callback_trigger_t &trigger);

	//! Process keyboard event.

	bool process_key_event(ONLY IN_THREAD, const key_event &ke,
			       listimpl_info_t::lock &lock);

	//! Clicked on the list item.
	void click(ONLY IN_THREAD, listimpl_info_t::lock &lock,
		   const callback_trigger_t &trigger);

	//! Advance currently selected item.
	std::optional<size_t> move_up_by(listimpl_info_t::lock &lock,
					 dim_t howmuch);

	//! Advance currently selected item.
	std::optional<size_t> move_down_by(listimpl_info_t::lock &lock,
					   dim_t howmuch);

	//! Invoke callbacks after list item's selection status changed.
	void notify_callbacks(ONLY IN_THREAD,
			      const listlayoutmanager &,
			      list_lock &,
			      const list_row_info_t &,
			      size_t,
			      bool,
			      const callback_trigger_t &);

	//! Invoke callbacks after list item's selection status changed.
	void notify_callbacks(ONLY IN_THREAD,
			      const listlayoutmanager &,
			      list_lock &,
			      const list_row_info_t &,
			      size_t,
			      bool,
			      const callback_trigger_t &,
			      const busy &);
public:

	//! Override hover_action_delay.

	//! Checks if the currently highlighted list item has a popup.
	std::chrono::milliseconds hover_action_delay(ONLY IN_THREAD) override;

	//! Override hover_action().

	//! Display the popup for the currently highlighted list item.
	void hover_action(ONLY IN_THREAD) override;

	//! Return an existing menu item's submenu layoutmanager.
	listlayoutmanagerptr submenu_listlayout(size_t i);
};

//! Internal lock on textlist_info

//! listimpl_info_t is an mpobj. A lock must get acquired first,
//! then used to instantiate th textlist_info_lock. This checks
//! if list_row_info_vector.modified if set, then if so calls
//! recalculate().

struct textlist_info_lock {

public:

	listimpl_info_t::lock &lock;

	// Whether the list was modified, before we were constructed.

	const bool was_modified;

	// The construct checks row_infos.modified, and if so calls
	// recalculate().

	textlist_info_lock(ONLY IN_THREAD,
			   listimpl_info_t::lock &lock,
			   list_elementObj::implObj &me);

	//! Destructor

	~textlist_info_lock();

	//! Convenience function
	inline auto operator->() noexcept
	{
		return lock.operator->();
	}

	//! Convenience function
	inline auto operator->() const noexcept
	{
		return lock.operator->();
	}
};

//! If the show starts without a lock in place, create one.

struct create_textlist_info_lock
	: public listimpl_info_t::lock,
	  public textlist_info_lock {

	//! Constructor
	create_textlist_info_lock(ONLY IN_THREAD,
				  list_elementObj::implObj &me);

	//! Destructor
	~create_textlist_info_lock();

	using listimpl_info_t::lock::operator->;
};

LIBCXXW_NAMESPACE_END

#endif
