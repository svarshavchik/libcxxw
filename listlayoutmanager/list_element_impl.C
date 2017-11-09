/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/list_cell.H"
#include "popup/popup.H"
#include "popup/popup_attachedto_handler.H"
#include "focus/focusable_element.H"
#include "background_color_element.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "icon.H"
#include "richtext/richtext.H"
#include "richtext/richtext_draw_boundaries.H"
#include "x/w/motion_event.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/scratch_buffer.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "messages.H"
#include "themedim.H"
#include "background_color.H"
#include "draw_info.H"
#include "element_screen.H"
#include "busy.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include "defaulttheme.H"
#include <algorithm>
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

static property::value<unsigned>
listitempopup_delay(LIBCXX_NAMESPACE_STR "::w::listitempopup_delay", 500);

list_lock::list_lock(const listlayoutmanagerObj &manager)
	: listimpl_info_lock_t{manager.impl->list_element_singleton->impl
		->textlist_info},
	  layout_manager{&manager}
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
::textlist_info_lock(IN_THREAD_ONLY, implObj &me)
	: listimpl_info_t::lock{me.textlist_info},
	was_modified{ listimpl_info_t::lock::operator->()
			->row_infos.modified}
{
	if (was_modified)
		me.recalculate(IN_THREAD, *this);
}

list_elementObj::implObj::textlist_info_lock::~textlist_info_lock()=default;

////////////////////////////////////////////////////////////////////////////

list_elementObj::implObj::implObj(const ref<list_container_implObj>
				  &textlist_container,
				  const new_listlayoutmanager &style)
	: implObj(textlist_container, style,
		  textlist_container->get_element_impl())
{
}

list_elementObj::implObj::implObj(const ref<list_container_implObj>
			      &textlist_container,
			      const new_listlayoutmanager &style,
			      elementObj::implObj &container_element_impl)
	: implObj(textlist_container, style,
		  container_element_impl,
		  container_element_impl.get_screen())
{
}

list_elementObj::implObj::implObj(const ref<list_container_implObj>
			      &textlist_container,
			      const new_listlayoutmanager &style,
			      elementObj::implObj &container_element_impl,
			      const screen &container_screen)
	: superclass_t(container_element_impl
		       .create_background_color(style.selected_color),
		       container_element_impl
		       .create_background_color(style.highlighted_color),
		       container_element_impl
		       .create_background_color(style.current_color),
		       textlist_container),
	  textlist_container(textlist_container),
	  list_style(style.list_style),
	  columns(list_style.actual_columns(style)),
	  rows(style.rows),
	  requested_col_widths(list_style.actual_col_widths(style)),
	  col_alignments(list_style.actual_col_alignments(style)),
	  scratch_buffer_for_separator(container_screen->create_scratch_buffer
				       ("list_separator_scratch@libcxx",
					container_screen
					->find_alpha_pictformat_by_depth(1),
					0, 0)),
	  separator_border(container_screen->impl
			   ->get_theme_border("list_separator_border")),
	  bullet1(container_element_impl.get_window_handler()
		  .create_icon_mm("bullet1", render_repeat::none, 0, 0)),
	  bullet2(container_element_impl.get_window_handler()
		  .create_icon_mm("bullet2", render_repeat::none, 0, 0)),

	  itemlabel_meta{create_background_color("label_foreground_color"),
		create_theme_font(label_theme_font())},
	  itemshortcut_meta{create_background_color("label_foreground_color"),
			  create_theme_font("menu_shortcut")}

{
	// Some sanity checks.

	if (rows <= 0)
		throw EXCEPTION(_("Cannot create a list with 0 visible rows."));

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

	listimpl_info_t::lock lock{textlist_info};

	lock->column_widths.resize(columns);
	lock->selection_type=style.selection_type;
	lock->selection_changed=style.selection_changed;
}

list_elementObj::implObj::~implObj()=default;

void list_elementObj::implObj::remove_row(const listlayoutmanager &lm,
				      size_t row_number)
{
	list_lock lock{lm};

	listimpl_info_t::lock &l{lock};

	remove_rows(lm, l, row_number, 1);
}

void list_elementObj::implObj::append_rows(const listlayoutmanager &lm,
				       const std::vector<list_item_param>
				       &items)
{
	std::vector<list_cell> texts;
	std::vector<textlist_rowinfo> meta;

	list_style.create_cells(items, *this, texts, meta);

	append_rows(lm, texts, meta);
}

void list_elementObj::implObj::append_rows(const listlayoutmanager &lm,
				       const std::vector<list_cell> &texts,
				       const std::vector<textlist_rowinfo> &meta
				       )
{
	list_lock lock{lm};

	listimpl_info_t::lock &l{lock};

	// Implement by calling insert at the end of the list.
	insert_rows(lm, lock, l->row_infos.size(), texts, meta);
}

