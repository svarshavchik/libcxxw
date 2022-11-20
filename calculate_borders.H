/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef calculate_borders_h
#define calculate_borders_h

#include "metrics_grid_pos.H"
#include "x/w/types.H"
#include <x/number_hash.H>
#include <x/functional.H>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <limits>

LIBCXXW_NAMESPACE_START

//! In the collapsed border number, cell positions are physical odd grid coords

#define CALCULATE_BORDERS_COORD(n) ((n)*2+1)

//! And border positions are even grid coords
#define IS_BORDER_RESERVED_COORD(n) ( ((n) % 2) == 0)

//! Vertical spanning is computed incrementally. Increment by one logical row.
#define CALCULATE_BORDERS_INCR_SPAN(n) ((n)=(n)+2)
#define CALCULATE_BORDERS_DECR_SPAN(n) ((n)=(n)-2)

//! Given a border coordinate, translate to logical row or column number.

#define BORDER_COORD_TO_ROWCOL(n) ((n)/2)

//! Given a non-border coordinate, translate to logical row or column number.

//! Same thing, really.

#define NONBORDER_COORD_TO_ROWCOL(n) ((n)/2)

//! Invoke v_lambda passed to calculate_borders()

//! The logical calls to v_lambda pass logical rows and column. This
//! translates to actual physical rows and columns

#define CALCULATE_BORDERS_INVOKE_HLAMBDA(cell1,cell2,row,start,end) \
	h_lambda( (cell1), (cell2),					\
		CALCULATE_BORDERS_COORD(row)-1,			\
		CALCULATE_BORDERS_COORD(start),			\
		  CALCULATE_BORDERS_COORD(end))

//! Merge extended vertical borders.

//! Helper class used by calculate_borders()
//!
//! We process one row at a time. Although calculate_borders() is smart
//! enough to emit multicolumn horizontal borders, where possible,
//! calculate_borders() emits one row's worth of vertical borders. Here
//! we attempt to combine vertical borders across multiple rows using
//! the following approach.
//!
//! This structure collects single-row vertical borders from a single
//! row, first, instead of calling v_lambda().
//!
//! When processing the next row:
//!
//! 1. calculate_borders() generates horizontal borders first. If the
//! horizontal border begins or ends at a vertical border, the
//! vertical border is going to form a corner, so it is removed from
//! this cache, and forwarded to v_lambda().
//!
//! 2. Otherwise, if there is an existing vertical border from the
//! previous row in the same column, WITH THE SAME LEFT AND RIGHT
//! cell, its extended into this row.
//!
//! 3. Otherwise this row's vertical boder is added to the cache.
//!
//! 4. All vertical borders that did not get extended, they get
//! forwarded to v_lambda().

template<typename cell_type>
struct LIBCXX_HIDDEN vertical_border_cache_t {

	//! Vertical callback type.
	typedef function<void (cell_type *, cell_type *,
			       metrics::grid_xy,
			       metrics::grid_xy,
			       metrics::grid_xy)> callback_t;

	//! The real vertical callback.
	const callback_t &callback;

	//! Constructor
	vertical_border_cache_t(const callback_t &callback)
		: callback(callback)
	{
	}

	//! Vertical border specifications.
	struct border_info {
		cell_type *left;
		cell_type *right;
		metrics::grid_xy start;
		metrics::grid_xy end;
	};

	//! A container for collected vertical borders.

	//! Container key is the column number.
	typedef std::unordered_map<metrics::grid_xy, border_info> v_borders_t;

	//! Current collection of vertical borders.
	v_borders_t v_borders;

	//! Report a vertical border for a single row.

	void v(cell_type *left,
	       cell_type *right,
	       metrics::grid_xy column_number,
	       metrics::grid_xy row_number)
	{
		// Let's see if we can find a vertical border from the
		// previous row. That border's left and right must match
		// left and right.

		auto p=v_borders.find(column_number);

		if (p != v_borders.end() &&
		    ( (p->second.left == nullptr && left == nullptr) ||
		      (p->second.left != nullptr && left != nullptr &&
		       (*p->second.left)->grid_element ==
		       (*left)->grid_element)) &&
		    ( (p->second.right == nullptr && right == nullptr) ||
		      (p->second.right != nullptr && right != nullptr &&
		       (*p->second.right)->grid_element ==
		       (*right)->grid_element)))
		{
			p->second.end=row_number; // Should be one more.
			return;
		}

		// If there's some other border there, report it. It wasn't
		// extended.

		if (p != v_borders.end())
			emit(p);

		// Save this vertical border, to be check on the next row.

		v_borders.insert({column_number, {left, right, row_number,
						row_number}});
	}

	//! invoke_callback(), then remove this vertical border.

