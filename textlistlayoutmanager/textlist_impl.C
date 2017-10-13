/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlistlayoutmanager/textlist_impl.H"
#include "textlistlayoutmanager/textlistlayoutstyle_impl.H"
#include "textlistlayoutmanager/textlistlayoutmanager_impl.H"
#include "textlistlayoutmanager/textlist_cell.H"
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

list_lock::list_lock(const textlistlayoutmanagerObj &manager)
	: variant_lock_t{std::in_place_type_t<textlist_tuple_t>{},
		manager.impl->textlist_element->impl->textlist_info, &manager}
{
}

list_lock::operator listimpl_info_t::lock &()
{
	return std::get<listimpl_info_t::lock>
		(std::get<textlist_tuple_t>(*this));
}

////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
void list_row_info_t::default_status_change_callback(list_lock &, size_t, bool)
{
}

textlistObj::implObj::implObj(const ref<textlist_container_implObj>
			      &textlist_container,
			      const new_listlayoutmanager &style,
			      const textlistlayout_style_impl &list_style)
	: implObj(textlist_container, style,
		  textlist_container->get_element_impl(), list_style)
{
}

textlistObj::implObj::implObj(const ref<textlist_container_implObj>
			      &textlist_container,
			      const new_listlayoutmanager &style,
			      elementObj::implObj &container_element_impl,
			      const textlistlayout_style_impl &list_style)
	: implObj(textlist_container, style,
		  container_element_impl,
		  container_element_impl.get_screen(), list_style)
{
}

textlistObj::implObj::implObj(const ref<textlist_container_implObj>
			      &textlist_container,
			      const new_listlayoutmanager &style,
			      elementObj::implObj &container_element_impl,
			      const screen &container_screen,
			      const textlistlayout_style_impl &list_style)
	: superclass_t(container_element_impl
		       .create_background_color(style.selected_color),
		       container_element_impl
		       .create_background_color(style.highlighted_color),
		       container_element_impl
		       .create_background_color(style.current_color),
		       textlist_container),
	  textlist_container(textlist_container),
	  list_style(list_style),
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
		  .create_icon_mm("bullet2", render_repeat::none, 0, 0))
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

textlistObj::implObj::~implObj()=default;

void textlistObj::implObj::remove_row(size_t row_number)
{
	listimpl_info_t::lock lock{textlist_info};

	remove_rows(lock, row_number, 1);
}

void textlistObj::implObj::append_rows(const textlistlayoutmanager &lm,
				       const std::vector<list_item_param>
				       &items)
{
	auto texts=list_style.create_cells(items, *this);

	list_lock lock{lm};

	listimpl_info_t::lock &l{lock};

	// Implement by calling insert at the end of the list.
	insert_rows(lm, lock, l->row_infos.size(), texts);
}

void textlistObj::implObj::insert_rows(const textlistlayoutmanager &lm,
				       size_t row_number,
				       const std::vector<list_item_param>
				       &items)
{
	auto texts=list_style.create_cells(items, *this);

	list_lock lock{lm};

	insert_rows(lm, lock, row_number, texts);
}

void textlistObj::implObj::insert_rows(const textlistlayoutmanager &lm,
				       list_lock &ll,
				       size_t row_number,
				       const std::vector<textlist_cell>
				       &texts)
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
			       rows, {});

	std::for_each(lock->row_infos.begin() + row_number,
		      lock->row_infos.begin() + row_number + rows,
		      [&]
		      (auto &iter)
		      {
			      iter.status_change_callback=lm->next_callback();
		      });

	lock->cells.insert(lock->cells.begin() + row_number * columns,
			   texts.begin(), texts.end());

	// Everything must be recalculated and redrawn.

	lock->recalculation_needed=true;
	lock->full_redraw_needed=true;

	// If a current element was selected on or after the insertion point,
	// update it accordingly.
	if (current_element(lock) && current_element(lock).value()>=row_number)
		++*current_element(lock);

	if (current_keyed_element(lock) &&
	    current_keyed_element(lock).value()>=row_number)
		++*current_keyed_element(lock);
}