void list_elementObj::implObj::insert_rows(const listlayoutmanager &lm,
					   size_t row_number,
					   const std::vector<list_item_param>
					   &items)
{
	std::vector<list_cell> texts;
	std::vector<textlist_rowinfo> meta;

	list_style.create_cells(items, *this, texts, meta);

	insert_rows(lm, row_number, texts, meta);
}

void list_elementObj::implObj::insert_rows(const listlayoutmanager &lm,
					   size_t row_number,
					   const std::vector<list_cell> &texts,
					   const std::vector<textlist_rowinfo>
					   &meta)
{
	list_lock lock{lm};

	insert_rows(lm, lock, row_number, texts, meta);
}

void list_elementObj::implObj::insert_rows(const listlayoutmanager &lm,
					   list_lock &ll,
					   size_t row_number,
					   const std::vector<list_cell>
					   &texts,
					   const std::vector<textlist_rowinfo>
					   &meta)
{
	listimpl_info_t::lock &lock=ll;

	if (row_number > lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"),
					   row_number));

	size_t rows=texts.size() / columns;

	// Size the arrays in advance.

	lock->row_infos.reserve(lock->row_infos.size() + rows);
	lock->cells.reserve(lock->cells.size() + rows);

	// We can now insert them.
	lock->row_infos.insert(lock->row_infos.begin() + row_number,
			       rows, list_row_info_t{});

	bool first_one=true;
	size_t row_num=0;

	std::for_each(lock->row_infos.begin() + row_number,
		      lock->row_infos.begin() + row_number + rows,
		      [&]
		      (auto &iter)
		      {
			      // Each insert()ed list_row_info points to the
			      // the same extra object, we can fix this here.

			      if (!first_one)
				      iter.extra=extra_list_row_info::create();
			      first_one=false;

			      iter.extra->set_meta(lm, meta.at(row_num++));
		      });

	lock->cells.insert(lock->cells.begin() + row_number * columns,
			   texts.begin(), texts.end());

	// Everything must be recalculated and redrawn.

	lock->full_redraw_needed=true;

	// If a current element was selected on or after the insertion point,
	// update it accordingly.
	if (current_element(lock) && current_element(lock).value()>=row_number)
		++*current_element(lock);

	if (current_keyed_element(lock) &&
	    current_keyed_element(lock).value()>=row_number)
		++*current_keyed_element(lock);
}

void list_elementObj::implObj::replace_rows(const listlayoutmanager &lm,
					    size_t row_number,
					    const std::vector<list_item_param>
					    &items)
{
	std::vector<list_cell> texts;
	std::vector<textlist_rowinfo> meta;

	list_style.create_cells(items, *this, texts, meta);

	replace_rows(lm, row_number, texts, meta);
}

void list_elementObj::implObj::replace_rows(const listlayoutmanager &lm,
					    size_t row_number,
					    const std::vector<list_cell>
					    &texts,
					    const std::vector<textlist_rowinfo>
					    &meta)
{
	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	size_t n=texts.size() / columns;

	if (row_number > lock->row_infos.size())
		removing_rows(lm, lock, row_number, n); // Throw the exception

	if (n >= lock->row_infos.size()-row_number)
	{
		// Edge case, replacing rows at the end. Do this via
		// remove+insert.

		remove_rows(lm, lock, row_number,
			    lock->row_infos.size()-row_number);
		insert_rows(lm, ll, lock->row_infos.size(), texts, meta);
		return;
	}

	// The existing rows are officially being removed.
	removing_rows(lm, lock, row_number, n);

	// With the booking out of the way, we simply replace the rows and
	// cells.

	size_t row_num=0;

	std::generate(lock->row_infos.begin()+row_number,
		      lock->row_infos.begin()+row_number+n,
		      [&]
		      {
			      list_row_info_t r;

			      r.extra->set_meta(lm, meta.at(row_num++));

			      return r;
		      });

	// Need to explicitly set modified, since std::generate is going
	// to bypass our carefully drafted contract.
	lock->row_infos.modified=true;

	std::copy(texts.begin(), texts.end(),
		  lock->cells.begin()+row_number*columns);

	// Recalculate and redraw everything.

	lock->full_redraw_needed=true;
}

void list_elementObj::implObj
::replace_all_rows(const listlayoutmanager &lm,
		   const std::vector<list_item_param> &items)
{
	std::vector<list_cell> texts;
	std::vector<textlist_rowinfo> meta;

	list_style.create_cells(items, *this, texts, meta);

	replace_all_rows(lm, texts, meta);
}