	void emit(typename v_borders_t::iterator iter)
	{
		invoke_callback(iter);
		v_borders.erase(iter);
	}

	//! Invoke the vertical callback given to calculate_borders()

	//! invoke the vertical border callback for a particular entry in
	//! v_borders.
	void invoke_callback(typename v_borders_t::iterator iter)
	{
		callback(iter->second.left,
			 iter->second.right,
			 CALCULATE_BORDERS_COORD(iter->first)-1,
			 CALCULATE_BORDERS_COORD(iter->second.start),
			 CALCULATE_BORDERS_COORD(iter->second.end));
	}

	//! calculate_borders() finished processing a single row

	//! Any vertical borders from a previous row that were not extended
	//! to this row are emitted.

	void end_row(metrics::grid_xy row_number)
	{
		// Find v_borders from a prior row that were not extended,
		// and emit them.

		for (auto b=v_borders.begin(), e=v_borders.end(); b != e;)
		{
			auto p=b;
			++b;
			if (p->second.end == row_number)
				continue; // Extended to next row

			emit(p);
		}
	}

	//! calculate_borders() finished the last row

	//! So, all the vertical borders go out the door.
	void end_last_row()
	{
		for (auto b=v_borders.begin(), e=v_borders.end(); b != e;)
		{
			auto p=b;
			++b;
			emit(p);
		}
	}

	//! calculate_borders() collapsed a horizontal border.

	//! Make sure that any vertical border on either side of the
	//! horizontal border does NOT get extended.

	void h(metrics::grid_xy from,
	       metrics::grid_xy to,
	       metrics::grid_xy current_row)
	{
		h(from, current_row);
		h(to+1, current_row);
	}

	//! Vertical border at the given column cannot be extended.

	//! Because there's a horizontal border on either side of it.

	void h(metrics::grid_xy col,
	       metrics::grid_xy current_row)
	{
		auto p=v_borders.find(col);

		if (p == v_borders.end())
			return;

		if (p->second.end != current_row)
		{
			// This is a border from the previous row that has
			// not been extended yet, so it can simply go out
			// the door, as is, and we close the books.

			emit(p);
			return;
		}

		// If this border started on this row, that's fine.

		if (p->second.start == current_row)
			return;

		// Ok, this border was marked for extension. Pretend that
		// it wasn't extended, and emit it.

		--p->second.end;
		invoke_callback(p);

		// Now, we just FIX UP this border, as if it was just started
		// on this row. That's it.

		p->second.start=++p->second.end;
	}
};

template<typename container_type,
	 typename v_lambda_type,
	 typename h_lambda_type>
metrics::grid_xy calculate_borders(container_type &container,
					   v_lambda_type &&v_lambda,
					   h_lambda_type &&h_lambda)
	LIBCXX_HIDDEN;

//! Calculate collapsed borders between cells.

//! Algorithm used by the grid layout manager to lay out the borders.
//! Implemented as a generic template, so the logic is unit-testable.
//! The first parameter is a container of rows. Each row is a container of
//! cells. Each cell is a pointer-like object that points to the following
//! fields:
//!
//! {type} grid_element.
//! dim_t width;
//! dim_t height;
//!
//! Neither width, nor height, can be 0. grid_element can be any type that
//! implements strict weak ordering.
//!
//! An additional field:
//!
//! pos
//!
//! is expected to be a metrics::grid_pos object. calculate_borders()
//! computes and sets both of its horiz_pos and vert_pos's start and end
//! values.
//!
//! rowspan and colspan should be the same as width and height, except in
//! some edge cases, where it gets adjusted as needed.
//!
//! v_lambda() gets called for each computed vertical border, on a single row.
//! The first parameter is a POINTER to the left-most cell (which will be NULL
//! for the first cell on each row), the second parameter is a POINTER to the
//! right-most cell, which is NULL if the left-most cell is the last cell on
//! the row.
//!
//! The third parameter is the right-most cell's column number, and the last
//! two parameters to v_lambda() are the starting and the ending row numbers
//! spanned by the vertical border.
//!
//! h_lambda() gets called for each computed horizontal border. Its first two
//! parameters are POINTERS to the cell above (which will be NULL for the
//! cells on the first row), and the cell below the horizontal border, which
//! will be NULL for the cells on the last row in the table.
//!
//! The third parameter is the row number of the cell BELOW the horizontal
//! border, followed by two more parameters, the standing and ending column.
//!
//! Note that the respective parameters to these lambdas are not the cell
//! pointers themselves, but a pointer to those cell pointers, which could
//! be NULL.

template<typename container_type,
	 typename v_lambda_type,
	 typename h_lambda_type>
