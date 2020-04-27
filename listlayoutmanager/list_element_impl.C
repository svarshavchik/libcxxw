/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/list_cell.H"
#include "listlayoutmanager/extra_list_row_info.H"
#include "listlayoutmanager/listitemhandle_impl.H"
#include "popup/popup.H"
#include "shared_handler_data.H"
#include "x/w/generic_window_appearance.H"
#include "x/w/impl/focus/focusable_element.H"
#include "x/w/impl/background_color_element.H"
#include "x/w/impl/themedim_element.H"
#include "x/w/impl/themeborder_element.H"
#include "x/w/impl/border_impl.H"
#include "x/w/impl/icon.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtext_draw_boundaries.H"
#include "calculate_borders.H"
#include "x/w/motion_event.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/scratch_buffer.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "messages.H"
#include "x/w/impl/themedim.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/draw_info.H"
#include "busy.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include "defaulttheme.H"
#include "synchronized_axis_value.H"
#include "ellipsiscache.H"
#include <algorithm>
#include <x/algorithm.H>
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

static property::value<unsigned>
listitempopup_delay(LIBCXX_NAMESPACE_STR "::w::listitempopup_delay", 500);

const_list_lock::const_list_lock(const listlayoutmanagerObj &manager)
	: listimpl_info_lock_t{manager.impl->list_element_singleton->impl
		->textlist_info},
	  locked_layoutmanager{&manager}
{
}

const_list_lock::~const_list_lock()=default;

list_lock::list_lock(listlayoutmanagerObj &manager)
	: const_list_lock{manager},
	  locked_layoutmanager{&manager}
{
}

list_lock::~list_lock()=default;

////////////////////////////////////////////////////////////////////////////

// The connection thread uses this to lock the list information. This
// enforces that recalculate() must be called if the list was modified,
// before working with it.
//
// This is not bulletproof, but should be good enough. We need to make sure
// that IN_THREAD we always instantiate this textlist_info_lock, instead of
// listimpl_info_t::lock.


// The construct checks row_infos.modified, and if so calls
// recalculate().

list_elementObj::implObj::textlist_info_lock
::textlist_info_lock(ONLY IN_THREAD,
		     listimpl_info_t::lock &lock,
		     implObj &me)
	: lock{lock},
	  was_modified{ lock->row_infos.modified}
{
	if (was_modified)
		me.recalculate(IN_THREAD, lock);
}

list_elementObj::implObj::textlist_info_lock::~textlist_info_lock()=default;

list_elementObj::implObj::create_textlist_info_lock
::create_textlist_info_lock(ONLY IN_THREAD, implObj &me)
	: listimpl_info_t::lock{me.textlist_info},
	  textlist_info_lock{IN_THREAD, *this, me}
{
}

list_elementObj::implObj::create_textlist_info_lock
::~create_textlist_info_lock()=default;

////////////////////////////////////////////////////////////////////////////
//
// synchronized_axis are used to synchronized the columns widths.
//
// Implement synchronized_axis_valueObj::synchronized_axis_updated() to
// schedule recalculate(), which will take it into consideration.

class LIBCXX_HIDDEN list_element_synchronized_columnsObj
	: public synchronized_axis_valueObj {

 public:

	const container_impl parent_container;

	list_element_synchronized_columnsObj
		(const container_impl &parent_container)
		: parent_container{parent_container}
	{
	}

 public:

	void synchronized_axis_updated(ONLY IN_THREAD,
				       const synchronized_axis_values_t &)
		override
	{
		// Trigger a call to recalculate(), whihc calls
		// calculate_column_poswidths, which will grab the
		// updated metrics.
		parent_container->tell_layout_manager_it_needs_recalculation
			(IN_THREAD);
	}
};

////////////////////////////////////////////////////////////////////////////

static inline auto create_column_borders(elementObj::implObj &e,
					 const new_listlayoutmanager &style)
{
	std::unordered_map<size_t, current_border_impl> column_borders;

	for (const auto &cb:style.column_borders)
	{
		if (cb.first < 1 || cb.first >= style.columns)
		{
			throw EXCEPTION(_("Border numbers must be between 1 "
					  "and one less than the number of "
					  "columns in the list."));
		}
		column_borders.emplace(cb.first, e.create_border(cb.second));
	}

	return column_borders;
}

list_row_info_t::list_row_info_t(const std::tuple<extra_list_row_info,
				 textlist_rowinfo> &rowmeta)
	: extra{std::get<0>(rowmeta)}
{
}


list_row_info_t::~list_row_info_t()=default;

list_elementObj::implObj::implObj(const list_element_impl_init_args &init_args)
	: implObj{init_args,
		  init_args.textlist_container->get_element_impl()}
{
}

list_elementObj::implObj::implObj(const list_element_impl_init_args &init_args,
				  elementObj::implObj &container_element_impl)
	: implObj{init_args,
		  container_element_impl,
		  container_element_impl.get_screen()}
{
}

// Validate column width values.

static std::unordered_map<size_t, int> &&
validate_col_widths(std::unordered_map<size_t,
		    int> &&requested_col_widths)
{
	for (auto &cw:requested_col_widths)
	{
		if (cw.second < 0)
			cw.second=0;

		if (cw.second > 100)
			cw.second=100;
	}

	return std::move(requested_col_widths);
}

// Initial minimum column widths

static auto initial_minimum_column_width_pixels
(const std::unordered_map<size_t, double> &initial_minimum_column_width,
 const const_defaulttheme &theme)
{
	std::unordered_map<size_t, std::tuple<double, dim_t>> m;

	for (const auto &v:initial_minimum_column_width)
		m.emplace(v.first, std::tuple{v.second, theme->compute_width
						      (v.second)});


	return m;
}

inline dim_t list_elementObj::implObj::list_v_padding(ONLY IN_THREAD) const
{
	return themedim_element<listcontainer_dim_v>::pixels(IN_THREAD);
}

std::tuple<dim_t, dim_t> list_elementObj::implObj
::get_paddings(ONLY IN_THREAD, size_t n) const
{
	auto default_value=themedim_element<listcontainer_dim_h>
		::pixels(IN_THREAD);

	auto iter=lr_padding_pixels(IN_THREAD).find(n);

	if (iter == lr_padding_pixels(IN_THREAD).end())
		return {default_value, default_value};

	return iter->second;
}

// Recompute minimum_column_width_pixels after a theme change.

// This is called before recursively invoking theme_updated (and initialize).
// The container_element superclass calls the list layout manager's
// recalculate() after we already did this.

void list_elementObj::implObj
::recalculate_minimum_column_width_pixels(ONLY IN_THREAD,
					  const const_defaulttheme &theme)
{
	// First, recompute padding pixels
	for (const auto &dim_args:lr_paddings)
	{
		auto &[l,r]=dim_args.second;

		auto lp=theme->get_theme_dim_t(l, themedimaxis::width);
		auto rp=theme->get_theme_dim_t(r, themedimaxis::width);

		lr_padding_pixels(IN_THREAD)[dim_args.first]={lp, rp};
	}

	// And the minimum column widths
	for (auto &v:minimum_column_widths(IN_THREAD))
	{
		auto &[mm, pixels]=v.second;

		pixels=theme->compute_width(mm);
	}
}