void list_elementObj::implObj
::replace_all_rows(const listlayoutmanager &lm,
		   const std::vector<list_cell> &texts,
		   const std::vector<textlist_rowinfo> &meta)
{
	list_lock ll{lm};

	unselect(lm, ll);

	listimpl_info_t::lock &lock=ll;

	current_element(lock)={};
	current_keyed_element(lock)={};

	// Clear out everything, then use insert_rows().

	lock->row_infos.clear();
	lock->cells.clear();
	for (auto &column_widths:lock->column_widths)
		column_widths.clear();
	insert_rows(lm, ll, 0, texts, meta);
}

void list_elementObj::implObj::remove_rows(const listlayoutmanager &lm,
					   listimpl_info_t::lock &lock,
					   size_t row_number,
					   size_t count)
{
	removing_rows(lm, lock, row_number, count);

	lock->row_infos.erase(lock->row_infos.begin()+row_number,
			      lock->row_infos.begin()+row_number+count);

	lock->cells.erase(lock->cells.begin()+row_number*columns,
			  lock->cells.begin()+(row_number+count)*columns);
}

void list_elementObj::implObj
::removing_rows(const listlayoutmanager &lm,
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

		if (r.extra->selected)
			selected(lm, row+i, false, {});
	}

	if (row > lock->row_infos.size() ||
	    lock->row_infos.size() - row < count)
		throw EXCEPTION(_("The range of rows to remove or replace is not valid."));

	// If the current element is in the selected range, unselect it.

	if (current_element(lock) && current_element(lock).value() <= row &&
	    current_element(lock).value() < row+count)
	{
		is_key_or_button_down=false;
		current_element(lock)={};
	}

	if (current_keyed_element(lock) &&
	    current_keyed_element(lock).value() <= row &&
	    current_keyed_element(lock).value() < row+count)
		current_keyed_element(lock)={};

	// Unlink the rows being removed from column_widths.

	auto p=lock->row_infos.begin()+row;
	auto cellp=lock->cells.begin()+row*columns;

	for (; count; ++p, --count)
	{
		for (auto &column_widths:lock->column_widths)
		{
			if (p->size_computed)
				column_widths.erase((*cellp)->column_iterator);
			++cellp;
		}
	}
	lock->full_redraw_needed=true;
}

void list_elementObj::implObj::recalculate(IN_THREAD_ONLY)
{
	textlist_info_lock lock{IN_THREAD, *this};
}

void list_elementObj::implObj
::calculate_column_widths(IN_THREAD_ONLY,
			  listimpl_info_t::lock &lock)
{
	lock->calculated_column_widths.clear();

	lock->calculated_column_widths.reserve(columns);

	for (auto &column_widths:lock->column_widths)
	{
		dim_t maximum_width{0};

		if (!column_widths.empty())
			maximum_width=*column_widths.begin();

		lock->calculated_column_widths.push_back(maximum_width);
	}
}

void list_elementObj::implObj::initialize(IN_THREAD_ONLY)
{
	auto screen=get_screen()->impl;
	auto current_theme=*current_theme_t::lock{screen->current_theme};

	listimpl_info_t::lock lock{textlist_info};

	separator_border->theme_updated(IN_THREAD, current_theme);

	recalculate_with_new_theme(IN_THREAD, lock);
	superclass_t::initialize(IN_THREAD);
	request_visibility(IN_THREAD, true);
}

void list_elementObj::implObj::theme_updated(IN_THREAD_ONLY,
					     const defaulttheme &new_theme)
{
	listimpl_info_t::lock lock{textlist_info};

	for (const auto &cell:lock->cells)
		cell->cell_theme_updated(IN_THREAD, new_theme);

	separator_border->theme_updated(IN_THREAD, new_theme);
	recalculate_with_new_theme(IN_THREAD, lock);
	superclass_t::theme_updated(IN_THREAD, new_theme);
}

void list_elementObj::implObj::recalculate_with_new_theme(IN_THREAD_ONLY,
							  listimpl_info_t::lock
							  &lock)
{
	// Shortcut: clear the aggregate column_widths, and clear
	// size_computed from every row, and recalculate() will rebuild it.

	for (auto &cell:lock->row_infos)
		cell.size_computed=false;

	for (auto &cw:lock->column_widths)
		cw.clear();

	lock->full_redraw_needed=true;
	recalculate(IN_THREAD, lock);
}

