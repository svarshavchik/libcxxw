/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/pixmap.H"
#include "x/w/drawable.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtext_draw_info.H"
#include "richtext/richtext_draw_boundaries.H"
#include "richtext/richtext_alteration_config.H"
#include "richtext/richtextfragment_render.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextiterator.H"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtext_range.H"

#include "x/w/impl/draw_info.H"
#include "x/w/impl/element_draw.H"
#include <x/sorted_range.H>
#include "screen.H"
#include "picture.H"
#include <functional>

LIBCXXW_NAMESPACE_START

void richtextObj::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	impl_t::lock lock{impl};

	(*lock)->theme_updated(IN_THREAD, new_theme);
}

void richtextObj::full_redraw(ONLY IN_THREAD,
			      element_drawObj &element,
			      const richtext_draw_info &rdi,
			      const draw_info &di,
			      const rectarea &areas)
{
	richtext_draw_boundaries draw_bounds{di, areas};
	clip_region_set clipped{IN_THREAD, element.get_window_handler(), di};

	full_redraw(IN_THREAD, element, rdi, di, clipped, draw_bounds);
}

void richtextObj::full_redraw(ONLY IN_THREAD,
			      element_drawObj &element,
			      const richtext_draw_info &rdi,
			      const draw_info &di,
			      clip_region_set &clipped,
			      richtext_draw_boundaries &draw_bounds)
{
	draw(IN_THREAD, element, rdi, di, clipped,
	     make_function<bool (richtextfragmentObj *)>
	     ([]
	      (richtextfragmentObj *f)
	      {
		      // Record where this fragment was (about to be) redrawn.
		      f->redrawn_y_position=f->y_position();
		      return true;
	      }),
	     true, draw_bounds);
}

void richtextObj::redraw_whatsneeded(ONLY IN_THREAD,
				     element_drawObj &element,
				     const richtext_draw_info &rdi,
				     const draw_info &di)
{
	redraw_whatsneeded(IN_THREAD, element, rdi, di, di.entire_area());
}

void richtextObj::redraw_whatsneeded(ONLY IN_THREAD,
				     element_drawObj &element,
				     const richtext_draw_info &rdi,
				     const draw_info &di,
				     const rectarea &areas)
{
	richtext_draw_boundaries draw_bounds{di, areas};
	clip_region_set clipped{IN_THREAD, element.get_window_handler(), di};

	draw(IN_THREAD, element, rdi, di, clipped,
	     make_function<bool (richtextfragmentObj *)>
	     ([]
	      (richtextfragmentObj *f)
	      {
		      bool flag=f->redraw_needed;
		      f->redraw_needed=false;

		      // Even if redraw_needed was false, if another row
		      // was inserted above this one, this row's position
		      // has changed. redraw_needed gets set only if something
		      // about this specific row has changed, and won't be
		      // set in that case. So, redraw this row, in any case.

		      coord_t y_pos=coord_t::truncate(f->y_position());
		      if (y_pos != f->redrawn_y_position)
			      flag=true;
		      f->redrawn_y_position=y_pos;
		      return flag;
	      }),
	     false, draw_bounds);
}

void richtextObj::redraw_between(ONLY IN_THREAD,
				 element_drawObj &element,
				 const richtextiterator &a,
				 const richtextiterator &b,
				 const richtext_draw_info &rdi,
				 const draw_info &di)
{
	assert_or_throw(a->my_richtext == b->my_richtext &&
			a->my_richtext == richtext(this),
			"Internal error: invalid iterators passed to redraw_between().");

	bool first=true;
	size_t a_index=0;
	size_t b_index=0;

	richtext_draw_boundaries draw_bounds{di, di.entire_area()};

	clip_region_set clipped{IN_THREAD, element.get_window_handler(), di};

	draw(IN_THREAD, element, rdi, di, clipped,
	     make_function<bool (richtextfragmentObj *)>
	     ([&]
	      (richtextfragmentObj *f)
	      {
		      // This is now executing while holding a lock on the
		      // implementation object. We cannot access the fragments
		      // until this happens.

		      if (first)
		      {
			      assert_or_throw(a->my_location->my_fragment &&
					      b->my_location->my_fragment,
					      "Internal error: null fragment "
					      " in redraw_between");

			      first=false;
			      a_index=a->my_location->my_fragment->index();
			      b_index=b->my_location->my_fragment->index();

			      if (a_index > b_index)
				      std::swap(a_index, b_index);
		      }

		      auto i=f->index();

		      return ( a_index <= i && i <= b_index );
	      }),
	     false,
	     draw_bounds);
}