list_elementObj::implObj::implObj(const list_element_impl_init_args &init_args,
				  elementObj::implObj &container_element_impl,
				  const screen &container_screen)
	: superclass_t{init_args.style.appearance->selected_color,
		       init_args.style.appearance->highlighted_color,
		       init_args.style.appearance->current_color,
		       init_args.style.appearance->h_padding,
		       themedimaxis::width,
		       init_args.style.appearance->v_padding,
		       themedimaxis::height,
		       init_args.style.appearance->indent,
		       themedimaxis::width,
		       init_args.style.appearance->list_separator_border,
		       init_args.textlist_container},
	  richtext_alteration_config{
				     container_element_impl.get_window_handler()
				     .get_screen()->impl->ellipsiscaches
				     ->get(container_element_impl)
	  },
	  textlist_container{init_args.textlist_container},
	  lr_paddings{init_args.style.lr_paddings},
	  list_style{init_args.style.list_style},
	  columns{list_style.actual_columns(init_args.style)},
	  requested_col_widths{validate_col_widths
			       (list_style.actual_col_widths(init_args.style))},
	  col_alignments{list_style.actual_col_alignments(init_args.style)},
	  row_alignments{list_style.actual_row_alignments(init_args.style)},
	  column_borders{create_column_borders(textlist_container
					       ->container_element_impl(),
					       init_args.style)},
	  synchronized_info{init_args.synchronized_columns,
			    ref<list_element_synchronized_columnsObj>::create
			    (textlist_container)},
	  minimum_column_widths_thread_only{initial_minimum_column_width_pixels
					    (init_args.style
					     .minimum_column_widths,
					     container_screen->impl
					     ->current_theme.get())},
	  textlist_info{listimpl_info_s{init_args.style.selection_type,
					init_args.style.selection_changed}},
	  scratch_buffer_for_separator{container_screen->create_scratch_buffer
				       ("list_separator_scratch@libcxx.com",
					container_screen
					->find_alpha_pictformat_by_depth(1))},
	  scratch_buffer_for_borders{container_screen->create_scratch_buffer
				     ("list_border_scratch@libcxx.com",
				      container_screen
				      ->find_alpha_pictformat_by_depth(1))},
	  bullet1{container_element_impl.get_window_handler()
		  .create_icon({init_args.style.appearance->unmarked_icon})},
	  bullet2{container_element_impl.get_window_handler()
		  .create_icon({init_args.style.appearance->marked_icon})},
	  submenu{container_element_impl.get_window_handler()
		  .create_icon({init_args.style.appearance->submenu_icon})},

	  itemlabel_meta{create_background_color(init_args.style.appearance
						 ->contents_appearance
						 ->label_color),
			  create_current_fontcollection(init_args.style
							.appearance
							->contents_appearance
							->label_font)},
	  itemshortcut_meta{create_background_color
			  (init_args.style.appearance
			   ->shortcut_foreground_color),
			  create_current_fontcollection
			  (init_args.style.appearance->shortcut_font)},
	  current_list_item_changed_thread_only
	{
	 init_args.style.current_list_item_changed
	}
{
	for (auto &info:requested_col_widths)
	{
		if (info.first >= columns)
			throw EXCEPTION(gettextmsg(_("Column %1% does not exist"),
						   info.first));

		if (info.second < 0 || info.second > 100)
			throw EXCEPTION(gettextmsg(_("Invalid width of %1%%2% specified for column %3%"),
						   info.second, "%", info.first));
	}

	for (auto &info:col_alignments)
		if (info.first >= columns)
			throw EXCEPTION(gettextmsg(_("Column %1% does not exist"),
						   info.first));

	for (auto &info:row_alignments)
		if (info.first >= columns)
			throw EXCEPTION(gettextmsg(_("Column %1% does not exist"),
						   info.first));

	for (auto &info:minimum_column_widths_thread_only)
		if (info.first >= columns)
			throw EXCEPTION(gettextmsg(_("Minimum width specified"
						     " for non-existent column"
						     " %1%"),
						   info.first));

	listimpl_info_t::lock lock{textlist_info};

	lock->list_column_widths.resize(columns);
}

list_elementObj::implObj::~implObj()=default;

void list_elementObj::implObj::removed_from_container(ONLY IN_THREAD)
{
	superclass_t::removed_from_container(IN_THREAD);

	// Organized detachment of this list's column widths from any other
	// list that this list's column widths get synchronized to.

	synchronized_info.removed_from_container(IN_THREAD);
}

void list_elementObj::implObj
::report_new_current_element(ONLY IN_THREAD,
			     const listimpl_info_t::lock &lock,
			     const std::optional<size_t> &original,
			     const std::optional<size_t> &current,
			     const callback_trigger_t &trigger)
{
	if (!current_list_item_changed(IN_THREAD))
		return;

	ptr<listlayoutmanagerObj::implObj> llm_impl;

	textlist_container->invoke_layoutmanager
		([&, this]
		 (const auto &lm)
		 {
			 llm_impl=lm;
		 });

	if (!llm_impl)
		return;

	listlayoutmanager llm=llm_impl->create_public_object();
	list_lock llock{llm};
	busy_impl yes_i_am{*this};

	list_item_status_info_t info
		{
		 llm,
		 llock,
		 0,
		 false,
		 trigger,
		 yes_i_am,
		};

	if (original)
	{
		info.item_number=*original;

		try {
			current_list_item_changed(IN_THREAD)(IN_THREAD, info);
		} REPORT_EXCEPTIONS(this);

	}

	if (current)
	{
		info.item_number=*current;
		info.selected=true;

		try {
			current_list_item_changed(IN_THREAD)(IN_THREAD, info);
		} REPORT_EXCEPTIONS(this);
	}
}

void list_elementObj::implObj::remove_rows(ONLY IN_THREAD,
					   const listlayoutmanager &lm,
					   size_t row_number,
					   size_t n_rows)
{
	list_lock lock{lm};

	listimpl_info_t::lock &l{lock};

	remove_rows(IN_THREAD, lm, l, row_number, n_rows);
}

void list_elementObj::implObj
::append_rows(ONLY IN_THREAD,
	      const listlayoutmanager &lm,
	      new_cells_info &info)
{
	list_lock lock{lm};

	listimpl_info_t::lock &l{lock};

	// Implement by calling insert at the end of the list.
	insert_rows(IN_THREAD, lm, lock, l->row_infos.size(), info);
}

void list_elementObj::implObj
::insert_rows(ONLY IN_THREAD,
	      const listlayoutmanager &lm,
	      size_t row_number,
	      new_cells_info &info)
{
	list_lock lock{lm};

	insert_rows(IN_THREAD, lm, lock, row_number, info);
}

void list_elementObj::implObj
::insert_rows(ONLY IN_THREAD,
	      const listlayoutmanager &lm,
	      list_lock &ll,
	      size_t row_number,
	      new_cells_info &info)
{
	listimpl_info_t::lock &lock=ll;

	if (row_number > lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Item %1% does not exist"),
					   row_number));

	size_t rows=info.newcells.size() / columns;

	if (rows != info.rowmeta.size())
		throw EXCEPTION("Internal error, wrong number of row items");

	// Size the arrays in advance.

	lock->row_infos.reserve(lock->row_infos.size() + rows);
	lock->cells.reserve(lock->cells.size() + rows);

	// We can now insert them.
	lock->row_infos.insert(lock->row_infos.begin() + row_number,
			       info.rowmeta.begin(),
			       info.rowmeta.end());

	size_t row_num=0;

	std::for_each(lock->row_infos.begin() + row_number,
		      lock->row_infos.begin() + row_number + rows,
		      [&]
		      (auto &iter)
		      {
			      iter.extra->set_meta(lm, iter,
						   lock,
						   std::get<1>
						   (info.rowmeta.at(row_num++))
						   );
		      });

	lock->cells.insert(lock->cells.begin() + row_number * columns,
			   info.newcells.begin(), info.newcells.end());

	// Everything must be recalculated and redrawn.

	lock->full_redraw_needed=true;

	// If a current element was selected on or after the insertion point,
	// update it accordingly.
	if (current_element(lock) && current_element(lock).value()>=row_number)
	{
		current_element(IN_THREAD, lock, *current_element(lock) + rows,
				std::monostate{});
	}

	if (current_keyed_element(lock) &&
	    current_keyed_element(lock).value()>=row_number)
		*current_keyed_element(lock) += rows;
}