void list_elementObj::implObj::recalculate(IN_THREAD_ONLY,
					   listimpl_info_t::lock &lock)
{
	size_t n=lock->row_infos.size();
	coord_t y=0;
	auto row=lock->row_infos.begin();

	calculate_column_widths(IN_THREAD, lock);

	auto v_padding_times_two=
		textlist_container->list_v_padding()->pixels(IN_THREAD);

	v_padding_times_two=dim_t::truncate(v_padding_times_two +
					    v_padding_times_two);

	auto screen=get_screen()->impl;
	auto current_theme=*current_theme_t::lock{screen->current_theme};

	tallest_row_height(IN_THREAD)=0;

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

			for (auto &column_widths:lock->column_widths)
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
					 preferred_width,
					 data(IN_THREAD)
					 .inherited_visibility);

				// If this is a separator row we still need
				// to go through the motion and visit event
				// cell, in order to initialize the column
				// separators.

				if ((*cell_iter)->cell_is_separator())
					is_separator=true;

				(*cell_iter)->column_iterator=
					column_widths.insert(horiz.preferred());
				(*cell_iter)->height=vert.preferred();

				if (row->height < vert.preferred())
					row->height=vert.preferred();

				++cell_iter;
			}
			row->size_computed=true;

			if (is_separator)
			{
				// This row becomes a separator line.

				row->extra->row_type=list_row_type_t::separator;
				row->height=separator_border->border(IN_THREAD)
					->calculated_border_height;
			}
		}
		coord_t old_y=y;

		y=coord_t::truncate(y+row->height);
		y=coord_t::truncate(y+v_padding_times_two);

		dim_t total_height=dim_t::truncate(y-old_y);

		if (total_height > tallest_row_height(IN_THREAD))
			tallest_row_height(IN_THREAD)=total_height;
	}

	calculate_column_widths(IN_THREAD, lock);

	dim_t width=calculate_column_poswidths(IN_THREAD, lock);
	dim_t height=dim_t::truncate(y);

	lock->row_infos.modified=false;
	get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      { width, width, width},
				      { height, height, height});

	if (lock->full_redraw_needed)
	{
		lock->full_redraw_needed=false;
		schedule_redraw(IN_THREAD);
	}
}

dim_t list_elementObj::implObj
::calculate_column_poswidths(IN_THREAD_ONLY,
			     listimpl_info_t::lock &lock)
{
	lock->columns_poswidths.clear();
	lock->columns_poswidths.reserve(lock->calculated_column_widths.size());

	dim_t final_width=0;

	auto h_padding=
		textlist_container->list_left_padding()->pixels(IN_THREAD);

	auto inner_padding_times_two=
		textlist_container->list_inner_padding()->pixels(IN_THREAD);

	inner_padding_times_two=dim_t::truncate(inner_padding_times_two +
						inner_padding_times_two);

	for (const auto &w:lock->calculated_column_widths)
	{
		final_width=dim_t::truncate(final_width + h_padding);
		lock->columns_poswidths
			.emplace_back(coord_t::truncate(final_width), w);
		final_width=dim_t::truncate(final_width + w);
		h_padding=inner_padding_times_two;
	}

	final_width=dim_t::truncate(final_width
				    + textlist_container->list_right_padding()
				    ->pixels(IN_THREAD));


	auto total_column_width=final_width;
	if (total_column_width==dim_t::infinite())
		total_column_width=dim_t::infinite()-1;

	// If the list is wider than final_width, distribute the additional
	// real estate according to requested_col_widths

	dim_t available_width=data(IN_THREAD).current_position.width;

	if (final_width != dim_t::infinite() // If we didn't overflow
	    && available_width > final_width)
	{
		unsigned denominator=0;

		for (const auto &requested_col_width:requested_col_widths)
			denominator += requested_col_width.second;

		dim_squared_t::value_type total_to_distribute=
			dim_squared_t::truncate(available_width-final_width);

		dim_squared_t::value_type numerator=0;

		dim_t coord_offset=0;

		for (size_t i=0; i<columns; ++i)
		{
			auto &poswidth=lock->columns_poswidths.at(i);

			poswidth.first=coord_t::truncate(poswidth.first +
							 coord_offset);

			unsigned n=0;

			auto iter=requested_col_widths.find(i);

			if (iter!=requested_col_widths.end())
				n=iter->second;

			numerator += total_to_distribute * n;

			dim_t extra=0;

			if (denominator)
			{
				extra=dim_t::truncate(numerator / denominator);

				numerator %= denominator;
			}

			poswidth.second=dim_t::truncate(poswidth.second+extra);
			coord_offset=dim_t::truncate(coord_offset+extra);
		}
	}

	return total_column_width;
}

void list_elementObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	{
		textlist_info_lock lock{IN_THREAD, *this};

		// If full recalculation was done, that's it. Otherwise the only
		// thing we need to do is to calculate_column_poswidths;

		if (!lock.was_modified)
			calculate_column_poswidths(IN_THREAD, lock);
	}

	superclass_t::process_updated_position(IN_THREAD);
}