void textlistObj::implObj::replace_rows(const textlistlayoutmanager &lm,
					size_t row_number,
					const std::vector<list_item_param>
					&items)
{
	auto texts=list_style.create_cells(items, *this);

	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	size_t n=items.size() / columns;

	if (row_number > lock->row_infos.size())
		removing_rows(lock, row_number, n); // Throw the exception

	if (n >= lock->row_infos.size()-row_number)
	{
		// Edge case, replacing rows at the end. Do this via
		// remove+insert.

		remove_rows(lock, row_number,
			    lock->row_infos.size()-row_number);
		insert_rows(lm, ll, lock->row_infos.size(), texts);
		return;
	}

	// The existing rows are officially being removed.
	removing_rows(lock, row_number, n);

	// With the booking out of the way, we simply replace the rows and
	// cells.

	std::generate(lock->row_infos.begin()+row_number,
		      lock->row_infos.begin()+row_number+n,
		      [&]
		      {
			      list_row_info_t r;

			      r.status_change_callback=lm->next_callback();

			      return r;
		      });

	std::copy(texts.begin(), texts.end(),
		  lock->cells.begin()+row_number*columns);

	// Recalculate and redraw everything.

	lock->recalculation_needed=true;
	lock->full_redraw_needed=true;
}

void textlistObj::implObj::replace_all_rows(const textlistlayoutmanager &lm,
					    const std::vector<list_item_param>
					    &items)
{
	auto texts=list_style.create_cells(items, *this);

	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	current_element(lock)={};
	current_keyed_element(lock)={};

	// Clear out everything, then use insert_rows().

	lock->row_infos.clear();
	lock->cells.clear();
	for (auto &column_widths:lock->column_widths)
		column_widths.clear();
	insert_rows(lm, ll, 0, texts);
}

void textlistObj::implObj::remove_rows(listimpl_info_t::lock &lock,
				       size_t row_number,
				       size_t count)
{
	removing_rows(lock, row_number, count);

	lock->row_infos.erase(lock->row_infos.begin()+row_number,
			      lock->row_infos.begin()+row_number+count);

	lock->cells.erase(lock->cells.begin()+row_number*columns,
			  lock->cells.begin()+(row_number+count)*columns);
}

void textlistObj::implObj
::removing_rows(listimpl_info_t::lock &lock,
		size_t row,
		size_t count)
{
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
	lock->recalculation_needed=true;
	lock->full_redraw_needed=true;
}

void textlistObj::implObj::recalculate(IN_THREAD_ONLY)
{
	listimpl_info_t::lock lock{textlist_info};

	if (!lock->recalculation_needed)
		return;

	recalculate(IN_THREAD, lock);
}

void textlistObj::implObj::calculate_column_widths(IN_THREAD_ONLY,
					      listimpl_info_t::lock &lock)
{
	lock->calculated_column_widths.clear();

	lock->calculated_column_widths.reserve(columns);
	lock->total_column_width=0;

	for (auto &column_widths:lock->column_widths)
	{
		dim_t maximum_width{0};

		if (!column_widths.empty())
			maximum_width=*column_widths.begin();

		lock->calculated_column_widths.push_back(maximum_width);
		lock->total_column_width=
			dim_t::truncate(lock->total_column_width+maximum_width);
	}

	if (lock->total_column_width==dim_t::infinite())
		lock->total_column_width=dim_t::infinite()-1;
}

void textlistObj::implObj::initialize(IN_THREAD_ONLY)
{
	auto screen=get_screen()->impl;
	auto current_theme=*current_theme_t::lock{screen->current_theme};

	listimpl_info_t::lock lock{textlist_info};

	separator_border->theme_updated(IN_THREAD, current_theme);

	recalculate_with_new_theme(IN_THREAD, lock);
	superclass_t::initialize(IN_THREAD);
}

void textlistObj::implObj::theme_updated(IN_THREAD_ONLY,
					 const defaulttheme &new_theme)
{
	listimpl_info_t::lock lock{textlist_info};

	for (const auto &cell:lock->cells)
		cell->cell_theme_updated(IN_THREAD, new_theme);

	separator_border->theme_updated(IN_THREAD, new_theme);
	recalculate_with_new_theme(IN_THREAD, lock);
	superclass_t::theme_updated(IN_THREAD, new_theme);
}

