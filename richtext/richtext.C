/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/pixmap.H"
#include "x/w/drawable.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextmetalink.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextfragment_render.H"
#include "richtext/richtextiterator.H"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtext_draw_info.H"
#include "draw_info.H"
#include "element_draw.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

richtext richtextBase::create(const richtextstring &string,
			      halign alignment, dim_t initial_width)
{
	return ptrrefBase::objfactory<richtext>
		::create(ref<richtext_implObj>
			 ::create(string, alignment), initial_width);
}

richtextObj::richtextObj(const ref<richtext_implObj> &impl,
			 dim_t word_wrap_width)
	: impl(impl),
	  word_wrap_width_thread_only(word_wrap_width)
{
}

richtextObj::~richtextObj()=default;

// We must make sure that finish_initialization() gets invoked after the
// object gets constructed.

richtextObj::impl_t::lock::lock(IN_THREAD_ONLY, impl_t &me)
	: internal_richtext_impl_t::lock(me)
{
	(**this)->finish_initialization(IN_THREAD);
}

void richtextObj::set(IN_THREAD_ONLY, const richtextstring &string)
{
	impl_t::lock lock{IN_THREAD, impl};

	(*lock)->set(IN_THREAD, string);
}

bool richtextObj::rewrap(IN_THREAD_ONLY,
			 dim_t width)
{
	impl_t::lock lock{IN_THREAD, impl};

	word_wrap_width(IN_THREAD)=width;

	return width > 0 ? (*lock)->rewrap(IN_THREAD, width)
		: (*lock)->unwrap(IN_THREAD);
}

dim_t richtextObj::get_width(IN_THREAD_ONLY)
{
	impl_t::lock lock{IN_THREAD, impl};

	return dim_t::truncate((*lock)->width());
}

std::pair<metrics::axis, metrics::axis>
richtextObj::get_metrics(IN_THREAD_ONLY, dim_t preferred_width, bool visible)
{
	impl_t::lock lock{IN_THREAD, impl};

	dim_t width= dim_t::truncate((*lock)->width());
	dim_t height= dim_t::truncate((*lock)->height());

	if (width >= dim_t::infinite())
		width=width-1;
	if (height >= dim_t::infinite())
		height=height-1;

	auto min_width=width;
	auto max_width=width;

	if (word_wrap_width(IN_THREAD) > 0)
	{
		// This label is word-wrapped, and it is visible.
		// We compute the metrics like this. Here's our minimum
		// and maximum widths:
		max_width=dim_t::truncate((*lock)->real_maximum_width);

		if (max_width == dim_t::infinite()) // Let's not go there.
			max_width=max_width-1;

		min_width=(*lock)->minimum_width;

		// And let's try to be sane.

		if (min_width > max_width)
			min_width=max_width;

		width=preferred_width;

		if (width < min_width)
			width=min_width;

		if (width > max_width)
			width=max_width;

		// Even if the text is wrappable, if it's not visible we don't
		// do any of the above, so the resulting metrics, returned
		// below, specify a fixed width.
		if (!visible)
			min_width=width=max_width=preferred_width;
	}

	return {
		{min_width, width, max_width},
		{height, height, height}
	};
}

void richtextObj::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	impl_t::lock lock{IN_THREAD, impl};

	(*lock)->theme_updated(IN_THREAD, new_theme);
}

void richtextObj::full_redraw(IN_THREAD_ONLY,
			      element_drawObj &element,
			      const richtext_draw_info &rdi,
			      const draw_info &di,
			      const rectangle_set &areas)
{
	draw(IN_THREAD, element, rdi, di,
	     make_function<bool (richtextfragmentObj *)>
	     ([]
	      (richtextfragmentObj *ignore)
	      {
		      return true;
	      }),
	     true, areas);
}

void richtextObj::redraw_whatsneeded(IN_THREAD_ONLY,
				     element_drawObj &element,
				     const richtext_draw_info &rdi,
				     const draw_info &di)
{
	redraw_whatsneeded(IN_THREAD, element, rdi, di, di.entire_area());
}

void richtextObj::redraw_whatsneeded(IN_THREAD_ONLY,
				     element_drawObj &element,
				     const richtext_draw_info &rdi,
				     const draw_info &di,
				     const rectangle_set &areas)
{
	draw(IN_THREAD, element, rdi, di,
	     make_function<bool (richtextfragmentObj *)>
	     ([]
	      (richtextfragmentObj *f)
	      {
		      bool flag=f->redraw_needed;
		      f->redraw_needed=false;
		      return flag;
	      }),
	     false, areas);
}

void richtextObj::redraw_between(IN_THREAD_ONLY,
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

	draw(IN_THREAD, element, rdi, di,
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
	     di.entire_area());
}