void list_elementObj::implObj::do_draw(IN_THREAD_ONLY,
				       const draw_info &di,
				       const rectangle_set &areas)
{
	do_draw(IN_THREAD, di, areas, false);
}

void list_elementObj::implObj::redraw_needed_rows(IN_THREAD_ONLY)
{
	const auto &di=get_draw_info(IN_THREAD);

	do_draw(IN_THREAD, di, di.entire_area(), true);
}

void list_elementObj::implObj::do_draw(IN_THREAD_ONLY,
				       const draw_info &di,
				       const rectangle_set &areas,
				       bool only_whats_needed)
{
	richtext_draw_boundaries bounds{di, areas};

	textlist_info_lock lock{IN_THREAD, *this};

	if (only_whats_needed)
	{
		if (!lock->row_redraw_needed)
			return;
	}

	lock->row_redraw_needed=false;

	if (redraw_scheduled(IN_THREAD))
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
	rectangle_set drawn;

	for (; iter != e; ++iter)
	{
		if (iter->y >= last_y)
			break;

		if (only_whats_needed)
		{
			if (!iter->redraw_needed)
				continue;
		}
		auto rect=do_draw_row(IN_THREAD, di, bounds, lock, iter-b,
				      false);

		drawn.insert(rect);
	}

	if (only_whats_needed)
		return;

	// Subtract the drawn areas from what we have to draw, and clear
	// the rest to background color.
	superclass_t::do_draw(IN_THREAD, di, subtract(areas, drawn));
}

void list_elementObj::implObj::redraw_rows(IN_THREAD_ONLY,
				       listimpl_info_t::lock &lock,
				       size_t row_number1)
{
	redraw_rows(IN_THREAD, lock, row_number1, row_number1, false);
}

void list_elementObj::implObj::redraw_rows(IN_THREAD_ONLY,
					   listimpl_info_t::lock &lock,
					   size_t row_number1,
					   size_t row_number2,
					   bool make_sure_row2_is_visible)
{
	const auto &di=get_draw_info(IN_THREAD);

	richtext_draw_boundaries bounds{di, di.entire_area()};

	do_draw_row(IN_THREAD, di, bounds, lock, row_number2,
		    make_sure_row2_is_visible);

	if (row_number1 != row_number2)
		do_draw_row(IN_THREAD, di, bounds, lock, row_number1, false);
}


rectangle list_elementObj::implObj::do_draw_row(IN_THREAD_ONLY,
						const draw_info &di,
						richtext_draw_boundaries &bounds,
						listimpl_info_t::lock &lock,
						size_t row_number,
						bool make_sure_row_is_visible)
{
	auto &r=lock->row_infos.at(row_number);

	r.redraw_needed=false;

	if (r.extra->row_type == list_row_type_t::separator)
	{
		rectangle border_rect{
			0, r.y, di.absolute_location.width,
				r.height};

		if (redraw_scheduled(IN_THREAD))
			return border_rect;

		clip_region_set clip{IN_THREAD, di};

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
						       separator_border->border
							       (IN_THREAD)
							       ->draw_horizontal
							       (bdi);
					       });

			 },
			 border_rect,
			 di, di, clip);

		return border_rect;
	}

	// Fudge draw_info, pretending that we have a different background
	// color.

	if (current_element(lock) && current_element(lock).value()
	    == row_number)
	{
		auto cpy=di;

		cpy.window_background=
			(is_key_or_button_down
			 ? background_color_element<
			 listcontainer_highlighted_color>::get(IN_THREAD)
			 : background_color_element<listcontainer_current_color>
			 ::get(IN_THREAD))->get_current_color(IN_THREAD)->impl;


		return do_draw_row(IN_THREAD, cpy, bounds, lock, row_number, r,
				   make_sure_row_is_visible);
	}

	if (r.extra->selected)
	{
		auto cpy=di;

		// If this is the highlighted list style, install this
		// background color.

		list_style.set_selected_background
			(IN_THREAD, cpy,
			 background_color_element<listcontainer_selected_color>
			 ::get(IN_THREAD));

		return do_draw_row(IN_THREAD, cpy, bounds, lock, row_number, r,
				   make_sure_row_is_visible);
	}

	return do_draw_row(IN_THREAD, di, bounds, lock, row_number, r,
			   make_sure_row_is_visible);
}