metrics::grid_xy calculate_borders(container_type &container,
				   v_lambda_type &&v_lambda,
				   h_lambda_type &&h_lambda)
{
	typedef typename std::remove_reference<decltype( *(container.begin()->begin()))>::type cell_type;


	auto callback=make_function<void (cell_type *, cell_type *,
					  metrics::grid_xy,
					  metrics::grid_xy,
					  metrics::grid_xy)>
		(std::forward<v_lambda_type>(v_lambda));

	vertical_border_cache_t<cell_type> vcache(callback);

	// Keep track of the cells from the previous row, and how many
	// rows the previous row's cells span. The ones that had one row left
	// to span will have a horizontal border attached to them. Two or
	// more rows result in this row's cells skipping over their columns,
	// generating vertical borders, in addition to the vertical borders
	// between the cells on this row.
	//
	// Afterwards, only the cells that had two or more rows get copied over
	// to the span_map_t for the next row, in addition to all the new
	// cells from this row.

	struct span_info;

	typedef std::unordered_map<metrics::grid_xy, span_info> span_map_t;

	struct span_info {
		dim_t::value_type colspan;
		dim_t::value_type rowspan;

		cell_type *cell;

		// Additional fields that are used to defer generation of
		// vertical borders until after horizontal borders are
		// generated.
		//
		// The chain together a list of columns with deferred
		// vertical borders.

		cell_type *defer_v_previous_cell;
		metrics::grid_xy defer_v_first; // My key.
		span_info *defer_v_next;
	};

	span_map_t spans[2];

	span_map_t *prev_span=&spans[0], *new_span=&spans[1];

	metrics::grid_xy width=0;

	metrics::grid_xy row_number=0;

	// We will naturally prune excessive rowspans by the nature of the
	// fact that we process individual rows one at a time, and the
	// excessive rowspans will get pruned as a natural outcome of this
	// algorithm. We have no such natural protection, so we must calculate
	// the maximum column number we can reference without an overflow.
	//
	// In the collapsed border model there's a column between each
	// possible cell. If a table has N virtual columns, the actual
	// number of columns used will be N*2+1. This algorithm has the
	// border on the left side of a cell column "belonging" to that
	// cell column. Plus one more for the rightmost border.

	const metrics::grid_xy::value_type max_column=
		std::numeric_limits<metrics::grid_xy::value_type>::max()/2-1;

	for (auto &row:container)
	{
		std::swap(prev_span, new_span);

		// We start numbering each cell, starting at column 0.
		// and we start looking at the first column from the prior
		// row.

		metrics::grid_xy col_number=0;

		auto span_iter=prev_span->find(0);

		cell_type *previous_cell=nullptr;

		span_info *defer_v_head=nullptr;
		span_info **defer_v_tail=&defer_v_head;

		for (auto beg=row.begin(), end=row.end() ; beg != end; )
		{
			dim_t::value_type colspan{(*beg)->width};

			// Check for overflow on the column.
			{
				auto next_col=metrics::grid_xy::value_type
					(col_number)+colspan;

				if (next_col > max_column)
				{
					// Somebody wants to play games.
					//
					// This'll fix their red wagon.

					row.erase(beg, end);

					beg=end=row.end();
					continue;
				}
			}

			// Advance span_iter past all the previous row's
			// cells that are now in the rear-view mirror.

			while (span_iter != prev_span->end() &&
			       span_iter->first+span_iter->second.colspan
			       <= col_number)
			{
				span_iter=prev_span->find(span_iter->first
							  + span_iter->second.colspan);
			}

			// Check if the previous row's cell spans into this
			// row.

			if (span_iter != prev_span->end() &&
			    span_iter->second.rowspan > 1)
			{
				if (span_iter->first > col_number)
				{
					// Check a situation with the current
					// cell's width will bump into an
					// active span. In which case we'll
					// just adjust the width so that it
					// does not.

					if (col_number+colspan
					    > span_iter->first)
					{
						colspan=metrics::grid_xy
							::value_type
							(span_iter->first
							 -col_number);
					}
				}
				else
				{
					// The previous row's cell spans
					// into this row.
					//
					// Declare a vertical border from
					// the previosly-seen cell with the
					// spanning cell.
					//
					// But we need to defer all vertical
					// borders until any horizontal borders
					// are emitted. This is part of our
					// contract. So, we jury-rig a
					// stack, in this fashion.

					span_iter->second.defer_v_previous_cell=
						previous_cell;

					// Stitch together a stack:

					span_iter->second.defer_v_next=nullptr;
					span_iter->second.defer_v_first=
						span_iter->first;
					*defer_v_tail=&span_iter->second;
					defer_v_tail=
						&span_iter->second.defer_v_next;

					// This is what the above silliness
					// is punting, until the horizontal
					// borders are done:
					//
					// vcache.v(previous_cell,
					//	    span_iter->second.cell,
					//	    span_iter->first,
					//	    row_number);

					previous_cell=span_iter->second.cell;

					col_number = span_iter->first +
						span_iter->second.colspan;

					span_iter=prev_span
						->find(span_iter->first
						       +
						       span_iter->second
						       .colspan);
					continue;
				}
			}

			// Check for previous row's cell having a horizontal
			// border with this cell.
			else if (span_iter != prev_span->end() &&
				 span_iter->first < col_number + colspan)
			{
				metrics::grid_xy border_from=span_iter->first;
				metrics::grid_xy border_to=span_iter->first
					+ span_iter->second.colspan;

				if (border_from < col_number)
					border_from=col_number;

				if (border_to > col_number+colspan)
					border_to=col_number+colspan;

				CALCULATE_BORDERS_INVOKE_HLAMBDA
					(span_iter->second.cell,
					 &*beg,
					 row_number, border_from, border_to-1);
				vcache.h(border_from, border_to-1,
					 row_number);

				if (span_iter->first+span_iter->second.colspan
				    < col_number + colspan)
				{
					span_iter=prev_span
						->find(span_iter->first
						       +span_iter
						       ->second.colspan);
					continue;
				}
			}

			if (row_number == 0)
			{
				CALCULATE_BORDERS_INVOKE_HLAMBDA
					(nullptr,
					 &*beg,
					 row_number,
					 col_number,
					 col_number+(colspan-1));
				vcache.h(col_number,
					 col_number+(colspan-1), row_number);
			}

			// Ok, the horizontal borders are done, time to
			// generate the vertical ones that have stacked up
			// to this point (see above)

			for (auto defer_v=defer_v_head; defer_v;
			     defer_v=defer_v->defer_v_next)
			{

				auto previous_cell=
					defer_v->defer_v_previous_cell;

				vcache.v(previous_cell,
					 defer_v->cell,
					 defer_v->defer_v_first,
					 row_number);
			}

			// And reset it.
			defer_v_head=nullptr;
			defer_v_tail=&defer_v_head;

			// Ok, with the preliminaries over, time to process
			// this cell. The first order of business is to set
			// its column number, and declare a vertical border
			// with the previous cell.

			(*beg)->pos->horiz_pos.start=col_number; // TRANSLATED BELOW.

			(*beg)->pos->horiz_pos.end=col_number+(colspan-1);

			vcache.v(previous_cell,
				 &*beg,
				 col_number,
				 row_number);
			previous_cell= &*beg;

			++beg;

			// Finally, set the next column number, here.
			col_number += colspan;
		}

		if (previous_cell)
		{
			vcache.v(previous_cell,
				 nullptr,
				 col_number,
				 row_number);
		}

		vcache.end_row(row_number);

		// Compute the width.

		if (col_number > width)
			col_number=width;

		// Now, calculate a new spanning row, for the next iteration.
		//
		// Start by copying all previous row's cells that still have
		// another row to span, decrementing their rowspan.

		new_span->clear();

		for (const auto &continue_span:*prev_span)
		{
			if (continue_span.second.rowspan <= 1)
				continue;

			CALCULATE_BORDERS_INCR_SPAN
				( (*continue_span.second.cell)
				  ->pos->vert_pos.end);
			new_span->insert({continue_span.first,
					{
						continue_span.second.colspan,
						metrics::grid_xy::value_type
							(continue_span.second
							 .rowspan-1),
						continue_span.second.cell
							}});
		}

		// Then add everything from this row.

		for (auto &c:row)
		{
			new_span->insert({c->pos->horiz_pos.start,
					{
						dim_t::value_type(c->width),
						dim_t::value_type(c->height),
						&c,
					}});
			// Translate from virtual to collapsed
			c->pos->vert_pos.start=
				c->pos->vert_pos.end=
				CALCULATE_BORDERS_COORD(row_number);
			c->pos->horiz_pos.start=
				CALCULATE_BORDERS_COORD(c->pos->
							horiz_pos.start);
			c->pos->horiz_pos.end=
				CALCULATE_BORDERS_COORD(c->pos->
							horiz_pos.end);

		}
		++row_number;
	}

	vcache.end_last_row();
	// Bottom borders.

	metrics::grid_xy col_number=0;

	for (auto iter=new_span->find(col_number),
		     end=new_span->end(); iter != end;
	     iter=new_span->find(col_number))
	{
		CALCULATE_BORDERS_INVOKE_HLAMBDA
			(iter->second.cell,
			 nullptr,
			 row_number,
			 iter->first,
			 iter->first+(iter->second.colspan-1)
			 );

		col_number += iter->second.colspan;
	}
	return width;
}

LIBCXXW_NAMESPACE_END;

#endif