void list_elementObj::implObj
::replace_rows(ONLY IN_THREAD,
	       const listlayoutmanager &lm,
	       size_t row_number,
	       new_cells_info &info)
{
	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	size_t n=info.newcells.size() / columns;

	if (row_number > lock->row_infos.size())
		removing_rows(IN_THREAD, lm, lock, row_number, n); // Throw the exception

	if (n >= lock->row_infos.size()-row_number)
	{
		// Edge case, replacing rows at the end. Do this via
		// remove+insert.

		remove_rows(IN_THREAD, lm, lock, row_number,
			    lock->row_infos.size()-row_number);
		insert_rows(IN_THREAD, lm, ll, lock->row_infos.size(), info);
		return;
	}

	// The existing rows are officially being removed.
	removing_rows(IN_THREAD, lm, lock, row_number, n);

	// With the booking out of the way, we simply replace the rows and
	// cells.

	size_t row_num=0;

	std::generate(lock->row_infos.begin()+row_number,
		      lock->row_infos.begin()+row_number+n,
		      [&]
		      {
			      auto &meta=info.rowmeta.at(row_num++);

			      list_row_info_t r{meta};

			      r.extra->set_meta(lm, r,
						lock,
						std::get<1>(meta));

			      return r;
		      });

	// Need to explicitly set modified, since std::generate is going
	// to bypass our carefully drafted contract.
	lock->row_infos.modified=true;

	std::copy(info.newcells.begin(), info.newcells.end(),
		  lock->cells.begin()+row_number*columns);

	// Recalculate and redraw everything.

	lock->full_redraw_needed=true;
}

void list_elementObj::implObj
::replace_all_rows(ONLY IN_THREAD,
		   const listlayoutmanager &lm,
		   new_cells_info &info)
{
	list_lock ll{lm};

	unselect(IN_THREAD, lm, ll);

	listimpl_info_t::lock &lock=ll;

	if (current_element(lock))
		current_element(IN_THREAD, lock, std::nullopt,
				std::monostate{});
	current_keyed_element(lock)={};

	// Clear out everything, then use insert_rows().

	lock->row_infos.clear();
	lock->cells.clear();
	for (auto &column_widths:lock->list_column_widths)
		column_widths.clear();
	insert_rows(IN_THREAD, lm, ll, 0, info);
}

void list_elementObj::implObj
::resort_rows(ONLY IN_THREAD,
	      const listlayoutmanager &lm,
	      std::vector<size_t> &indexes)
{
	list_lock ll{lm};
	listimpl_info_t::lock &lock=ll;

	if (current_element(lock))
		current_element(IN_THREAD, lock, std::nullopt,
				std::monostate{});
	current_keyed_element(lock)={};

	lock->row_infos.modified=true; // We don't do anything that gets flagged
	lock->full_redraw_needed=true;

	sort_by(indexes,
		[&, cells_b=lock->cells.begin()]
		(size_t a, size_t b)
		{
			std::swap(lock->row_infos.at(a),
				  lock->row_infos.at(b));

			auto ca=cells_b+a*columns;
			auto cb=cells_b+b*columns;

			std::swap_ranges(ca, ca+columns, cb);
		});

}

void list_elementObj::implObj::remove_rows(ONLY IN_THREAD,
					   const listlayoutmanager &lm,
					   listimpl_info_t::lock &lock,
					   size_t row_number,
					   size_t count)
{
	removing_rows(IN_THREAD, lm, lock, row_number, count);

	lock->row_infos.erase(lock->row_infos.begin()+row_number,
			      lock->row_infos.begin()+row_number+count);

	lock->cells.erase(lock->cells.begin()+row_number*columns,
			  lock->cells.begin()+(row_number+count)*columns);
}

void list_elementObj::implObj
::removing_rows(ONLY IN_THREAD,
		const listlayoutmanager &lm,
		listimpl_info_t::lock &lock,
		size_t row,
		size_t count)
{
	// Before we proceed with the removal we must unselect anything
	// that's selected in this range.
	//
	// If the given range is invalid, we'll throw the exception below,
	// but for now we can ignore this.
	//
	// removing_rows() gets invoked at the start of an operation
	// that ultimately removes the rows. Because the unselection invokes
	// an app callback, a rude app callback can try and make additional
	// modifications to the contents of the list. We do this up-front,
	// here, before we get down to the business of removing the rows,
	// so the list is still in a valid state by the time we're done.

	for (size_t i=0; i<count; ++i)
	{
		if (row+i >= lock->row_infos.size())
			break;

		auto &r=lock->row_infos.at(row+i);

		if (r.extra->data(lock).selected)
			selected(IN_THREAD, lm, row+i, false, {});
	}

	if (row > lock->row_infos.size() ||
	    lock->row_infos.size() - row < count)
		throw EXCEPTION(_("The range of rows to remove or replace is not valid."));

	// If the current element is in the selected range, unselect it.

	if (current_element(lock) && current_element(lock).value() >= row)
	{
		if (current_element(lock).value() < row+count)
		{
			is_key_or_button_down=false;
			current_element(IN_THREAD, lock, std::nullopt,
					std::monostate{});
		}
		else
		{
			current_element(IN_THREAD, lock,
					*current_element(lock)-count,
					std::monostate{});
		}
	}

	if (current_keyed_element(lock) &&
	    current_keyed_element(lock).value() >= row)
	{
		if (current_keyed_element(lock).value() < row+count)
			current_keyed_element(lock)={};
		else
		{
			*current_keyed_element(lock) -= count;
		}
	}

	// Unlink the rows being removed from column_widths.

	auto p=lock->row_infos.begin()+row;
	auto cellp=lock->cells.begin()+row*columns;

	for (; count; ++p, --count)
	{
		for (auto &column_widths:lock->list_column_widths)
		{
			if (p->size_computed)
				column_widths.erase((*cellp)->column_iterator);
			++cellp;
		}
	}
	lock->full_redraw_needed=true;
}

// Note this is also implements update_current_position().

void list_elementObj::implObj::recalculate(ONLY IN_THREAD)
{
	create_textlist_info_lock lock{IN_THREAD, *this};

	// If full recalculation was done, that's it. Otherwise the only
	// thing we need to do is to calculate_column_poswidths, because
	// the update_synchronized_column_widths()-d, and they were
	// changed, and we need to reflect that.

	if (!lock.was_modified)
	{
		dim_t width=calculate_column_poswidths(IN_THREAD, lock);

		update_metrics(IN_THREAD,
			       { width, width, width},
			       get_horizvert(IN_THREAD)->vert);

		if (lock->full_redraw_needed)
		{
			lock->full_redraw_needed=false;
			schedule_full_redraw(IN_THREAD);
		}
	}

	should_be_current_element_actually_is(IN_THREAD, lock);
}

void list_elementObj::implObj
::should_be_current_element_actually_is(ONLY IN_THREAD,
					const listimpl_info_t::lock &lock)

{
	if (should_be_current_element(lock) == current_element(lock))
		return;

	current_element(IN_THREAD, lock,
			should_be_current_element(lock),
			std::monostate{});
}

void list_elementObj::implObj::update_metrics(ONLY IN_THREAD,
					      const metrics::axis &h,
					      const metrics::axis &v)
{
	get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD, h, v);
}

void list_elementObj::implObj
::calculate_column_widths(ONLY IN_THREAD,
			  listimpl_info_t::lock &lock)
{
	lock->calculated_column_widths.clear();

	lock->calculated_column_widths.reserve(columns);

	size_t i=0;

	for (auto &column_widths:lock->list_column_widths)
	{
		dim_t maximum_width{0};

		if (!column_widths.empty())
			maximum_width=*column_widths.begin();

		// Minimum column width override:
		auto iter=minimum_column_widths(IN_THREAD).find(i);

		if (iter != minimum_column_widths(IN_THREAD).end())
			maximum_width=std::get<dim_t>(iter->second);

		lock->calculated_column_widths.push_back(maximum_width);
		++i;
	}
}