rectangle list_elementObj::implObj
::do_draw_row(IN_THREAD_ONLY,
	      const draw_info &di,
	      richtext_draw_boundaries &bounds,
	      listimpl_info_t::lock &lock,
	      size_t row_number,
	      const list_row_info_t &r,
	      bool make_sure_row_is_visible)
{
	rectangle_set drawn_columns;

	coord_t y=r.y;

	dim_t v_padding=textlist_container->list_v_padding()
		->pixels(IN_THREAD);

	rectangle entire_row{0, y, di.absolute_location.width,
			dim_t::truncate(r.height + v_padding + v_padding)};

	if (make_sure_row_is_visible)
		ensure_visibility(IN_THREAD, entire_row);

	if (redraw_scheduled(IN_THREAD))
		// Don't bother, ensure_visibility() punted us.
		return entire_row;

	coord_t bottom_y=coord_t::truncate(y+r.height+v_padding);

	auto *cell=&lock->cells.at(row_number * columns);

	for (const auto &poswidth:lock->columns_poswidths)
	{
		rectangle rc{poswidth.first,
				coord_t::truncate(bottom_y-(*cell)->height),
				poswidth.second,
				(*cell)->height};

		bounds.position_at(rc);
		drawn_columns.insert(rc);

		(*cell)->cell_redraw(IN_THREAD, *this, di,
				     r.extra->row_type == list_row_type_t::disabled,
				     bounds);

		++cell;
	}

	auto to_clear=subtract(rectangle_set{{entire_row}},
			       drawn_columns);

	superclass_t::do_draw(IN_THREAD, di, to_clear);

	return entire_row; // What we just drew.
}


void list_elementObj::implObj::report_motion_event(IN_THREAD_ONLY,
						   const motion_event &me)
{
	superclass_t::report_motion_event(IN_THREAD, me);

	if (me.y < 0) // Shouldn't happen.
		return;

	textlist_info_lock lock{IN_THREAD, *this};

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
	    iter->extra->enabled())
		set_current_element(IN_THREAD, lock, iter-b, false);
	else
		unset_current_element(IN_THREAD, lock);
}

bool list_elementObj::implObj::process_key_event(IN_THREAD_ONLY,
						 const key_event &ke)
{
	textlist_info_lock lock{IN_THREAD, *this};

	if (process_key_event(IN_THREAD, ke, lock))
		return true;

	return superclass_t::process_key_event(IN_THREAD, ke);
}

bool list_elementObj::implObj::process_key_event(IN_THREAD_ONLY,
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
			auto r=move_up_by(lock, 1);

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	case XK_Down:
	case XK_KP_Down:
		{
			auto r=move_down_by(lock, 1);

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	case XK_Page_Up:
	case XK_KP_Page_Up:
		{
			auto r=move_up_by(lock, rows);

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	case XK_Page_Down:
	case XK_KP_Page_Down:
		{
			auto r=move_down_by(lock, rows);

			if (!r)
				return false;

			next_row=r.value();
		}
		break;
	default:
		return false;
	}

	set_current_element(IN_THREAD, lock, next_row, true);
	current_keyed_element(lock)=next_row;

	return true;
}

std::optional<size_t>
list_elementObj::implObj::move_up_by(listimpl_info_t::lock &lock,
				     size_t howmuch)
{
	if (!current_keyed_element(lock))
		return {};

	auto next_row=current_keyed_element(lock).value();

	if (next_row > howmuch)
		next_row-=howmuch;
	else
		next_row=0;

	while (!lock->row_infos.at(next_row).extra->enabled())
	{
		if (next_row == 0)
			return {};
		--next_row;
	}

	return next_row;
}

std::optional<size_t>
list_elementObj::implObj::move_down_by(listimpl_info_t::lock &lock,
				       size_t howmuch)
{
	size_t next_row;

	if (!current_keyed_element(lock))
		next_row=0;
	else
		next_row=current_keyed_element(lock).value()+howmuch;

	if (next_row >= lock->row_infos.size())
		next_row=lock->row_infos.size()-1;

	while (1)
	{
		if (next_row >= lock->row_infos.size())
			return {};

		if (lock->row_infos.at(next_row).extra->enabled())
			break;

		++next_row;
	}
	return next_row;
}

bool list_elementObj::implObj::process_button_event(IN_THREAD_ONLY,
						    const button_event &be,
						    xcb_timestamp_t timestamp)
{
	// Forward the event to the parent class first. This is so that the
	// button click propagates to the focusable element, and set the
	// input focus to this element.

	bool flag=superclass_t::process_button_event(IN_THREAD, be, timestamp);

	if (be.button == 1)
	{
		textlist_info_lock lock{IN_THREAD, *this};

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
	return flag;
}

void list_elementObj::implObj::pointer_focus(IN_THREAD_ONLY)
{
	superclass_t::pointer_focus(IN_THREAD);

	textlist_info_lock lock{IN_THREAD, *this};

	if (!current_pointer_focus(IN_THREAD))
	{
		unset_current_element(IN_THREAD, lock);
		current_keyed_element(lock).reset();
	}
}

void list_elementObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	superclass_t::keyboard_focus(IN_THREAD);

	textlist_info_lock lock{IN_THREAD, *this};

	if (!current_keyboard_focus(IN_THREAD))
	{
		unset_current_element(IN_THREAD, lock);
		current_keyed_element(lock).reset();
	}
}

void list_elementObj::implObj::unset_current_element(IN_THREAD_ONLY,
						     listimpl_info_t::lock &lock)
{
	if (!current_element(lock))
		return;

	auto row_number=*current_element(lock);

	// Reset some things.
	is_key_or_button_down=false;
	current_element(lock)={};
	redraw_rows(IN_THREAD, lock, row_number);
}

void list_elementObj::implObj
::set_current_element(IN_THREAD_ONLY,
		      listimpl_info_t::lock &lock,
		      size_t row_number,
		      bool make_sure_row_is_visible)
{
	size_t row_number1=row_number;

	if (current_element(lock))
		row_number1=current_element(lock).value();

	current_element(lock)=row_number;

	// Reset some things.
	is_key_or_button_down=false;
	current_keyed_element(lock)={};
	redraw_rows(IN_THREAD, lock, row_number1, row_number,
		    make_sure_row_is_visible);
}

void list_elementObj::implObj::click(IN_THREAD_ONLY,
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

			 tlm->autoselect(row_number, trigger);
		 });
}