void richtextObj::draw(ONLY IN_THREAD,
		       element_drawObj &element,
		       const richtext_draw_info &rdi,
		       const draw_info &di,
		       clip_region_set &clipped,
		       const function<bool (richtextfragmentObj *)>
		       &redraw_fragment,
		       bool clear_padding,
		       richtext_draw_boundaries &draw_bounds)
{
	if (draw_bounds.nothing_to_draw())
		return;

	assert_or_throw(draw_bounds.draw_bounds.x >= 0 &&
			draw_bounds.draw_bounds.y >= 0,
			"Bounding rectangle cannot start on a negative coordinate");

	// Check if the rich text should have a trailing ellipsis if it's
	// truncated.

	if (rdi.richtext_alteration.ellipsis)
	{
		impl_t::lock lock{rdi.richtext_alteration.ellipsis->impl};

		// We use ellipsis' first fragment only. Too bad.

		if (!(*lock)->paragraphs.empty())
		{
			auto &p= *(*lock)->paragraphs.get_paragraph(0);

			if (!p->fragments.empty())
			{
				auto &f=*p->fragments.get_iter(0);

				draw_with_ellipsis(IN_THREAD, element,
						   rdi, di, clipped,
						   redraw_fragment,
						   clear_padding,
						   draw_bounds, &*f);
				return;
			}
		}
	}

	draw_with_ellipsis(IN_THREAD, element, rdi, di, clipped,
			   redraw_fragment,
			   clear_padding,
			   draw_bounds, nullptr);
}

// Data and logic for rendering a single fragment of text

// draw_fragment/draw_with_ellipsis calls this to draw_using_scratch_buffer
// each fragment (line) of the label.
//
// If this is a truncated label with ellipsis, this also is used to draw the
// trailing ellipsis separately, after fudging the draw_bounds.

struct LIBCXX_HIDDEN richtextObj::draw_fragment_info {

	// The main information that's needed to draw each fragment:
	element_drawObj &element;
	const draw_info &di;
	clip_region_set &clipped;
	richtext_draw_boundaries &draw_bounds;

	// If the richtext-draw_info parameter to the drawing function
	// specifies that this label has a selection (this is the label
	// code used by editorObj::implObj to draw the contents of the
	// input field, and the input field has a selected, highlighted
	// chunk.
	//
	// This is translated into starting and ending fragment index number
	// and the range in each fragment where the selection starts and
	// ends, and has_selection gets set to true. selection_start_range
	// and selection_end range are defined as character indexes in the
	// corresponding fragment, as the starting index and one past the
	// ending index.
	//
	// This is ignored when has_selection=false
	size_t selection_start_fragment_index=0;
	std::tuple<size_t, size_t> selection_start_range{0, 0};

	size_t selection_end_fragment_index=0;
	std::tuple<size_t, size_t> selection_end_range{0, 0};

	bool has_selection=false;

	// The index # (the line #) of the fragment being drawn.
	size_t fragment_index;

	// Position in the scratch buffer where the baseline of the drawn
	// text is.
	//
	// This is each label's above_baseline value. If a trailing ellipsis
	// gets drawn, the fragment_ypos value remains unchanged, drawing
	// the ellipsis with the same baseline.
	coord_t fragment_ypos;

	// While drawing fragments, we capture the height of the scratch
	// pixmap we're using at the first opportunity.

	dim_t scratch_height=0;

	// The starting y coordinate and the height of the line. This is used
	// for correctly compositing the gradient background color.

	coord_squared_t y_position=0;
	dim_t height=0;