void list_elementObj::implObj::initialize(ONLY IN_THREAD)
{
	recalculate_minimum_column_width_pixels(IN_THREAD,
						get_screen()->impl
						->current_theme.get());

	superclass_t::initialize(IN_THREAD);

	listimpl_info_t::lock lock{textlist_info};

	recalculate_with_new_theme(IN_THREAD, lock);
	request_visibility(IN_THREAD, true);
}

void list_elementObj::implObj::theme_updated(ONLY IN_THREAD,
					     const const_defaulttheme &new_theme)
{
	recalculate_minimum_column_width_pixels(IN_THREAD,
						get_screen()->impl
						->current_theme.get());
	superclass_t::theme_updated(IN_THREAD, new_theme);
	listimpl_info_t::lock lock{textlist_info};

	for (const auto &cell:lock->cells)
		cell->cell_theme_updated(IN_THREAD, new_theme);

	recalculate_with_new_theme(IN_THREAD, lock);
}

void list_elementObj::implObj::recalculate_with_new_theme(ONLY IN_THREAD,
							  listimpl_info_t::lock
							  &lock)
{
	// Shortcut: clear the aggregate column_widths, and clear
	// size_computed from every row, and recalculate() will rebuild it.

	for (auto &cell:lock->row_infos)
		cell.size_computed=false;

	for (auto &cw:lock->list_column_widths)
		cw.clear();

	lock->full_redraw_needed=true;
	recalculate(IN_THREAD, lock);
}

void list_elementObj::implObj::recalculate(ONLY IN_THREAD,
					   listimpl_info_t::lock &lock)
{
	size_t n=lock->row_infos.size();
	coord_t y=0;
	auto row=lock->row_infos.begin();

	calculate_column_widths(IN_THREAD, lock);

	auto v_padding_times_two=list_v_padding(IN_THREAD);

	v_padding_times_two=dim_t::truncate(v_padding_times_two +
					    v_padding_times_two);

	auto screen=get_screen()->impl;
	auto current_theme=*current_theme_t::lock{screen->current_theme};

	tallest_row_height(IN_THREAD)={0, 0};

	for (size_t i=0; i<n; ++i, ++row)
	{
		row->y=y;
		row->extra->current_row_number(IN_THREAD)=i;

		if (!row->size_computed)
		{
			// This is a new row that's not been linked into
			// column_widths yet.

			row->height=0;

			auto cell_iter=lock->cells.begin()+i*columns;

			auto calculated_column_width=
				lock->calculated_column_widths.begin();

			bool is_separator=false;

			size_t column_counter=columns;

			for (auto &column_widths:lock->list_column_widths)
			{
				auto preferred_width=
					*calculated_column_width++;

				(*cell_iter)->cell_initialize(IN_THREAD,
							      current_theme);

				// Perhaps we'll handle word-wrapping columns
				// properly, some day.
				auto [horiz, vert]=(*cell_iter)
					->cell_get_metrics
					(IN_THREAD,
					 preferred_width);

				// If this is a separator row we still need
				// to go through the motion and visit every
				// cell, in order to initialize the column
				// separators.

				if ((*cell_iter)->cell_is_separator())
					is_separator=true;

				auto h=horiz.preferred();

				// Add the row's indent to the
				// last column's width.
				if (--column_counter == 0 && !is_separator)
					h=dim_t::truncate
						(h + dim_t::truncate
						 (row->indent) *
						 themedim_element
						 <listcontainer_indent>
						 ::pixels(IN_THREAD));

				(*cell_iter)->column_iterator=
					column_widths.insert(h);
				(*cell_iter)->height=vert.preferred();

				if (row->height < vert.preferred())
					row->height=vert.preferred();

				++cell_iter;
			}
			row->size_computed=true;

			if (is_separator)
			{
				// This row becomes a separator line.

				row->extra->data(lock).row_type=
					list_row_type_t::separator;
				row->height=current_border(IN_THREAD)->border(IN_THREAD)
					->calculated_border_height;
			}
		}

		y=coord_t::truncate(y+row->height);
		y=coord_t::truncate(y+v_padding_times_two);

		if (row->height > tallest_row_height(IN_THREAD).without_padding)
		{
			tallest_row_height(IN_THREAD).with_padding=
				dim_t::truncate
				((tallest_row_height(IN_THREAD).without_padding=
				  row->height) + v_padding_times_two);
		}
	}

	calculate_column_widths(IN_THREAD, lock);

	dim_t width=calculate_column_poswidths(IN_THREAD, lock);
	dim_t height=dim_t::truncate(y);

	lock->row_infos.modified=false;

	update_metrics(IN_THREAD,
		       { width, width, width},
		       { height, height, height});

	if (lock->full_redraw_needed)
	{
		lock->full_redraw_needed=false;
		schedule_full_redraw(IN_THREAD);
	}
}

dim_t list_elementObj::implObj
::calculate_column_poswidths(ONLY IN_THREAD,
			     listimpl_info_t::lock &lock)
{
	// These are the columns that we will synchronize with any other
	// lists or grid.

	std::vector<metrics::axis> our_column_widths_values;

	const size_t n=lock->calculated_column_widths.size();

	our_column_widths_values.reserve(n*2+1);

	// We want to build our_column_widths_values as we were a grid
	// which has border columns interspersed with cell columns.

	size_t i=0;

	for (const auto &w:lock->calculated_column_widths)
	{
		if (i == 0)
		{
			// First column in a grid is the grid's left border.
			// Lists don't have a left border, so simulate one.
			our_column_widths_values.emplace_back(0, 0, 0);
		}
		else
		{
			// If there's a border column before this cell column,
			// grab it's width, otherwise the border column still
			// exists, but it's 0.

			auto iter=column_borders.find(i);

			dim_t w{0};

			if (iter != column_borders.end())
			{
				w=iter->second
					->border(IN_THREAD)
					->calculated_border_width;
			}

			our_column_widths_values.emplace_back(w, w, w);
		}

		auto [left_h_padding, right_h_padding] =
			get_paddings(IN_THREAD, i);
		++i;

		dim_t total_padding=dim_t::truncate(left_h_padding +
						   right_h_padding);
		auto total_column_width=dim_t::truncate(total_padding + w);

		// This is what we will synchronize. The synchronized metrics
		// represent the width of each column including the padding.

		our_column_widths_values.emplace_back(total_column_width,
						      total_column_width,
						      total_column_width);
	}

	// Prepared requested column widths, for synchronization.

	std::unordered_map<size_t, int> requested_col_and_border_widths;

	for (const auto &colwidth:requested_col_widths)
	{
		requested_col_and_border_widths.emplace
			(CALCULATE_BORDERS_COORD(colwidth.first),
			 colwidth.second);
	}

	// Synchronize this list's column widths with the other lists'.

	my_synchronized_axis::lock sync_lock{synchronized_info};

	sync_lock.update_values(IN_THREAD, our_column_widths_values,
				data(IN_THREAD).current_position.width,
				requested_col_and_border_widths);

	// We now use the synchronized column_widths to compute the final
	// position and width of our columns and borders. We extract the
	// combined list of borders and columns into columns_poswidths and
	// border_positions.

	std::vector<std::tuple<coord_t, dim_t>> new_columns_poswidths;
	std::unordered_map<size_t, std::tuple<coord_t, dim_t>
			   > new_border_positions;

	new_columns_poswidths.reserve(n);

	i=0;

	dim_t final_width=0;

	// Now, we take the synchronized column widths. Since it is based on
	// the width of each column including its padding, we now substract
	// each column's padding from each derived_value, in order to
	// compute what goes into the new_columns_poswidths.

	for (const auto &axis:sync_lock->scaled_values)
	{
		// Don't forget to advance past the border area before this
		// column

		if (IS_BORDER_RESERVED_COORD(i))
		{
			auto col=BORDER_COORD_TO_ROWCOL(i);
			++i;

			// Ignore column #0, the left border before the list
			// or grid. Lists don't have column #0, this is for
			// compatibility with grids.
			if (col == 0 && col >= n)
				continue;

			coord_t x{coord_t::truncate(final_width)};
			auto w=axis.minimum();

			new_border_positions.emplace(col, std::tuple{x, w});

			final_width=dim_t::truncate(final_width+w);
			continue;
		}

		auto col=NONBORDER_COORD_TO_ROWCOL(i);
		++i;

		if (col >= n)
			continue; // Ignore extra columns

		auto [left_h_padding, right_h_padding]=
			get_paddings(IN_THREAD, col);

		dim_t total_padding=dim_t::truncate(left_h_padding +
						   right_h_padding);
		dim_t total_column_width=axis.minimum();

		dim_t total_usable_width=total_column_width-total_padding;

		// This shouldn't happen:
		if (total_column_width < total_padding)
			total_usable_width=0;

		new_columns_poswidths.emplace_back
			(coord_t::truncate(final_width+left_h_padding),
			 total_usable_width);

		final_width=dim_t::truncate(final_width+total_column_width);
	}

	// We will definitely need a redraw if something changes.
	// Even if the final_width is the same.

	if (lock->columns_poswidths != new_columns_poswidths)
	{
		std::swap(lock->columns_poswidths, new_columns_poswidths);
		lock->full_redraw_needed=true;
	}

	if (lock->border_positions != new_border_positions)
	{
		std::swap(lock->border_positions, new_border_positions);
		lock->full_redraw_needed=true;
	}

	dim_t minimum_width=
		dim_t::truncate(metrics::axis::total_minimum
				(sync_lock->unscaled_values.begin(),
				 sync_lock->unscaled_values.end()));

	if (minimum_width == dim_t::infinite())
		--minimum_width;
	return minimum_width;
}

