/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/pixmap.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextmetalink.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextfragment_render.H"
#include "draw_info.H"
#include "element_draw.H"

LIBCXXW_NAMESPACE_START

richtext richtextBase::create(richtextstring &string,
			      halign alignment, dim_t initial_width)
{
	return ptrrefBase::objfactory<richtext>
		::create(ref<richtextObj::implObj>
			 ::create(string, alignment), initial_width);
}

richtextObj::richtextObj(const ref<implObj> &impl,
			 dim_t word_wrap_width)
	: impl(impl),
	  word_wrap_width_thread_only(word_wrap_width)
{
}

richtextObj::~richtextObj()=default;

// We must make sure that finish_initialization() gets invoked after the
// object gets constructed.

richtextObj::impl_t::lock::lock(IN_THREAD_ONLY,
				impl_t &me) : mpobj::lock(me)
{
	(**this)->finish_initialization(IN_THREAD);
}

void richtextObj::set(IN_THREAD_ONLY, richtextstring &string)
{
	impl_t::lock lock{IN_THREAD, impl};

	(*lock)->set(IN_THREAD, string);
}

std::pair<metrics::axis, metrics::axis> richtextObj::get_metrics(IN_THREAD_ONLY)
{
	impl_t::lock lock{IN_THREAD, impl};

	auto width= dim_t::truncate((*lock)->width());
	auto height= dim_t::truncate((*lock)->height());

	if (width >= dim_t::infinite())
		width=width-1;
	if (height >= dim_t::infinite())
		height=height-1;

	return {
		{width, width, width},
		{height, height, height}
	};
}

void richtextObj::theme_updated(IN_THREAD_ONLY)
{
	impl_t::lock lock{IN_THREAD, impl};

	(*lock)->theme_updated(IN_THREAD);
}

void richtextObj::do_draw(IN_THREAD_ONLY,
			  element_drawObj &element,
			  const draw_info &di,
			  bool force)
{
	impl_t::lock lock{IN_THREAD, impl};

	clip_region_set clipped{IN_THREAD, di};

	richtextfragmentObj *f=nullptr;

	if (!(*lock)->paragraphs.empty())
	{
		f= &*(*(*lock)->paragraphs.get_paragraph(0))->get_fragment(0);
	}

	// We'll capture the height of the scratch pixmap we're using
	// at the first opportunity.

	dim_t scratch_height=0;

	// It's unlikely, but possible that the container gave us
	// more height than we'll actually draw. Keep track of the ending
	// y coordinate that was rendered.
	coord_t y=0;

	// Now draw each fragment. Loop iteration advances y by each
	// fragment's height.

	for (; f; f=f->next_fragment())
	{
		// We rely on the fragments' y_position()s being accurate.

		dim_t y_position=f->y_position();

		if (y_position >= di.absolute_location.height)
			break;

		auto height=f->height();

		y=coord_t::truncate(y_position+height);

		if (!force)
		{
			if (!f->redraw_needed)
				continue;
		}

		element.draw_using_scratch_buffer
			(IN_THREAD,
			 [&]
			 (const picture &scratch_picture,
			  const pixmap &scratch_pixmap,
			  const gc &scratch_gc)
			 {
				 richtextfragmentObj::render_info
					 render_info{ scratch_picture,
						 di,
						 di,

						 // We don't use reverse_video,
						 // so just pass something here.

						 di.window_picture,
						 di.window_picture,

						 di.absolute_location.width,
						 0, // render_x_start
						 0, // vert_scroll
						 (*lock)->alignment,
						 };

				 f->redraw_needed=false;
				 f->render(IN_THREAD, render_info);

				 scratch_height=scratch_pixmap->get_height();

			 },
			 rectangle{0, coord_t::truncate(y_position),
					 di.absolute_location.width, height},
			 di, di,
			 clipped);
	}

	if (!force)
		return;

	// If there's undrawn area between the label and the bottom of the
	// display element, what we'll do is use draw_using_scratch_buffer()
	// to acquire the buffer. draw_using_scratch_buffer() clears it to
	// the element's background color. That's all we need to do, and
	// nothing more.

	while (dim_t::truncate(y) < di.absolute_location.height)
	{
		if (scratch_height < 16)
			scratch_height=16;

		dim_t remaining_height=
			di.absolute_location.height-dim_t::truncate(y);

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

ref<richtextObj::implObj> richtextObj::debug_get_impl(IN_THREAD_ONLY)
{
	impl_t::lock lock{IN_THREAD, impl};

	return *lock;
}

LIBCXXW_NAMESPACE_END