	void draw_fragment(ONLY IN_THREAD, richtextfragmentObj *f);
};

void richtextObj::draw_fragment_info::draw_fragment(ONLY IN_THREAD,
						    richtextfragmentObj  *f)
{
	if (draw_bounds.nothing_to_draw())
		return;

	element.draw_using_scratch_buffer
		(IN_THREAD,
		 [&]
		 (const picture &scratch_picture,
		  const pixmap &scratch_pixmap,
		  const gc &scratch_gc)
		 {
			 richtextfragmentObj::render_info render_info
				 {
				  scratch_picture,
				  di.window_background_color,
				  di.background_x,
				  di.background_y,
				  coord_t::truncate
				  (di.absolute_location.x +
				   draw_bounds.position.x),
				  coord_t::truncate
				  (di.absolute_location.y +
				   draw_bounds.position.y),

				  draw_bounds.draw_bounds.width,
				  dim_t::truncate(draw_bounds
						  .draw_bounds
						  .x),

				  fragment_ypos,
				 };

			 // If we're drawing a selection,
			 // figure out which part of it is
			 // on this line.

			 if (has_selection &&
			     fragment_index >= selection_start_fragment_index &&
			     fragment_index <= selection_end_fragment_index)
			 {
				 size_t from=0;
				 size_t to=f->string.size();

				 if (fragment_index ==
				     selection_start_fragment_index)
					 std::tie(from, to)=
						 selection_start_range;
				 else if (fragment_index ==
					  selection_end_fragment_index)
					 std::tie(from, to)=
						 selection_end_range;

				 render_info.selection_start=from;
				 render_info.selection_end=to;
			 }

			 f->render(IN_THREAD, render_info);

			 scratch_height=scratch_pixmap->get_height();

		 },
		 rectangle{coord_t::truncate(draw_bounds.draw_bounds.x +
					     draw_bounds.position.x),
				   coord_t::truncate(coord_t::truncate
						     (y_position) +
						     draw_bounds
						     .position.y),
				   draw_bounds.draw_bounds.width,
				   height},
		 di, di,
		 clipped);
}

namespace {
#if 0
}
#endif

// Determine the parts of the first and last line in a selection range.
//
// When there is a selection the fragments between the first and the
// last fragment in the selection range get highlighted in their entirety,
// that's the easy part.
//
// The first and the last fragment in the selection range have only a part
// of the fragment highlighted. Use the logic in richtext_range to define
// the highlighted parts of the first and the last fragment in the selection
// range.
//
// The constructor gets the richtext and the start/end of the selection
// range. This is forwarded to richtext_range.
//
// richtext_range's location_a and location_b define the first and the
// last fragment in the selection range.
//
// location_a_range and location_b_range define the portion of the
// corresponding location which is highlighted. Each one is a tuple of
// first, and one-past the last highlighted characters.
//
// When location_a and location_b are the same, the location_a_range
// gives the range of the highlighted characters,m and location_b
// specifies an empty range.

struct selected_range_info : public richtext_range {

	selected_range_info(const ref<richtext_implObj> &impl,
			    const richtextcursorlocation &a,
			    const richtextcursorlocation &b)
		: richtext_range{impl->paragraph_embedding_level, a, b}
	{
		line_range.reserve(3);

		if (complete_line())
		{
			// We already collected the needed information.
			location_a_range=line_range.range();
			return;
		}

		// Ok, call line() to collect the needed info for location_a
		line(location_a->my_fragment);
		location_a_range=line_range.range();

		// Clear it, do the same for location_b
		line_range.clear();
		line(location_b->my_fragment);
		location_b_range=line_range.range();
	}

	// Use the sorted_range collect the ranges that range() receives.

	struct selection_range {
		size_t begin;
		size_t end;
	};

	mutable sorted_range<selection_range> line_range;

	std::tuple<size_t, size_t> location_a_range{0, 0},
		location_b_range{0, 0};

	void range(const richtextstring &other,
		   size_t start,
		   size_t n) const override
	{
		line_range.add(selection_range{start, start+n});
	}
};