void list_elementObj::implObj::process_updated_position(ONLY IN_THREAD)

{
	recalculate(IN_THREAD);
	superclass_t::process_updated_position(IN_THREAD);
}

void list_elementObj::implObj::do_draw(ONLY IN_THREAD,
				       const draw_info &di,
				       const rectarea &areas)
{
	create_textlist_info_lock lock{IN_THREAD, *this};

	do_draw(IN_THREAD, di, areas, lock);
}


void list_elementObj::implObj::do_draw(ONLY IN_THREAD,
				       const draw_info &di,
				       const rectarea &areas,
				       textlist_info_lock &our_lock)
{
	auto &lock=our_lock.lock;

	richtext_draw_boundaries bounds{di, areas};

	if (full_redraw_scheduled(IN_THREAD))
		return; // Don't bother.

	if (bounds.nothing_to_draw())
	{
		superclass_t::do_draw(IN_THREAD, di, areas); // Clear to bg colr
		return;
	}

	if (lock->full_redraw_needed)
		// Something else must be reponsible for rescheduling us later.
		return;

	auto b=lock->row_infos.begin();
	auto e=lock->row_infos.end();

	auto iter=std::upper_bound(b, e,
				   bounds.draw_bounds.y,
				   []
				   (auto y,
				    const list_row_info_t &row_info)
				   {
					   return y < row_info.y;
				   });

	if (iter != b)
		--iter;

	coord_t last_y=coord_t::truncate(bounds.draw_bounds.y +
					 bounds.draw_bounds.height);

	// Draw all the rows, collecting the drawn areas, in absolute
	// coordinates.
	rectarea drawn;

	drawn.reserve(e-iter);

	{
		coord_t end_y{0};
		dim_t row_height{0};

		clip_region_set clipped{IN_THREAD, get_window_handler(), di};

		for (; iter != e; ++iter)
		{
			if (iter->y >= last_y)
				break;

			auto rect=do_draw_row(IN_THREAD, di, clipped, bounds,
					      lock, iter-b);

			drawn.push_back(rect);
			row_height=rect.height;
			end_y=coord_t::truncate(rect.y+rect.height);
		}

		// We need to draw any separator borders in any extra space
		// where we did not draw any rows. We will intelligently pick
		// how much vertical space to draw the borders into, each time.

		while (end_y < last_y)
		{
			dim_t left=dim_t::truncate(last_y - end_y);

			if (row_height == 0)
				row_height=itemlabel_meta.getfont()
					->fc(IN_THREAD)->height();

			if (left > row_height)
				left=row_height;

			rectangle entire_row{0, end_y,
					     di.absolute_location.width,
					     left};

			if (!full_redraw_scheduled(IN_THREAD))
			{
				do_draw_row_borders(IN_THREAD, di, clipped,
						    lock, entire_row, drawn);
			}

			end_y=coord_t::truncate(end_y + left);
		}
	}
	// Subtract the drawn areas from what we have to draw, and clear
	// the rest to background color.
	superclass_t::do_draw(IN_THREAD, di, subtract(areas, drawn));
}

void list_elementObj::implObj::redraw_rows(ONLY IN_THREAD,
				       listimpl_info_t::lock &lock,
				       size_t row_number1)
{
	redraw_rows(IN_THREAD, lock, row_number1, row_number1, false);
}

void list_elementObj::implObj::redraw_rows(ONLY IN_THREAD,
					   listimpl_info_t::lock &lock,
					   size_t row_number1,
					   size_t row_number2,
					   bool make_sure_row2_is_visible)
{
	dim_t v_padding=list_v_padding(IN_THREAD);
	auto &width=data(IN_THREAD).current_position.width;

	if (row_number2 < lock->row_infos.size())
	{
		auto &r=lock->row_infos.at(row_number2);

		coord_t y=r.y;

		rectangle entire_row{0, y, width,
				     dim_t::truncate(r.height +
						     v_padding + v_padding)};

		if (make_sure_row2_is_visible)
			ensure_visibility(IN_THREAD, entire_row);

		schedule_redraw(IN_THREAD, entire_row);
	}

	if (row_number1 != row_number2 &&
	    row_number1 < lock->row_infos.size())
	{
		auto &r=lock->row_infos.at(row_number1);

		coord_t y=r.y;

		rectangle entire_row{0, y, width,
				     dim_t::truncate(r.height +
						     v_padding + v_padding)};

		schedule_redraw(IN_THREAD, entire_row);
	}
}


rectangle list_elementObj::implObj::do_draw_row(ONLY IN_THREAD,
						const draw_info &di,
						clip_region_set &clipped,
						richtext_draw_boundaries &bounds,
						listimpl_info_t::lock &lock,
						size_t row_number)
{
	clip_region_set clip{IN_THREAD, get_window_handler(), di};

	auto &r=lock->row_infos.at(row_number);

	if (r.extra->data(lock).row_type == list_row_type_t::separator)
	{
		rectangle border_rect{
			0, r.y, di.absolute_location.width,
				r.height};

		if (full_redraw_scheduled(IN_THREAD))
			return border_rect;

		draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const picture &area_picture,
			  const pixmap &area_pixmap,
			  const gc &area_gc)
			 {
				 scratch_buffer_for_separator
					 ->get(border_rect.width,
					       border_rect.height,
					       [&, this]
					       (const picture &mask_picture,
						const pixmap &mask_pixmap,
						const gc &mask_gc)
					       {
						       border_implObj
							       ::draw_info bdi={
							       area_picture,
							       border_rect,
							       area_pixmap,
							       mask_picture,
							       mask_pixmap,
							       mask_gc,
							       di.absolute_location.x,
							       di.absolute_location.y};
						       current_border(IN_THREAD)->border
							       (IN_THREAD)
							       ->draw_horizontal
							       (IN_THREAD, bdi);
					       });

			 },
			 border_rect,
			 di, di, clipped);

		return border_rect;
	}

	// Fudge draw_info, pretending that we have a different background
	// color.

	if (current_element(lock) && current_element(lock).value()
	    == row_number)
	{
		auto cpy=di;

		cpy.window_background_color=
			(is_key_or_button_down
			 ? background_color_element<
			 listcontainer_highlighted_color>::get(IN_THREAD)
			 : background_color_element<listcontainer_current_color>
			 ::get(IN_THREAD))->get_current_color(IN_THREAD);


		return do_draw_row(IN_THREAD, cpy, clipped, bounds,
				   lock, row_number, r);
	}

	if (r.extra->data(lock).selected)
	{
		auto cpy=di;

		// If this is the highlighted list style, install this
		// background color.

		list_style.set_selected_background
			(IN_THREAD, cpy,
			 background_color_element<listcontainer_selected_color>
			 ::get(IN_THREAD));

		return do_draw_row(IN_THREAD, cpy, clipped, bounds, lock,
				   row_number, r);
	}

	return do_draw_row(IN_THREAD, di, clipped, bounds, lock, row_number, r);
}