void list_elementObj::implObj::autoselect(const listlayoutmanager &lm,
					  size_t i,
					  const callback_trigger_t &trigger)
{
	listimpl_info_t::lock lock{textlist_info};

	busy_impl yes_i_am{*this};

	lock->selection_type(lm, i, trigger, yes_i_am);
}

void list_elementObj::implObj::selected(const listlayoutmanager &lm,
					size_t i,
					bool selected_flag,
					const callback_trigger_t &trigger)
{
	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"), i));

	selected_common(lm, ll, lock, i, selected_flag, trigger);
}

void list_elementObj::implObj
::menuitem_selected(const listlayoutmanager &lm,
		    size_t i,
		    const callback_trigger_t &trigger,
		    const busy &mcguffin)
{
	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"), i));

	auto &row=lock->row_infos.at(i);

	if (row.extra->has_submenu())
	{
		// Need to postpone accessing row location until we're
		// IN_THREAD

		this->THREAD->run_as
			([e=ref(this),
			  extra=row.extra]
			 (IN_THREAD_ONLY)
			 {
				 textlist_info_lock lock{IN_THREAD, *e};

				 auto i=extra->current_row_number(IN_THREAD);

				 if (i >= lock->row_infos.size())
					 return;

				 const auto &row=lock->row_infos.at(i);

				 if (row.extra != extra)
					 return;

				 auto y=row.y;
				 auto height=row.height;

				 auto r=e->get_absolute_location(IN_THREAD);

				 r.y = coord_t::truncate(r.y+y);
				 r.height=height;

				 e->get_window_handler()
					 .get_absolute_location_on_screen
					 (IN_THREAD, r);

				 extra->toggle_submenu(IN_THREAD, r);
			 });
		return;
	}

	if (row.extra->is_option())
	{
		selected_common(lm, ll, lock, i,
				!row.extra->selected,
				trigger);
	}
	else
	{
		notify_callbacks(lm, ll, row, i, row.extra->selected,
				 trigger, mcguffin);
	}

	// Our job is to make arrangements to close
	// all menu popups, now that the menu selection
	// has been made...

	this->THREAD->run_as
		([e=ref(this)]
		 (IN_THREAD_ONLY)
		 {
			 e->get_window_handler().handler_data
				 ->close_all_menu_popups(IN_THREAD);
		 });
}

void list_elementObj::implObj
::selected_common(const listlayoutmanager &lm,
		  list_lock &ll,
		  listimpl_info_t::lock &lock,
		  size_t i,
		  bool selected_flag,
		  const callback_trigger_t &trigger)
{
	auto &r=lock->row_infos.at(i);

	if (r.extra->selected == selected_flag)
		return;

	r.extra->selected=selected_flag;
	r.redraw_needed=true;

	try {
		list_style.selected_changed(&lock->cells.at(i*columns),
					    selected_flag);
	} CATCH_EXCEPTIONS;

	notify_callbacks(lm, ll, r, i, selected_flag, trigger);
	schedule_row_redraw(lock);
}

void list_elementObj::implObj
::notify_callbacks(const listlayoutmanager &lm,
		   list_lock &ll,
		   const list_row_info_t &r,
		   size_t i,
		   bool selected_flag,
		   const callback_trigger_t &trigger)
{
	busy_impl yes_i_am{*this};

	notify_callbacks(lm, ll, r, i, selected_flag, trigger, yes_i_am);
}