void richtextObj::draw(IN_THREAD_ONLY,
		       element_drawObj &element,
		       const richtext_draw_info &rdi,
		       const draw_info &di,
		       const function<bool (richtextfragmentObj *)> &redraw_fragment,
		       bool clear_padding,
		       const rectangle_set &areas)
{
	// Do only the bare minimum of work. We are told to draw only the
	// given areas.

	// First, translate element_view from absolute window coordinates
	// to relative element coordinates. Compute the intersection with
	// the given areas. If the result is empty, draw nothing.
	//
	// Otherwise we compute the bounding rectangle and draw only the
	// fragments that fall within the boundaries. of the bounding
	// rectangle.
	auto draw_bounds=bounds
		(({
				std::vector<rectangle>
					rects{di.element_viewport.begin(),
						di.element_viewport.end()
						};

				for (auto &r:rects)
				{
					r.x=coord_t::truncate
						(r.x-di.absolute_location.x);
					r.y=coord_t::truncate
						(r.y-di.absolute_location.y);
				}

				auto what_to_draw=
					intersect(rectangle_set{rects.begin(),
								rects.end()},
						areas);

				if (what_to_draw.empty())
					return;

				what_to_draw;
			}));

	assert_or_throw(draw_bounds.x >= 0 && draw_bounds.y >= 0,
			"Bounding rectangle cannot start on a negative coordinate");
	impl_t::lock lock{IN_THREAD, impl};

	clip_region_set clipped{IN_THREAD, di};

	richtextfragmentObj *f=nullptr;

	if (!(*lock)->paragraphs.empty())
	{
		size_t y_pos=draw_bounds.y < 0 ? 0:
			(coord_t::value_type)(draw_bounds.y);

		auto frag=(*lock)->find_fragment_for_y_position(y_pos);

		if (frag)
			f=&*frag;
	}

	// We'll capture the height of the scratch pixmap we're using
	// at the first opportunity.

	dim_t scratch_height=0;

	// It's unlikely, but possible that the container gave us
	// more height than we'll actually draw. Keep track of the ending
	// y coordinate that was rendered.
	coord_t y=draw_bounds.y;

	auto ending_y_position=draw_bounds.y + draw_bounds.height;

	// Determine if there's a current selection.

	size_t selection_start_fragment_index=0;
	size_t selection_start_offset=0;
	size_t selection_end_fragment_index=0;
	size_t selection_end_offset=0;
	bool has_selection=false;

	if (rdi.selection_start && rdi.selection_end)
	{
		auto a=rdi.selection_start->my_location;
		auto b=rdi.selection_end->my_location;

		auto cmp=a->compare(*b);

		if (cmp)
		{
			if (cmp > 0)
				std::swap(a, b);

			selection_start_fragment_index=a->my_fragment->index();
			selection_start_offset=a->get_offset();

			selection_end_fragment_index=b->my_fragment->index();
			selection_end_offset=b->get_offset();
			has_selection=true;
		}
	}

	// Now draw each fragment. Loop iteration advances y by each
	// fragment's height.

	auto fragment_index=f ? f->index():0;

	for (; f; (f=f->next_fragment()), ++fragment_index)
	{
		// We rely on the fragments' y_position()s being accurate.

		auto y_position=coord_squared_t{f->y_position()};

		if (ending_y_position <= y_position)
			break;

		auto height=f->height();

		y=coord_t::truncate(y_position+height);

		if (!redraw_fragment(f))
			continue;

		element.draw_using_scratch_buffer
			(IN_THREAD,
			 [&]
			 (const picture &scratch_picture,
			  const pixmap &scratch_pixmap,
			  const gc &scratch_gc)
			 {
				 richtextfragmentObj::render_info
					 render_info{ scratch_picture,
						 di.window_background,
						 di.background_x,
						 di.background_y,
						 di.absolute_location.x,
						 di.absolute_location.y,

						 draw_bounds.width,
						 dim_t::truncate(draw_bounds.x),
						 };

				 // If we're drawing a selection, figure out
				 // which part of it is on this line.

				 if (has_selection &&
				     fragment_index >=
				     selection_start_fragment_index &&
				     fragment_index <=
				     selection_end_fragment_index)
				 {
					 size_t from=0;
					 size_t to=f->string.size();

					 if (fragment_index ==
					     selection_start_fragment_index)
						 from=selection_start_offset;

					 if (fragment_index ==
					     selection_end_fragment_index)
						 to=selection_end_offset;

					 render_info.selection_start=from;
					 render_info.selection_end=to;
				 }

				 f->render(IN_THREAD, render_info);

				 scratch_height=scratch_pixmap->get_height();

			 },
			 rectangle{draw_bounds.x, coord_t::truncate(y_position),
					 draw_bounds.width, height},
			 di, di,
			 clipped);
	}

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
		if (scratch_height < 16)
			scratch_height=16;

		dim_t remaining_height=dim_t::truncate(ending_y_position - y);

		if (remaining_height < scratch_height)
			scratch_height=remaining_height;

		element.draw_using_scratch_buffer
			(IN_THREAD,
			 [&]
			 (const picture &scratch_picture,
			  const pixmap &scratch_pixmap,
			  const gc &scratch_gc)
			 {
				 y=dim_t::truncate(y + scratch_height);
				 scratch_height=scratch_pixmap->get_height();
			 },
			 rectangle{0, y, di.absolute_location.width,
					 scratch_height},
			 di, di,
			 clipped);
	}

}