void textlistObj::implObj::recalculate_with_new_theme(IN_THREAD_ONLY,
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

void textlistObj::implObj::recalculate(IN_THREAD_ONLY,
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

	for (size_t i=0; i<n; ++i, ++row)
	{
		row->y=y;

		if (!row->size_computed)
		{
			// This is a new row that's not been linked into
			// column_widths yet.

			row->height=0;

			auto cell_iter=lock->cells.begin()+i*columns;

			auto calculated_column_width=
				lock->calculated_column_widths.begin();

			bool entirely_empty=true;

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

				if (! (*cell_iter)->cell_is_empty())
					entirely_empty=false;

				(*cell_iter)->column_iterator=
					column_widths.insert(horiz.preferred());
				(*cell_iter)->height=vert.preferred();

				if (row->height < vert.preferred())
					row->height=vert.preferred();

				++cell_iter;
			}
			row->size_computed=true;

			if (entirely_empty)
			{
				// This row becomes a separator line.

				row->row_type=row->separator;
				row->height=separator_border->border(IN_THREAD)
					->calculated_border_height;
			}
		}
		y=coord_t::truncate(y+row->height);
		y=coord_t::truncate(y+v_padding_times_two);
	}

	calculate_column_widths(IN_THREAD, lock);
	calculate_column_poswidths(IN_THREAD, lock);

	dim_t width=lock->total_column_width;
	dim_t height=dim_t::truncate(y);

	lock->recalculation_needed=false;
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

void textlistObj::implObj::calculate_column_poswidths(IN_THREAD_ONLY,
						       listimpl_info_t::lock
						       &lock)
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
}

void textlistObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	{
		listimpl_info_t::lock lock{textlist_info};

		// If full recalculation is needed to that. Otherwise the only
		// thing we need to do is to calculate_column_poswidths;

		if (lock->recalculation_needed)
			recalculate(IN_THREAD, lock);
		else
			calculate_column_poswidths(IN_THREAD, lock);
	}

	superclass_t::process_updated_position(IN_THREAD);
}

void textlistObj::implObj::do_draw(IN_THREAD_ONLY,
				   const draw_info &di,
				   const rectangle_set &areas)
{
	do_draw(IN_THREAD, di, areas, false);
}

void textlistObj::implObj::redraw_needed_rows(IN_THREAD_ONLY)
{
	const auto &di=get_draw_info(IN_THREAD);

	do_draw(IN_THREAD, di, di.entire_area(), true);
}

void textlistObj::implObj::do_draw(IN_THREAD_ONLY,
				   const draw_info &di,
				   const rectangle_set &areas,
				   bool only_whats_needed)
{
	richtext_draw_boundaries bounds{di, areas};

	listimpl_info_t::lock lock{textlist_info};

	if (lock->recalculation_needed)
		recalculate(IN_THREAD, lock);

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

	if (lock->recalculation_needed || lock->full_redraw_needed)
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

void textlistObj::implObj::redraw_rows(IN_THREAD_ONLY,
				       listimpl_info_t::lock &lock,
				       size_t row_number1)
{
	redraw_rows(IN_THREAD, lock, row_number1, row_number1, false);
}

void textlistObj::implObj::redraw_rows(IN_THREAD_ONLY,
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


rectangle textlistObj::implObj::do_draw_row(IN_THREAD_ONLY,
					    const draw_info &di,
					    richtext_draw_boundaries &bounds,
					    listimpl_info_t::lock &lock,
					    size_t row_number,
					    bool make_sure_row_is_visible)
{
	auto &r=lock->row_infos.at(row_number);

	r.redraw_needed=false;

	if (r.row_type == r.separator)
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

	if (r.selected)
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

rectangle textlistObj::implObj::do_draw_row(IN_THREAD_ONLY,
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
				     r.row_type == r.disabled,
				     bounds);

		++cell;
	}

	auto to_clear=subtract(rectangle_set{{entire_row}},
			       drawn_columns);

	superclass_t::do_draw(IN_THREAD, di, to_clear);

	return entire_row; // What we just drew.
}


void textlistObj::implObj::report_motion_event(IN_THREAD_ONLY,
					       const motion_event &me)
{
	superclass_t::report_motion_event(IN_THREAD, me);

	if (me.y < 0) // Shouldn't happen.
		return;

	listimpl_info_t::lock lock{textlist_info};

	if (lock->recalculation_needed)
		recalculate(IN_THREAD, lock);

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
	    iter->selectable())
		set_current_element(IN_THREAD, lock, iter-b, false);
	else
		unset_current_element(IN_THREAD, lock);
}