void list_elementObj::implObj
::notify_callbacks(const listlayoutmanager &lm,
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

	if (r.extra->status_change_callback)
		try {
			r.extra->status_change_callback(info);
		} CATCH_EXCEPTIONS;

	try {
		lock->selection_changed(info);
	} CATCH_EXCEPTIONS;
}

bool list_elementObj::implObj::enabled(size_t i)
{
	listimpl_info_t::lock lock{textlist_info};

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"), i));

	return lock->row_infos.at(i).extra->enabled();
}

void list_elementObj::implObj::enabled(size_t i, bool flag)
{
	listimpl_info_t::lock lock{textlist_info};

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"), i));

	auto &r=lock->row_infos.at(i);

	if (r.extra->row_type != list_row_type_t::enabled &&
	    r.extra->row_type != list_row_type_t::disabled)
		return; // Don't touch separators.

	auto new_type=flag ? list_row_type_t::enabled:list_row_type_t::disabled;

	if (r.extra->row_type == new_type)
		return;

	r.extra->row_type=new_type;
	r.redraw_needed=true;

	schedule_row_redraw(lock);
}

void list_elementObj::implObj::schedule_row_redraw(listimpl_info_t::lock &lock)
{
	if (lock->row_redraw_needed)
		return; // Don't bother.

	lock->row_redraw_needed=true;

	THREAD->run_as([me=ref(this)]
		       (IN_THREAD_ONLY)
		       {
			       me->redraw_needed_rows(IN_THREAD);
		       });
}

size_t list_elementObj::implObj::size()
{
	listimpl_info_t::lock lock{textlist_info};

	return lock->row_infos.size();
}

bool list_elementObj::implObj::selected(size_t i)
{
	listimpl_info_t::lock lock{textlist_info};

	return i < lock->row_infos.size() && lock->row_infos.at(i).extra->selected;
}

std::optional<size_t> list_elementObj::implObj::selected()
{
	listimpl_info_t::lock lock{textlist_info};

	size_t i=0;

	for (const auto &r:lock->row_infos)
	{
		if (r.extra->selected)
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
		if (r.extra->selected)
			all.push_back(i);
		++i;
	}

	return all;
}

void list_elementObj::implObj::unselect(const listlayoutmanager &lm)
{
	list_lock ll{lm};

	if (unselect(lm, ll))
		schedule_row_redraw(ll);
}

bool list_elementObj::implObj::unselect(const listlayoutmanager &lm,
					list_lock &ll)
{
	bool unselected=false;

	listimpl_info_t::lock &lock=ll;

	callback_trigger_t internal;

	// We are going to invoke app-provided callbacks.
	// A rude app can use the callback to modify this list, so do this
	// safely.

	for (size_t i=0; i < lock->row_infos.size(); ++i)
	{
		auto &r=lock->row_infos.at(i);

		if (r.extra->selected)
		{
			r.extra->selected=false;
			r.redraw_needed=true;

			try {
				list_style.selected_changed
					(&lock->cells.at(i*columns),
					 false);
			} CATCH_EXCEPTIONS;
			unselected=true;
			notify_callbacks(lm, ll, r, i, false, internal);
		}
	}

	return unselected;
}

std::chrono::milliseconds list_elementObj::implObj
::hover_action_delay(IN_THREAD_ONLY)
{
	textlist_info_lock lock{IN_THREAD, *this};

	if (current_element(lock))
	{
		auto &row=lock->row_infos.at(current_element(lock).value());

		if (row.extra->has_submenu())
			return std::chrono::milliseconds(listitempopup_delay
							 .getValue());
	}
	return std::chrono::milliseconds{0};
}

void list_elementObj::implObj::hover_action(IN_THREAD_ONLY)
{
	textlist_info_lock lock{IN_THREAD, *this};

	if (!current_element(lock))
		return;

	auto &row=lock->row_infos.at(current_element(lock).value());

	auto r=get_absolute_location(IN_THREAD);

	r.y = coord_t::truncate(r.y+row.y);
	r.height=row.height;

	get_window_handler().get_absolute_location_on_screen(IN_THREAD, r);
	row.extra->show_submenu(IN_THREAD, r);
}

listlayoutmanagerptr list_elementObj::implObj
::get_item_layoutmanager(size_t i)
{
	listlayoutmanagerptr ptr;

	listimpl_info_t::lock lock{textlist_info};

	if (i < lock->row_infos.size())
	{
		auto extra=lock->row_infos.at(i).extra;

		if (extra->has_submenu())
			ptr=extra->submenu_layoutmanager();
	}

	return ptr;
}

LIBCXXW_NAMESPACE_END