rectangle list_elementObj::implObj
::do_draw_row(ONLY IN_THREAD,
	      const draw_info &di,
	      clip_region_set &clipped,
	      richtext_draw_boundaries &bounds,
	      listimpl_info_t::lock &lock,
	      size_t row_number,
	      const list_row_info_t &r)
{
	rectarea drawn_columns;

	drawn_columns.reserve(lock->columns_poswidths.size());

	coord_t y=r.y;

	dim_t v_padding=list_v_padding(IN_THREAD);

	rectangle entire_row{0, y, di.absolute_location.width,
			dim_t::truncate(r.height + v_padding + v_padding)};

	if (full_redraw_scheduled(IN_THREAD))
		// Don't bother.
		return entire_row;

	// Indentation level:
	dim_t indent=dim_t::truncate
		(themedim_element<listcontainer_indent>::pixels(IN_THREAD)
		 * r.indent);

	coord_t top_y=coord_t::truncate(y+v_padding);

	auto *cell=&lock->cells.at(row_number * columns);

	dim_t element_width=data(IN_THREAD).current_position.width;

	for (const auto &poswidth:lock->columns_poswidths)
	{
		const auto &[x, width]=poswidth;

		rectangle rc{coord_t::truncate(x+indent),
			     coord_t::truncate
			     (top_y + ((*cell)->valignment == valign::middle
				       ? (r.height - (*cell)->height)/2
				       : (*cell)->valignment == valign::bottom
				       ? (r.height - (*cell)->height)
				       : dim_t{0})),
			     width,
			     (*cell)->height};

		// Check if the column lies outside of the element.
		//
		// Input field search popup limits the width of the list
		// to the width of the input field, truncating it if necessary.
		//
		// In order to show too-long search results with ellipsis we
		// need to truncate the column width.

		if (dim_t::truncate(rc.x) >= element_width)
			continue;
		dim_t maximum_width{element_width - dim_t::truncate(rc.x)};
		if (rc.width > maximum_width)
			rc.width=maximum_width;

		// Reposition the draw boundaries here.
		bounds.position_at(rc);
		drawn_columns.push_back(rc);

		(*cell)->cell_redraw(IN_THREAD, *this, di, clipped,
				     r.extra->data(lock).row_type
				     == list_row_type_t::disabled,
				     bounds);

		++cell;
	}

	if (indent == 0) // Not supposed to have borders with indentation level
		do_draw_row_borders(IN_THREAD, di, clipped, lock,
				    entire_row,
				    drawn_columns);

	auto to_clear=subtract(rectarea{{entire_row}},
			       drawn_columns);

	superclass_t::do_draw(IN_THREAD, di, to_clear);

	return entire_row; // What we just drew.
}

void list_elementObj::implObj
::do_draw_row_borders(ONLY IN_THREAD,
		      const draw_info &di,
		      clip_region_set &clipped,
		      listimpl_info_t::lock &lock,
		      const rectangle &entire_row,
		      rectarea &drawn_area)
{
	for (const auto &border_position:lock->border_positions)
	{
		auto iter=column_borders.find(border_position.first);

		if (iter == column_borders.end()) continue; // Wut?

		auto b=iter->second->border(IN_THREAD);

		rectangle border_rect=entire_row;

		auto &[x, width]=border_position.second;

		border_rect.x=x;
		border_rect.width=width;

		draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const picture &area_picture,
			  const pixmap &area_pixmap,
			  const gc &area_gc)
			 {
				 scratch_buffer_for_borders->get
					 (width,
					  border_rect.height,
					  [&, this]
					  (const picture &mask_picture,
					   const pixmap &mask_pixmap,
					   const gc &mask_gc)
					  {
						  border_implObj::draw_info bdi=
							  {area_picture,
							   {x, border_rect.y,
							    width,
							    border_rect.height},
							   area_pixmap,
							   mask_picture,
							   mask_pixmap,
							   mask_gc,
							   0, 0};

						  b->draw_vertical(IN_THREAD,
								   bdi);
					  });
			 },
			 border_rect,
			 di, di, clipped);

		drawn_area.push_back(border_rect);
	}
}


void list_elementObj::implObj::report_motion_event(ONLY IN_THREAD,
						   const motion_event &me)
{
	superclass_t::report_motion_event(IN_THREAD, me);

	if (me.y < 0) // Shouldn't happen.
		return;

	create_textlist_info_lock lock{IN_THREAD, *this};

	if (current_element(lock))
	{
		auto &row=lock->row_infos.at(current_element(lock).value());

		if (me.y >= row.y && me.y < coord_t::truncate(row.y+row.height))
			return; // Same row.
	}

	auto b=lock->row_infos.begin();
	auto e=lock->row_infos.end();

	auto iter=std::upper_bound(b, e,
				   me.y,
				   []
				   (auto y,
				    const list_row_info_t &row_info)
				   {
					   return y < row_info.y;
				   });
	if (iter == b)
		return;
	--iter;

	if (me.y >= iter->y &&
	    me.y < coord_t::truncate(iter->y+iter->height) &&
	    iter->extra->enabled(lock))
		set_current_element(IN_THREAD, lock, iter-b, false,
				    std::monostate{});
	else
		unset_current_element(IN_THREAD, lock, std::monostate{});
}

bool list_elementObj::implObj::process_key_event(ONLY IN_THREAD,
						 const key_event &ke)
{
	{
		create_textlist_info_lock lock{IN_THREAD, *this};

		if (process_key_event(IN_THREAD, ke, lock))
			return true;
	}

	return superclass_t::process_key_event(IN_THREAD, ke);
}