bool textlistObj::implObj::process_key_event(IN_THREAD_ONLY,
					     const key_event &ke)
{
	listimpl_info_t::lock lock{textlist_info};

	if (lock->recalculation_needed)
		recalculate(IN_THREAD, lock);

	if (process_key_event(IN_THREAD, ke, lock))
		return true;

	return superclass_t::process_key_event(IN_THREAD, ke);
}

bool textlistObj::implObj::process_key_event(IN_THREAD_ONLY,
					     const key_event &ke,
					     listimpl_info_t::lock &lock)
{
	if (lock->row_infos.empty())
		return false;

	if (ke.unicode == ' ' || ke.unicode == '\n')
	{
		if (!ke.keypress)
		{
			is_key_or_button_down=false;
		}
		else
		{
			if (current_element(lock))
				is_key_or_button_down=true;
		}

		if (current_element(lock))
			redraw_rows(IN_THREAD, lock,
				    current_element(lock).value());
		if (!is_key_or_button_down)
			click(IN_THREAD, lock, &ke);
		return true;
	}

	if (!ke.keypress)
		return false;

	if (lock->row_infos.size() == 0)
		return false;

	size_t next_row;

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
textlistObj::implObj::move_up_by(listimpl_info_t::lock &lock,
				 size_t howmuch)
{
	if (!current_keyed_element(lock))
		return {};

	auto next_row=current_keyed_element(lock).value();

	if (next_row > howmuch)
		next_row-=howmuch;
	else
		next_row=0;

	while (!lock->row_infos.at(next_row).selectable())
	{
		if (next_row == 0)
			return {};
		--next_row;
	}

	return next_row;
}

std::optional<size_t>
textlistObj::implObj::move_down_by(listimpl_info_t::lock &lock,
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

		if (lock->row_infos.at(next_row).selectable())
			break;

		++next_row;
	}
	return next_row;
}

bool textlistObj::implObj::process_button_event(IN_THREAD_ONLY,
						const button_event &be,
						xcb_timestamp_t timestamp)
{
	// Forward the event to the parent class first. This is so that the
	// button click propagates to the focusable element, and set the
	// input focus to this element.

	bool flag=superclass_t::process_button_event(IN_THREAD, be, timestamp);

	if (be.button == 1)
	{
		listimpl_info_t::lock lock{textlist_info};

		if (lock->recalculation_needed)
			recalculate(IN_THREAD, lock);

		if (current_element(lock))
		{
			is_key_or_button_down=be.press;
			redraw_rows(IN_THREAD, lock,
				    current_element(lock).value());

			if (!is_key_or_button_down)
				click(IN_THREAD, lock, &be);

			flag=true;
		}
	}
	return flag;
}

void textlistObj::implObj::pointer_focus(IN_THREAD_ONLY)
{
	superclass_t::pointer_focus(IN_THREAD);

	listimpl_info_t::lock lock{textlist_info};

	if (lock->recalculation_needed)
		recalculate(IN_THREAD, lock);

	if (!current_pointer_focus(IN_THREAD))
		unset_current_element(IN_THREAD, lock);
}

void textlistObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	superclass_t::keyboard_focus(IN_THREAD);

	listimpl_info_t::lock lock{textlist_info};

	if (lock->recalculation_needed)
		recalculate(IN_THREAD, lock);

	if (!current_keyboard_focus(IN_THREAD))
		unset_current_element(IN_THREAD, lock);
}