#if 0
{
#endif
}

void richtextObj::draw_with_ellipsis(ONLY IN_THREAD,
				     element_drawObj &element,
				     const richtext_draw_info &rdi,
				     const draw_info &di,
				     clip_region_set &clipped,
				     const function<bool (richtextfragmentObj
							  *)>
				     &redraw_fragment,
				     bool clear_padding,
				     richtext_draw_boundaries &draw_bounds,
				     richtextfragmentObj *ellipsis_fragment)
{
	impl_t::lock lock{impl};

	clipped.draw_as_disabled=rdi.draw_as_disabled;

	richtextfragmentObj *f=nullptr;

	if (!(*lock)->paragraphs.empty())
	{
		size_t y_pos=draw_bounds.draw_bounds.y < 0 ? 0:
			(coord_t::value_type)(draw_bounds.draw_bounds.y);

		auto frag=(*lock)->find_fragment_for_y_position(y_pos);

		if (frag)
			f=&*frag;
	}

	// It's unlikely, but possible that the container gave us
	// more height than we'll actually draw. Keep track of the ending
	// y coordinate that was rendered.
	coord_t y=draw_bounds.draw_bounds.y;

	auto ending_y_position=draw_bounds.draw_bounds.y + draw_bounds.draw_bounds.height;

	// Determine if there's a current selection.

	draw_fragment_info dfi{element, di, clipped, draw_bounds};

	if (rdi.selection_start && rdi.selection_end)
	{
		auto a=rdi.selection_start->my_location;
		auto b=rdi.selection_end->my_location;

		auto cmp=a->compare(*b);

		if (cmp)
		{
			selected_range_info info{ *lock, a, b};

			dfi.selection_start_fragment_index=
				info.location_a->my_fragment->index();
			dfi.selection_start_range=info.location_a_range;

			dfi.selection_end_fragment_index=
				info.location_b->my_fragment->index();
			dfi.selection_end_range=info.location_b_range;
			if (cmp > 0)
				std::swap(a, b);

			dfi.has_selection=true;
		}
	}

	// Now draw each fragment. Loop iteration advances y by each
	// fragment's height.

	dfi.fragment_index=f ? f->index():0;

	rectangle original_position=draw_bounds.position;

	for (; f; (f=f->next_fragment()), ++dfi.fragment_index)
	{
		// We rely on the fragments' y_position()s being accurate.

		dfi.y_position=coord_squared_t{f->y_position()};

		if (ending_y_position <= dfi.y_position)
			break;

		dfi.height=f->height();

		y=coord_t::truncate(dfi.y_position+dfi.height);

		if (!redraw_fragment(f))
			continue;

		rectangle ellipsis_rectangle;

		// Fragment is smaller than the bounds it's being drawn in?

		if (ellipsis_fragment && f->width > draw_bounds.position.width)
		{
			// Subtract the width of the ellipsis from the
			// width of the drawing boundaries. We will cut off
			// the fragment at this point.

			dim_t truncate_at=
				ellipsis_fragment->width
				> draw_bounds.position.width
				? dim_t{0}:draw_bounds.position.width
				  - ellipsis_fragment->width;

			// Copy the draw bounds to the ellipsis_rectangle,
			// then adjust the ellipsis_rectangle to start at
			// truncate_at.
			ellipsis_rectangle=draw_bounds.position;

			ellipsis_rectangle.width=
				ellipsis_rectangle.width-truncate_at;
			ellipsis_rectangle.x=coord_t::truncate
				(ellipsis_rectangle.x+truncate_at);


			// Ok, if we decided that we'll be drawing a
			// trailing ellipsis on this row, adjust the
			// fragment's bounds accordingly.
			if (ellipsis_rectangle.width > 0 &&
			    ellipsis_rectangle.height > 0)
			{
				rectangle adjusted_draw_rectangle
					{original_position};

				adjusted_draw_rectangle.width=truncate_at;

				draw_bounds.position_at
					(adjusted_draw_rectangle);
			}
		}

		// Y position for rendering the fragment in the scratch
		// buffer.
		//
		// This gets set to fragment->above_baseline
		dfi.fragment_ypos=coord_t::truncate(f->above_baseline);

		dfi.draw_fragment(IN_THREAD, f);

		// Did we decide to draw the ellipsis here?
		if (ellipsis_rectangle.width > 0 &&
		    ellipsis_rectangle.height > 0)
		{
			// Fudge our coordinates for the ellipsis.
			draw_bounds.position_at(ellipsis_rectangle);

			auto has_selection=dfi.has_selection;

			dfi.has_selection=false;
			dfi.draw_fragment(IN_THREAD, ellipsis_fragment);
			dfi.has_selection=has_selection;

			// Need to restore this, for the next loop iteration.
			draw_bounds.position_at(original_position);
		}
	}

	// If characters are removed from an input field and it becomes
	// less tall, we need to clear the padding even if we were asked
	// not to.
	auto h=(*lock)->height();

	if (h < rendered_height)
	{
		clear_padding=true;
	}

	rendered_height=h;
	if (!clear_padding)
		return;

	// If there's undrawn area between the label and the bottom of the
	// display element, what we'll do is use draw_using_scratch_buffer()
	// to acquire the buffer. draw_using_scratch_buffer() clears it to
	// the element's background color. That's all we need to do, and
	// nothing more.

	while (y < coord_t::truncate(ending_y_position))
	{
		// If we know the height of the existing scratch buffer, take
		// advantage of it fully, to clear the remaining space.
		//
		// But make sure that it's at least 16 pixels.
		if (dfi.scratch_height < 16)
			dfi.scratch_height=16;

		dim_t remaining_height=dim_t::truncate(ending_y_position - y);

		if (remaining_height < dfi.scratch_height)
			dfi.scratch_height=remaining_height;

		element.draw_using_scratch_buffer
			(IN_THREAD,
			 [&]
			 (const picture &scratch_picture,
			  const pixmap &scratch_pixmap,
			  const gc &scratch_gc)
			 {
				 y=dim_t::truncate(y + dfi.scratch_height);
				 dfi.scratch_height=scratch_pixmap->get_height();
			 },
			 rectangle{draw_bounds.position.x,
					 coord_t::truncate(y +
							   draw_bounds
							   .position.y),
					 di.absolute_location.width,
					 dfi.scratch_height},
			 di, di,
			 clipped);
	}
}