std::tuple<pixmap, picture> richtextObj::create(IN_THREAD_ONLY,
						const drawable &for_drawable)
{
	auto metrics=get_metrics(IN_THREAD, 0, false);

	auto width=metrics.first.preferred();
	auto height=metrics.second.preferred();

	auto pixmap=for_drawable->create_pixmap(width, height);
	auto p=pixmap->create_picture();

	impl_t::lock lock{IN_THREAD, impl};

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
				transparent_color->impl,
				0, 0,
				0, 0,
				width};
		f->render(IN_THREAD, render_info);
		p->composite(scratch_picture,
			     0, 0,
			     {0, y, width, h});
		y=coord_t::truncate(y+h);
	}

	return {pixmap, p};
}

richtextiterator richtextObj::begin()
{
	return at(0);
}

richtextiterator richtextObj::end()
{
	return at((size_t)-1);
}

richtextiterator richtextObj::at(size_t npos)
{
	internal_richtext_impl_t::lock lock{impl};

	return at(lock, npos);
}

richtextiterator richtextObj::at(internal_richtext_impl_t::lock &lock, size_t npos)
{
	size_t n_paragraph=(*lock)->find_paragraph_for_pos(npos);

	auto paragraph_iter=(*lock)->paragraphs.get_paragraph(n_paragraph);

	auto fragment=(*paragraph_iter)->find_fragment_for_pos(npos);

	auto s=fragment->string.size();

	if (s == 0)
		throw EXCEPTION("Internal error: empty rich text fragment in at().");

	if (npos >= s)
		npos=s-1;

	auto location=richtextcursorlocation::create();

	return richtextiterator::create(richtext(this),
					location,
					&*fragment,
					npos);
}

size_t richtextObj::insert_at_location(IN_THREAD_ONLY,
				       impl_t::lock &lock,
				       const richtext_insert_base &new_text)
{
	return (*lock)->insert_at_location(IN_THREAD,
					   word_wrap_width(IN_THREAD),
					   new_text);
}

void richtextObj::remove_at_location(IN_THREAD_ONLY,
				     impl_t::lock &lock,
				     const richtextcursorlocation &a,
				     const richtextcursorlocation &b)
{
	return (*lock)->remove_at_location(IN_THREAD,
					   word_wrap_width(IN_THREAD),
					   a, b);
}

size_t richtextObj::pos(const internal_richtext_impl_t::lock &lock,
			const richtextcursorlocation &l)
{
	assert_or_throw
		(l->my_fragment &&
		 l->my_fragment->string.size() > l->get_offset() &&
		 l->my_fragment->my_paragraph,
		 "Internal error in pos(): invalid cursor location");

	return l->get_offset() +l->my_fragment->first_char_n +
		l->my_fragment->my_paragraph->first_char_n;
}

void richtextObj::get(const internal_richtext_impl_t::lock &lock,
		      richtextstring &str,
		      const richtextcursorlocation &a,
		      const richtextcursorlocation &b)
{
	const richtextcursorlocationObj *location_a=&*a;
	const richtextcursorlocationObj *location_b=&*b;

	auto diff=location_a->compare(*location_b);

	if (diff == 0)
		return; // Too easy

	// Make sure we go from a to b.

	if (diff > 0)
	{
		location_b=&*a;
		location_a=&*b;
	}
	else
	{
		diff= -diff;
	}

	assert_or_throw(location_a->my_fragment &&
			location_b->my_fragment,
			"Internal error: uninitialized fragments in get()");

	if (diff == 1)
	{
		str.insert(0, {location_a->my_fragment->string,
					location_a->get_offset(),
					location_b->get_offset()-
					location_a->get_offset()});
		return;
	}

	str.insert(0, {location_a->my_fragment->string,
				location_a->get_offset(),
				location_a->my_fragment
				->string.size()
				-location_a->get_offset()});
	--diff;
	auto f=location_a->my_fragment->next_fragment();

	while (--diff)
	{
		assert_or_throw(f, "Internal error: NULL fragment");
		str.insert(str.size(), f->string);

		f=f->next_fragment();
	}
	assert_or_throw(f, "Internal error: NULL fragment");

	str.insert(str.size(),
		   {f->string, 0, location_b->get_offset()});
}

ref<richtext_implObj> richtextObj::debug_get_impl(IN_THREAD_ONLY)
{
	impl_t::lock lock{IN_THREAD, impl};

	return *lock;
}

LIBCXXW_NAMESPACE_END