void textlistObj::implObj::unset_current_element(IN_THREAD_ONLY,
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

void textlistObj::implObj::set_current_element(IN_THREAD_ONLY,
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

void textlistObj::implObj::click(IN_THREAD_ONLY,
				 listimpl_info_t::lock &lock,
				 const callback_trigger_t &trigger)
{
	if (!current_element(lock))
		return;

	textlist_container->invoke_layoutmanager
		([&, row_number=current_element(lock).value()]
		 (const auto &lm)
		 {
			 textlistlayoutmanager tlm=lm->create_public_object();

			 tlm->autoselect(row_number, trigger);
		 });
}

void textlistObj::implObj::autoselect(const textlistlayoutmanager &lm,
				      size_t i,
				      const callback_trigger_t &trigger)
{
	listimpl_info_t::lock lock{textlist_info};

	busy_impl yes_i_am{*this};

	lock->selection_type(lm, i, trigger, yes_i_am);
}

void textlistObj::implObj::selected(const textlistlayoutmanager &lm,
				    size_t i,
				    bool selected_flag,
				    const callback_trigger_t &trigger)
{
	list_lock ll{lm};

	listimpl_info_t::lock &lock=ll;

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"), i));

	auto &r=lock->row_infos.at(i);

	if (r.selected == selected_flag)
		return;

	r.selected=selected_flag;
	r.redraw_needed=true;

	list_style.selected_changed(&lock->cells.at(i*columns),
				    selected_flag);
	schedule_row_redraw(lock);

	notify_callbacks(lm, ll, r, i, selected_flag, trigger);
}

void textlistObj::implObj::notify_callbacks(const textlistlayoutmanager &lm,
					    list_lock &ll,
					    const list_row_info_t &r,
					    size_t i,
					    bool selected_flag,
					    const callback_trigger_t &trigger)
{
	listimpl_info_t::lock &lock=ll;

	if (r.status_change_callback)
		try {
			r.status_change_callback(ll, i, selected_flag);
		} CATCH_EXCEPTIONS;

	busy_impl yes_i_am{*this};
	try {
		lock->selection_changed(lm, i, selected_flag, trigger,
					yes_i_am);
	} CATCH_EXCEPTIONS;
}

bool textlistObj::implObj::enabled(size_t i)
{
	listimpl_info_t::lock lock{textlist_info};

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"), i));

	return lock->row_infos.at(i).row_type == list_row_info_t::enabled;
}

void textlistObj::implObj::enabled(size_t i, bool flag)
{
	listimpl_info_t::lock lock{textlist_info};

	if (i >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Row %1% does not exist"), i));

	auto &r=lock->row_infos.at(i);

	if (r.row_type != r.enabled && r.row_type != r.disabled)
		return; // Don't touch separators.

	auto new_type=flag ? r.enabled:r.disabled;

	if (r.row_type == new_type)
		return;

	r.row_type=new_type;
	r.redraw_needed=true;

	schedule_row_redraw(lock);
}

void textlistObj::implObj::schedule_row_redraw(listimpl_info_t::lock &lock)
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

size_t textlistObj::implObj::size()
{
	listimpl_info_t::lock lock{textlist_info};

	return lock->row_infos.size();
}

bool textlistObj::implObj::selected(size_t i)
{
	listimpl_info_t::lock lock{textlist_info};

	return i < lock->row_infos.size() && lock->row_infos.at(i).selected;
}

std::optional<size_t> textlistObj::implObj::selected()
{
	listimpl_info_t::lock lock{textlist_info};

	size_t i=0;

	for (const auto &r:lock->row_infos)
	{
		if (r.selected)
			return i;
		++i;
	}

	return {};
}

std::vector<size_t> textlistObj::implObj::all_selected()
{
	std::vector<size_t> all;

	listimpl_info_t::lock lock{textlist_info};

	size_t i=0;

	for (const auto &r:lock->row_infos)
	{
		if (r.selected)
			all.push_back(i);
		++i;
	}

	return all;
}

void textlistObj::implObj::unselect(const textlistlayoutmanager &lm)
{
	bool unselected=false;

	list_lock ll{lm};
	listimpl_info_t::lock &lock=ll;

	callback_trigger_t internal;

	size_t i=0;

	for (auto &r:lock->row_infos)
	{
		if (r.selected)
		{
			r.selected=false;
			r.redraw_needed=true;

			list_style.selected_changed(&lock->cells.at(i*columns),
						    false);
			unselected=true;
			notify_callbacks(lm, ll, r, i, false, internal);
		}
		++i;
	}

	if (unselected)
		schedule_row_redraw(lock);
}

LIBCXXW_NAMESPACE_END