bool list_elementObj::implObj::process_key_event(ONLY IN_THREAD,
						 const key_event &ke,
						 listimpl_info_t::lock &lock)
{
	if (lock->row_infos.empty())
		return false;

	if (select_key(ke))
	{
		bool changed=false;

		if (!ke.keypress)
		{
			if (!is_key_or_button_down)
				changed=true;

			is_key_or_button_down=false;
		}
		else
		{
			if (current_element(lock))
			{
				if (!is_key_or_button_down)
					changed=true;

				is_key_or_button_down=true;
			}
		}

		if (current_element(lock))
			redraw_rows(IN_THREAD, lock,
				    current_element(lock).value());

		if (current_element(lock) && changed && activate_for(ke))
			click(IN_THREAD, lock, &ke);
		return true;
	}

	if (!ke.notspecial())
		return false;

	if (lock->row_infos.size() == 0)
		return false;

	size_t next_row;

	if (!activate_for(ke))
		return false;

	switch (ke.keysym) {
	case XK_Up:
	case XK_KP_Up:
		{
			auto r=move_up_by(lock, 0);

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	case XK_Down:
	case XK_KP_Down:
		{
			auto r=move_down_by(lock, 0);

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	case XK_Page_Up:
	case XK_KP_Page_Up:
		{
			auto r=move_up_by(lock, textlist_container
					  ->most_recent_visible_height
					  (IN_THREAD));

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	case XK_Page_Down:
	case XK_KP_Page_Down:
		{
			auto r=move_down_by(lock,
					    textlist_container
					    ->most_recent_visible_height
					    (IN_THREAD));

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	default:
		return false;
	}

	set_current_element(IN_THREAD, lock, next_row, true, &ke);
	current_keyed_element(lock)=next_row;

	return true;
}

// move_up_by/move_down_by: move in the given direction,
// skipping over the disabled elements. Do not exceed "howmuch" pixels, but
// move at least one element unless all elements (if any) in the given
// direction are disabled.

std::optional<size_t>
list_elementObj::implObj::move_up_by(listimpl_info_t::lock &lock,
				     dim_t howmuch)
{
	if (!current_keyed_element(lock))
		return {};

	auto next_row=current_keyed_element(lock).value();

	std::optional<size_t> move_to;
	dim_t moved_by=0;

	while (next_row)
	{
		--next_row;

		auto &info=lock->row_infos.at(next_row);

		moved_by=dim_t::truncate(moved_by+info.height);

		if (moved_by > howmuch && move_to)
			break;

		if (!info.extra->enabled(lock))
			continue;

		move_to=next_row;
	}
	return move_to;
}

std::optional<size_t>
list_elementObj::implObj::move_down_by(listimpl_info_t::lock &lock,
				       dim_t howmuch)
{
	size_t next_row;

	if (!current_keyed_element(lock))
		next_row=0;
	else
		next_row=current_keyed_element(lock).value()+1;

	std::optional<size_t> move_to;
	dim_t moved_by=0;

	while (next_row < lock->row_infos.size())
	{
		auto &info=lock->row_infos.at(next_row);

		moved_by=dim_t::truncate(moved_by+info.height);

		if (moved_by > howmuch && move_to)
			break;

		if (info.extra->enabled(lock))
			move_to=next_row;
		++next_row;
	}
	return move_to;
}

bool list_elementObj::implObj::process_button_event(ONLY IN_THREAD,
						    const button_event &be,
						    xcb_timestamp_t timestamp)
{
	bool flag=false;

	if (be.button == 1 || be.button == 3)
	{
		create_textlist_info_lock lock{IN_THREAD, *this};

		if (current_element(lock))
		{
			bool potential_action=
				is_key_or_button_down != be.press;

			is_key_or_button_down=be.press;
			redraw_rows(IN_THREAD, lock,
				    current_element(lock).value());

			if (potential_action && activate_for(be))
				click(IN_THREAD, lock, &be);

			flag=true;
		}
	}

	// Also forward the event to the parent class, so that button click
	// also moves the input focus here.

	if (superclass_t::process_button_event(IN_THREAD, be, timestamp))
		flag=true;

	return flag;
}

void list_elementObj::implObj::pointer_focus(ONLY IN_THREAD,
					     const callback_trigger_t &trigger)
{
	superclass_t::pointer_focus(IN_THREAD, trigger);

#if 0
	// This causes deselection when the right context popup menu gets
	// opened, and the main window gets LeaveNotify event.

	create_textlist_info_lock lock{IN_THREAD, *this};

	if (!current_pointer_focus(IN_THREAD))
	{
		unset_current_element(IN_THREAD, lock, trigger);
		current_keyed_element(lock).reset();
	}
#endif
}

void list_elementObj::implObj::keyboard_focus(ONLY IN_THREAD,
					      const callback_trigger_t &trigger)
{
	superclass_t::keyboard_focus(IN_THREAD, trigger);

	create_textlist_info_lock lock{IN_THREAD, *this};

	if (!current_keyboard_focus(IN_THREAD))
	{
		unset_current_element(IN_THREAD, lock, trigger);
		current_keyed_element(lock).reset();
	}
}

void list_elementObj::implObj
::unset_current_element(ONLY IN_THREAD,
			listimpl_info_t::lock &lock,
			const callback_trigger_t &trigger)
{
	if (!current_element(lock))
		return;

	auto row_number=*current_element(lock);

	// Reset some things.
	is_key_or_button_down=false;
	current_element(IN_THREAD, lock, std::nullopt, trigger);
	redraw_rows(IN_THREAD, lock, row_number);
}

void list_elementObj::implObj
::set_current_element(ONLY IN_THREAD,
		      listimpl_info_t::lock &lock,
		      size_t row_number,
		      bool make_sure_row_is_visible,
		      const callback_trigger_t &trigger)
{
	size_t row_number1=row_number;

	if (current_element(lock))
		row_number1=current_element(lock).value();

	current_element(IN_THREAD, lock, row_number, trigger);

	// Reset some things.
	is_key_or_button_down=false;
	current_keyed_element(lock)={};
	redraw_rows(IN_THREAD, lock, row_number1, row_number,
		    make_sure_row_is_visible);
}

void list_elementObj::implObj::click(ONLY IN_THREAD,
				     listimpl_info_t::lock &lock,
				     const callback_trigger_t &trigger)
{
	if (!current_element(lock))
		return;

	textlist_container->invoke_layoutmanager
		([&, row_number=current_element(lock).value()]
		 (const auto &lm)
		 {
			 listlayoutmanager tlm=lm->create_public_object();

			 tlm->autoselect(IN_THREAD, row_number, trigger);
		 });
}

void list_elementObj::implObj::autoselect(ONLY IN_THREAD,
					  const listlayoutmanager &lm,
					  size_t i,
					  const callback_trigger_t &trigger)
{
	listimpl_info_t::lock lock{textlist_info};

	busy_impl yes_i_am{*this};

	lock->selection_type(IN_THREAD, lm, i, trigger, yes_i_am);
}

void list_elementObj::implObj::selected(ONLY IN_THREAD,
					const listlayoutmanager &lm,
					size_t i,
					bool selected_flag,
					const callback_trigger_t &trigger)
{
	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Item %1% does not exist"), i));

	selected_common(IN_THREAD,
			lm, ll, lock, i, selected_flag, trigger);
}

void list_elementObj::implObj
::menuitem_selected(ONLY IN_THREAD,
		    const listlayoutmanager &lm,
		    size_t i,
		    const callback_trigger_t &trigger,
		    const busy &mcguffin)
{
	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Item %1% does not exist"), i));

	auto &row=lock->row_infos.at(i);

	if (row.extra->has_submenu(lock))
	{
		auto y=row.y;
		auto height=row.height;

		auto r=get_absolute_location(IN_THREAD);

		r.y = coord_t::truncate(r.y+y);
		r.height=height;

		get_window_handler().get_absolute_location_on_screen
			(IN_THREAD, r);

		row.extra->toggle_submenu(IN_THREAD, lock, r);
		return;
	}

	if (row.extra->is_option(lock))
	{
		selected_common(IN_THREAD, lm, ll, lock, i,
				!row.extra->data(lock).selected,
				trigger);
	}
	else
	{
		notify_callbacks(IN_THREAD,
				 lm, ll, row, i, row.extra->data(lock).selected,
				 trigger, mcguffin);
	}

	// Our job is to make arrangements to close
	// all menu popups, now that the menu selection
	// has been made...

	get_window_handler().handler_data->close_all_menu_popups(IN_THREAD);
}

void list_elementObj::implObj
::selected_common(ONLY IN_THREAD,
		  const listlayoutmanager &lm,
		  list_lock &ll,
		  listimpl_info_t::lock &lock,
		  size_t i,
		  bool selected_flag,
		  const callback_trigger_t &trigger)
{
	auto &r=lock->row_infos.at(i);

	if (r.extra->data(lock).selected == selected_flag)
		return;

	r.extra->data(lock).selected=selected_flag;

	redraw_rows(IN_THREAD, lock, i, i,
		    selected_flag &&
		    (
		     trigger.index() == callback_trigger_key_event ||
		     trigger.index() == callback_trigger_button_event
		     ));

	try {
		list_style.selected_changed(&lock->cells.at(i*columns),
					    selected_flag);
	} REPORT_EXCEPTIONS(this);

	notify_callbacks(IN_THREAD, lm, ll, r, i, selected_flag, trigger);
}

void list_elementObj::implObj
::notify_callbacks(ONLY IN_THREAD,
		   const listlayoutmanager &lm,
		   list_lock &ll,
		   const list_row_info_t &r,
		   size_t i,
		   bool selected_flag,
		   const callback_trigger_t &trigger)
{
	busy_impl yes_i_am{*this};

	notify_callbacks(IN_THREAD,
			 lm, ll, r, i, selected_flag, trigger, yes_i_am);
}

void list_elementObj::implObj
::notify_callbacks(ONLY IN_THREAD,
		   const listlayoutmanager &lm,
		   list_lock &ll,
		   const list_row_info_t &r,
		   size_t i,
		   bool selected_flag,
		   const callback_trigger_t &trigger,
		   const busy &mcguffin)
{

	listimpl_info_t::lock &lock=ll;

	list_item_status_info_t info{
		lm, ll, i, selected_flag, trigger, mcguffin};

	if (r.extra->data(lock).status_change_callback)
		try {
			r.extra->data(lock)
				.status_change_callback(IN_THREAD, info);
		} REPORT_EXCEPTIONS(this);

	if (lock->selection_changed &&
	    trigger.index() != callback_trigger_initial)
		try {
			lock->selection_changed(IN_THREAD, info);
		} REPORT_EXCEPTIONS(this);
}

bool list_elementObj::implObj::enabled(size_t i)
{
	listimpl_info_t::lock lock{textlist_info};

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Item %1% does not exist"), i));

	return lock->row_infos.at(i).extra->enabled(lock);
}

void list_elementObj::implObj::enabled(ONLY IN_THREAD, size_t i, bool flag)
{
	create_textlist_info_lock lock{IN_THREAD, *this};

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Item %1% does not exist"), i));

	enabled(IN_THREAD, lock, lock->row_infos.at(i).extra, flag);
}

void list_elementObj::implObj::enabled(ONLY IN_THREAD,
				       const extra_list_row_info &extra,
				       bool flag)
{
	create_textlist_info_lock lock{IN_THREAD, *this};

	enabled(IN_THREAD, lock, extra, flag);
}

void list_elementObj::implObj::enabled(ONLY IN_THREAD,
				       textlist_info_lock &lock,
				       const extra_list_row_info &extra,
				       bool flag)
{
	auto &r=extra->data(lock.lock);

	if (r.row_type != list_row_type_t::enabled &&
	    r.row_type != list_row_type_t::disabled)
		return; // Don't touch separators.

	auto new_type=flag ? list_row_type_t::enabled:list_row_type_t::disabled;

	if (r.row_type == new_type)
		return;

	r.row_type=new_type;

	redraw_rows(IN_THREAD, lock.lock, extra->current_row_number(IN_THREAD));
}

void list_elementObj::implObj
::on_status_update(ONLY IN_THREAD,
		   const listlayoutmanager &lm,
		   const extra_list_row_info &extra,
		   const list_item_status_change_callback &new_callback)
{
	list_lock ll{lm};
	textlist_info_lock lock{IN_THREAD, ll, *this};

	size_t i=extra->current_row_number(IN_THREAD);

	if (i >= lock->row_infos.size())
		return;

	if (lock->row_infos.at(i).extra != extra)
		return;

	on_status_update(IN_THREAD, lm, ll, lock, i, new_callback);
}

void list_elementObj::implObj
::on_status_update(ONLY IN_THREAD,
		   const listlayoutmanager &lm,
		   size_t i,
		   const list_item_status_change_callback &new_callback)
{
	list_lock ll{lm};
	textlist_info_lock lock{IN_THREAD, ll, *this};

	on_status_update(IN_THREAD, lm, ll, lock, i, new_callback);
}

void list_elementObj::implObj
::on_status_update(ONLY IN_THREAD,
		   const listlayoutmanager &lm,
		   list_lock &ll,
		   textlist_info_lock &lock,
		   size_t i,
		   const list_item_status_change_callback &new_callback)
{
	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("List item #%1% does not exist"),
					   i));

	auto &info=lock->row_infos.at(i);

	info.extra->data(lock.lock).status_change_callback=new_callback;

	notify_callbacks(IN_THREAD, lm, ll, info,
			 i,
			 info.extra->data(lock.lock).selected,
			 initial{});
}

size_t list_elementObj::implObj::size()
{
	listimpl_info_t::lock lock{textlist_info};

	return lock->row_infos.size();
}

bool list_elementObj::implObj::selected(size_t i)
{
	listimpl_info_t::lock lock{textlist_info};

	return i < lock->row_infos.size() &&
		lock->row_infos.at(i).extra->data(lock).selected;
}

size_t list_elementObj::implObj::hierindent(size_t i)
{
	listimpl_info_t::lock lock{textlist_info};

	return i < lock->row_infos.size() ? lock->row_infos.at(i).indent : 0;
}

std::optional<size_t> list_elementObj::implObj::selected()
{
	listimpl_info_t::lock lock{textlist_info};

	size_t i=0;

	for (const auto &r:lock->row_infos)
	{
		if (r.extra->data(lock).selected)
			return i;
		++i;
	}

	return {};
}

std::vector<size_t> list_elementObj::implObj::all_selected()
{
	std::vector<size_t> all;

	listimpl_info_t::lock lock{textlist_info};

	size_t i=0;

	for (const auto &r:lock->row_infos)
	{
		if (r.extra->data(lock).selected)
			all.push_back(i);
		++i;
	}

	return all;
}

void list_elementObj::implObj::unselect(ONLY IN_THREAD,
					const listlayoutmanager &lm)
{
	list_lock ll{lm};

	unselect(IN_THREAD, lm, ll);
}

void list_elementObj::implObj::unselect(ONLY IN_THREAD,
					const listlayoutmanager &lm,
					list_lock &ll)
{
	listimpl_info_t::lock &lock=ll;

	callback_trigger_t internal;

	// We are going to invoke app-provided callbacks.
	// A rude app can use the callback to modify this list, so do this
	// safely.

	for (size_t i=0; i < lock->row_infos.size(); ++i)
	{
		auto &r=lock->row_infos.at(i);

		if (r.extra->data(lock).selected)
		{
			r.extra->data(lock).selected=false;

			redraw_rows(IN_THREAD, lock, i);

			try {
				list_style.selected_changed
					(&lock->cells.at(i*columns),
					 false);
			} CATCH_EXCEPTIONS;
			notify_callbacks(IN_THREAD,
					 lm, ll, r, i, false, internal);
		}
	}
}

std::chrono::milliseconds list_elementObj::implObj
::hover_action_delay(ONLY IN_THREAD)
{
	create_textlist_info_lock lock{IN_THREAD, *this};

	if (current_element(lock))
	{
		auto &row=lock->row_infos.at(current_element(lock).value());

		if (row.extra->has_submenu(lock))
			return std::chrono::milliseconds(listitempopup_delay
							 .get());
	}
	return std::chrono::milliseconds{0};
}

void list_elementObj::implObj::hover_action(ONLY IN_THREAD)
{
	create_textlist_info_lock lock{IN_THREAD, *this};

	if (!current_element(lock))
		return;

	auto &row=lock->row_infos.at(current_element(lock).value());

	auto r=get_absolute_location(IN_THREAD);

	r.y = coord_t::truncate(r.y+row.y);
	r.height=row.height;

	get_window_handler().get_absolute_location_on_screen(IN_THREAD, r);
	row.extra->show_submenu(IN_THREAD, lock, r);
}

listlayoutmanagerptr list_elementObj::implObj
::get_item_layoutmanager(size_t i)
{
	listlayoutmanagerptr ptr;

	listimpl_info_t::lock lock{textlist_info};

	if (i < lock->row_infos.size())
	{
		auto extra=lock->row_infos.at(i).extra;

		if (extra->has_submenu(lock))
			ptr=extra->submenu_layoutmanager(lock);
	}

	return ptr;
}

LIBCXXW_NAMESPACE_END