std::tuple<pixmap, picture> richtextObj::create(ONLY IN_THREAD,
						const drawable &for_drawable)
{
	impl_t::lock lock{impl};

	dim_t width= dim_t::truncate((*lock)->width());
	dim_t height= dim_t::truncate((*lock)->height());

	auto pixmap=for_drawable->create_pixmap(width, height);
	auto p=pixmap->create_picture();

	dim_t largest_height=1;

	// The text must be non-empty. There must always be a fragment.

	richtextfragment frag=(*lock)->find_fragment_for_y_position(0);

	for (auto f=&*frag; f; f=f->next_fragment())
	{
		auto h=f->height();

		if (h > largest_height)
			largest_height=h;
	}

	auto scratch_pixmap=for_drawable->create_pixmap(width, largest_height);
	auto scratch_picture=scratch_pixmap->create_picture();

	auto transparent=rgb(0, 0, 0, 0);
	auto transparent_color=for_drawable->get_screen()->impl
		->create_solid_color_picture(transparent);

	coord_t y=0;
	for (auto f=&*frag; f; f=f->next_fragment())
	{
		auto h=f->height();

		scratch_picture->fill_rectangle({0, 0, width, largest_height},
						transparent);

		richtextfragmentObj::render_info
			render_info{ scratch_picture,
				     transparent_color, 0, 0,
				     0, 0,
				     width};
		render_info.ypos=coord_t::truncate(f->above_baseline);
		f->render(IN_THREAD, render_info);
		p->composite(scratch_picture,
			     0, 0,
			     {0, y, width, h});
		y=coord_t::truncate(y+h);
	}

	return {pixmap, p};
}

LIBCXXW_NAMESPACE_END
