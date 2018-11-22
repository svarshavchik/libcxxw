/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_layoutmanager_impl.H"
#include "peephole/peepholed_element.H"
#include "x/w/impl/metrics_horizvert.H"
#include "catch_exceptions.H"
#include "x/w/impl/container.H"
#include "generic_window_handler.H"
#include "gc.H"
#include "x/w/impl/background_color.H"
#include "x/w/scratch_buffer.H"
#include "screen.H"
#include "defaulttheme.H"
#include "run_as.H"
#include <x/property_value.H>
#include <x/visitor.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::peepholeObj::layoutmanager_implObj);

LIBCXXW_NAMESPACE_START

static property::value<bool>
disable_fast_scroll(LIBCXX_NAMESPACE_STR
		    "::w::disable_fast_scroll", false);

void peepholeObj::layoutmanager_implObj
::update_scrollbars(ONLY IN_THREAD,
		    const rectangle &element_pos,
		    const rectangle &current_position)
{
}

bool peepholeObj::layoutmanager_implObj
::process_button_event(ONLY IN_THREAD,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	return false;
}

peepholeObj::layoutmanager_implObj
::layoutmanager_implObj(const container_impl &container_impl,
			peephole_style style,
			const peepholed &element_in_peephole)
	: superclass_t(container_impl),
	style(style),
	element_in_peephole(element_in_peephole)
{
}

peepholeObj::layoutmanager_implObj::~layoutmanager_implObj()=default;

void peepholeObj::layoutmanager_implObj::child_metrics_updated(ONLY IN_THREAD)
{
	update_our_metrics(IN_THREAD);
	superclass_t::child_metrics_updated(IN_THREAD);
}

void peepholeObj::layoutmanager_implObj
::do_for_each_child(ONLY IN_THREAD,
		    const function<void (const element &e)> &callback)
{
	callback(element_in_peephole->get_peepholed_element());
}

size_t peepholeObj::layoutmanager_implObj::num_children(ONLY IN_THREAD)
{
	return 1;
}

layoutmanager peepholeObj::layoutmanager_implObj::create_public_object()
{
	return layoutmanager::create(layout_impl{this});
}

void peepholeObj::layoutmanager_implObj
::ensure_visibility(ONLY IN_THREAD,
		    elementObj::implObj &e,
		    const rectangle &rArg)
{
	auto hv=e.get_horizvert(IN_THREAD);

	rectangle r=rArg;

	// Sanity check the requested visibility against the element's metrics,
	// not its current size.
	//
	// It's possible that appending text into a peephole editor element
	// results in the editor element request visibility for the cursor
	// position immediately after announcing its larger metrics but
	// before it gets resized.

	if (r.x < 0)
	{
		coord_t last_x=coord_t::truncate(r.x+r.width);

		if (last_x < 0)
			return;

		r.x=0;
		r.width=dim_t::truncate(last_x);
	}

	if (r.y < 0)
	{
		coord_t last_y=coord_t::truncate(r.y+r.height);

		if (last_y < 0)
			return;

		r.y=0;
		r.height=dim_t::truncate(last_y);
	}

	auto max_width=hv->horiz.minimum();
	auto max_height=hv->horiz.minimum();

	if (dim_t::truncate(r.x) >= max_width ||
	    dim_t::truncate(r.y) >= max_height)
		return;

	max_width=max_width-dim_t::truncate(r.x);
	max_height=max_height-dim_t::truncate(r.y);

	if (max_width < r.width)
		r.width=max_width;
	if (max_height < r.height)
		r.height=max_height;

	recalculate_with_requested_visibility(IN_THREAD, &r);
}

// Called from recalculate to make sure that the proposed element_pos
// will have (offset_x, offset_y) coordinate visible, given the peephole's
// own current_position.

static void adjust_for_visibility(rectangle &element_pos,
				  const rectangle &current_position,
				  coord_squared_t x,
				  coord_squared_t y)
{
	auto targeted_x=element_pos.x + coord_t::truncate(x);
	auto targeted_y=element_pos.y + coord_t::truncate(y);

	// If the computed coordinates is beyond the given
	// margin, shift element_pos accordingly.

	if (targeted_x > coord_t::truncate(current_position.width))
	{
		element_pos.x=coord_squared_t::truncate(current_position.width)
			-coord_t::truncate(x);
	}
	else if (targeted_x < 0)
	{
		element_pos.x=coord_t::truncate(-x);
	}

	if (targeted_y > coord_t::truncate(current_position.height))
	{
		element_pos.y=coord_squared_t::truncate(current_position.height)
			-coord_t::truncate(y);
	}
	else if (targeted_y < 0)
	{
		element_pos.y=coord_t::truncate(-y);
	}
}

void peepholeObj::layoutmanager_implObj::recalculate(ONLY IN_THREAD)
{
	recalculate_with_requested_visibility(IN_THREAD, nullptr);
}

// Used when peephole_scroll::centered
//
// The element in the peephole requested visible for pos through pos+size,
// and the peephole's size is peephole_size.
//
// Just before we call adjust_for_visibility_dim, we do this for
// (requested_visibility.x, requested_visibility.width, element_pos.width,
//  current_position.width)
// and
// (requested_visibility.y, requested_visibility.height, element_pos.height,
//  current_position.height).
//
// So what we do is simply adjust the requested visibility to have the real
// visibility centered inside it, and make the requested visibility equal to
// current_position's size, so adjusted_visibility ends up scrolling the
// peephole to this precise position.

static void center_visibility_at(coord_t &requested_pos,
				 dim_t &requested_size,
				 dim_t element_size,
				 dim_t peephole_size)
{
	if (requested_size >= peephole_size)
		return; // We can't fit the whole thing, anyway.

	if (peephole_size >= element_size)
		return; // Edge case, no work here.

	// Half the "extra space", and subtract it from requested_pos, but
	// stop at zero.

	auto shift_by=(peephole_size - requested_size)/2;

	if (requested_pos < coord_t::truncate(shift_by))
		requested_pos=0;
	else
		requested_pos=coord_t::truncate(requested_pos-shift_by);

	requested_size=peephole_size;

	// We know that peephole_size < element_size, see above.

	if (requested_pos > coord_t::truncate(element_size-peephole_size))
		requested_pos=coord_t::truncate(element_size-peephole_size);
}

void peepholeObj::layoutmanager_implObj::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);

	// Now we can initialize our element.

	auto element_in_peephole_impl=
		element_in_peephole->get_peepholed_element()->impl;

	element_in_peephole_impl->initialize_if_needed(IN_THREAD);

	recompute_vertical_metrics(IN_THREAD,
				   element_in_peephole_impl->get_screen()
				   ->impl->current_theme.get());

	update_our_metrics(IN_THREAD);

	needs_recalculation(IN_THREAD);
}

void peepholeObj::layoutmanager_implObj
::theme_updated(ONLY IN_THREAD,
		const defaulttheme &theme)
{
	superclass_t::theme_updated(IN_THREAD, theme);
	recompute_vertical_metrics(IN_THREAD, theme);
}

void peepholeObj::layoutmanager_implObj
::recompute_vertical_metrics(ONLY IN_THREAD,
			     const defaulttheme &theme)
{
	std::visit(visitor
		 {
		  [&,this](const dim_axis_arg &arg)
		  {
			  vertical_metrics(IN_THREAD)=
				  arg.compute(theme,
					      themedimaxis::height);
		  },
		  [](const auto &)
		  {
		  }
		 },
		   style.height_algorithm);
}

void peepholeObj::layoutmanager_implObj::update_our_metrics(ONLY IN_THREAD)
{
	// Copy the peepholed element's metrics to ours, if auto width/height.

	if (style.width_algorithm==peephole_algorithm::automatic &&
	    std::holds_alternative<peephole_algorithm>(style.height_algorithm)&&
	    std::get<peephole_algorithm>(style.height_algorithm)==
	    peephole_algorithm::automatic)
		return;

	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	// This is its advertised metrics
	auto element_horizvert=
		peephole_element_impl->get_horizvert(IN_THREAD);

	auto my_horizvert=get_element_impl().get_horizvert(IN_THREAD);

	auto horiz=my_horizvert->horiz;
	auto vert=my_horizvert->vert;

	if (style.width_algorithm==peephole_algorithm::stretch_peephole)
		horiz=element_horizvert->horiz;

	std::visit(visitor
		   {
			   [&](peephole_algorithm a)
			   {
				   if (a==peephole_algorithm::stretch_peephole)
					   vert=element_horizvert->vert;
			   },
			   [&, this](const dim_axis_arg &)
			   {
				   vert=vertical_metrics(IN_THREAD);
			   },
			   [&](const std::tuple<size_t, size_t> &s)
			   {
				   const auto &[min, max]=s;

				   auto rows=element_in_peephole->
					   peepholed_rows(IN_THREAD);

				   if (rows < min)
					   rows=min;

				   if (rows > max)
					   rows=max;

				   dim_t height=dim_t::truncate
					   (element_in_peephole->
					    vertical_increment(IN_THREAD)
					    * rows);

				   if (height == dim_t::infinite())
					   --height;
				   vert={height, height, height};
			   }
		   }, style.height_algorithm);

	my_horizvert->set_element_metrics(IN_THREAD, horiz, vert);
}

void peepholeObj::layoutmanager_implObj
::recalculate_with_requested_visibility(ONLY IN_THREAD, rectangle *adjust_for)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto &current_position=get_element_impl()
		.data(IN_THREAD).current_position;

	// This is the peepholed element's current position
	auto &element_current_position=
		peephole_element_impl->data(IN_THREAD).current_position;

	// This is its advertised metrics
	auto element_horizvert=
		peephole_element_impl->get_horizvert(IN_THREAD);

	// Compute the peepholed element's new position. Start with its
	// current x/y coordinates, and tentatively size it at its maximum
	// requested size.
	rectangle element_pos{element_current_position.x,
			element_current_position.y,
			element_horizvert->horiz.maximum(),
			element_horizvert->vert.maximum()};

	// If the maximum requested size exceeds the peephole's size,
	// truncate it down (this will also chop off the infinite() requested
	// size.

	if (element_pos.width > current_position.width)
		element_pos.width=current_position.width;
	if (element_pos.height > current_position.height)
		element_pos.height=current_position.height;

	// But don't go below the element's minimum size. This is what the
	// peephole is, after all.
	if (element_pos.width < element_horizvert->horiz.minimum())
		element_pos.width=element_horizvert->horiz.minimum();
	if (element_pos.height < element_horizvert->vert.minimum())
		element_pos.height=element_horizvert->vert.minimum();

	LOG_DEBUG("Peephole " << this
		  << ": current position is " << current_position
		  << std::endl
		  << "   element position is " << element_pos);

	// The "minimum" scroll position is technically accurate, but
	// misleading. The element in the peephole gets scrolled into view
	// by setting its starting coordinates to negative values (thus
	// bringing the bottom/right-most portions of the element into the
	// peepholed view). As such, min_scroll-x is actually the farthest
	// the element can be scrolled, at which point its bottom-right
	// corner is going to be aligned with the peephole's bottom-right
	// corner.
	//
	// But that's getting a bit ahead of the game. Firstly, unless the
	// element's size exceeds the peephole's size, it's a moot point, and
	// the "minimum" scroll position is 0 -- it won't scroll, it fits
	// entirely into the peephole.

	coord_t min_scroll_x=0;
	coord_t min_scroll_y=0;

	if (element_pos.width > current_position.width)
	{
		min_scroll_x=-coord_t::truncate(element_pos.width-
						current_position.width);
	}

	if (element_pos.height > current_position.height)
	{
		min_scroll_y=-coord_t::truncate(element_pos.height-
						current_position.height);
	}

	if (style.width_algorithm==peephole_algorithm::stretch_peephole)
		min_scroll_x=0;

	if (std::holds_alternative<peephole_algorithm>(style.height_algorithm)
	    && std::get<peephole_algorithm>(style.height_algorithm) ==
	    peephole_algorithm::stretch_peephole)
		min_scroll_y=0;

	LOG_DEBUG("Minimum X position is " << min_scroll_x);
	LOG_DEBUG("Minimum Y position is " << min_scroll_y);

	// Call for adjust_visibility. Notably we scroll bottom/right
	// requested_visibility corner into view first. This makes sure that
	// the bottom/right is scrolled into view first, then top/left.

	if (adjust_for)
	{
		LOG_DEBUG("Element's requested visibility: " << *adjust_for);

		if (style.scroll == peephole_scroll::centered)
		{
			center_visibility_at(adjust_for->x, adjust_for->width,
					     element_pos.width,
					     current_position.width);
			center_visibility_at(adjust_for->y, adjust_for->height,
					     element_pos.height,
					     current_position.height);
		}

		adjust_for_visibility(element_pos, current_position,
				      adjust_for->x+
				      adjust_for->width,
				      adjust_for->y+
				      adjust_for->height);

		adjust_for_visibility(element_pos, current_position,
				      adjust_for->x+dim_t{0},
				      adjust_for->y+
				      adjust_for->height);

		adjust_for_visibility(element_pos, current_position,
				      adjust_for->x+
				      adjust_for->width,
				      adjust_for->y+dim_t{0});

		adjust_for_visibility(element_pos, current_position,
				      adjust_for->x+dim_t{0},
				      adjust_for->y+dim_t{0});
	}

	LOG_DEBUG("Adjusted element position to: " << element_pos);

	// Final set of sanity checks.

	if (element_pos.x > 0)
		element_pos.x=0;

	if (element_pos.y > 0)
		element_pos.y=0;

	if (element_pos.x < min_scroll_x)
		element_pos.x=min_scroll_x;

	if (element_pos.y < min_scroll_y)
		element_pos.y=min_scroll_y;

	auto aligned_position=metrics::align(current_position.width,
					     current_position.height,
					     element_pos.width,
					     element_pos.height,
					     style.horizontal_alignment,
					     style.vertical_alignment);

	// We pay attention to what align() tells us only if the
	// peepholed element is smaller. Otherwise, we control the position.

	if (element_pos.width < current_position.width)
	{
		element_pos.x=aligned_position.x;
		element_pos.width=aligned_position.width;
	}

	if (element_pos.height < current_position.height)
	{
		element_pos.y=aligned_position.y;
		element_pos.height=aligned_position.height;
	}

	LOG_DEBUG("Positioning element to: " << element_pos);

	// If our size hasn't been set yet, don't both to position the
	// peepholed element.
	if (current_position.width > 0 &&
	    current_position.height > 0 &&
	    !attempt_scroll_to(IN_THREAD, element_pos))
		peephole_element_impl->update_current_position(IN_THREAD,
							       element_pos);

	// update_scrollbars expects to see only NEGATIVE x and y positions,
	// which is how we normally scroll. But if the element is smaller than
	// the peephole, then align() will result in positive x and y
	// coordinates, here.
	//
	// The alignment is our own doing, so hide it from update_scrollbars()

	if (element_pos.x > 0)
		element_pos.x=0;

	if (element_pos.y > 0)
		element_pos.y=0;
	update_scrollbars(IN_THREAD, element_pos, current_position);
}

bool peepholeObj::layoutmanager_implObj
::attempt_scroll_to(ONLY IN_THREAD, const rectangle &r)
{
	if (disable_fast_scroll.get())
		return false;

	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto &element_current_position=
		peephole_element_impl->data(IN_THREAD).current_position;

	// This must be a scroll if:

	// 0) We are visible, and our background is scrollable

	auto &e=get_element_impl();

	if (!e.data(IN_THREAD).logical_inherited_visibility)
		return false;

	if (!e.current_background_color(IN_THREAD)->is_scrollable_background())
		return false;

	// 1) The element's size doesn't change.
	if (element_current_position.width != r.width ||
	    element_current_position.height != r.height)
		return false;

	// 2) The element's position changes

	if (element_current_position.x == r.x &&
	    element_current_position.y == r.y)
		return false;

	auto &di=e.get_draw_info(IN_THREAD);

	// 3) The peephole is unobstructed.
	//
	// Look at the element_viewport. Unobstructed means exactly one
	// rectangle in there.
	if (di.element_viewport.size() != 1)
		return false;

	auto &viewport_rectangle=*di.element_viewport.begin();

	auto scrolled_rectangle=viewport_rectangle;

	// 4) The scrolling distance fits within the viewport.

	dim_t shift_left=0, shift_right=0, shift_up=0, shift_down=0;

	if (element_current_position.x > r.x)
	{
		shift_left=dim_t::truncate(element_current_position.x-r.x);

		if (shift_left >= viewport_rectangle.width)
			return false;

		scrolled_rectangle.x=coord_t::truncate(scrolled_rectangle.x-
						       shift_left);
	}
	else
	{
		shift_right=dim_t::truncate(r.x-element_current_position.x);

		if (shift_right >= viewport_rectangle.width)
			return false;

		scrolled_rectangle.x=coord_t::truncate(scrolled_rectangle.x+
						       shift_right);
	}

	if (element_current_position.y > r.y)
	{
		shift_up=dim_t::truncate(element_current_position.y-r.y);

		if (shift_up >= viewport_rectangle.height)
			return false;

		scrolled_rectangle.y=coord_t::truncate(scrolled_rectangle.y-
						       shift_up);
	}
	else
	{
		shift_down=dim_t::truncate(r.y-element_current_position.y);

		if (shift_down >= viewport_rectangle.height)
			return false;

		scrolled_rectangle.y=coord_t::truncate(scrolled_rectangle.y+
						       shift_down);
	}

	// So, viewport_rectangle is our current rectangle. And
	// scrolled_rectangle is what would its position be, if it was scrolled
	// like we want to scroll the peepholed element.

	// The intersection between the two of them would be the portion of the
	// peephole viewport that gets scrolled AFTER it is scrolled.
#if 0
	std::cout << "CURRENT POSITION: " << element_current_position
		  << std::endl;
	std::cout << "NEW POSITION: " << r << std::endl;

	std::cout << "SCROLL VIEWPORT: " << viewport_rectangle << std::endl;
	std::cout << "    (" << e.objname() << ")" << std::endl;
	std::cout << "TO: " << scrolled_rectangle << std::endl;
#endif
	auto after_scroll_rectangle_set=intersect(di.element_viewport,
						  scrolled_rectangle);

	// Should always be a single rectangle.

	if (after_scroll_rectangle_set.size() != 1)
		return false;

	auto scrolled_to=*after_scroll_rectangle_set.begin();

	// Now, reverse engineer where this gets scrolled from.

	auto scrolled_from=scrolled_to;

	scrolled_from.x =
		coord_t::truncate(scrolled_from.x-shift_right+shift_left);
	scrolled_from.y =
		coord_t::truncate(scrolled_from.y-shift_down+shift_up);

	// scrolled_to is the shifted position of the scroll area.
	//
	// The scroll effectively paints it. Subtract it from the entire
	// viewport to determine what should be manually redrawn, after the
	// scroll, because it was vacated by the scrolled content.

	auto redrawn=subtract(di.element_viewport, {scrolled_to});
#if 0
	std::cout << "MOVE: " << element_current_position
		  << " -> " << r
		  << std::endl;
	std::cout << "SCROLL: " << scrolled_from << " -> " << scrolled_to
		  << std::endl;
	std::cout << "REDRAW " << e.objname() << ":" << std::endl;
	for (const auto &r:redrawn)
		std::cout << " " << r << std::endl;
#endif
	// Effect the scroll using xcb_copy-area

	peephole_element_impl->element_scratch_buffer->get
		(0, 0,
		 [&, this]
		 (const picture &,
		  const pixmap &,
		  const gc &gc)
		 {
			 auto wid=peephole_element_impl->get_window_handler()
				 .id();

			 xcb_copy_area(IN_THREAD->info->conn, wid, wid,
				       gc->impl->gc_id(),
				       coord_t::truncate(scrolled_from.x),
				       coord_t::truncate(scrolled_from.y),
				       coord_t::truncate(scrolled_to.x),
				       coord_t::truncate(scrolled_to.y),
				       coord_t::truncate(scrolled_from.width),
				       coord_t::truncate(scrolled_from.height));
		 });

	// Notify the element via scroll_by_parent_container before
	// invoking exposure_event_recursive() in order to redraw everything
	// inside it.

	peephole_element_impl->scroll_by_parent_container(IN_THREAD, r.x, r.y);
	if (!redrawn.empty())
		e.exposure_event_recursively_top_down(IN_THREAD, redrawn);

	return true;
}

void peepholeObj::layoutmanager_implObj
::update_horizontal_scroll(ONLY IN_THREAD, dim_t offset)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t x=coord_t::truncate(offset);

	cur_pos.x= -x;

	if (!attempt_scroll_to(IN_THREAD, cur_pos))
		peephole_element_impl->update_current_position(IN_THREAD,
							       cur_pos);
}

void peepholeObj::layoutmanager_implObj
::update_vertical_scroll(ONLY IN_THREAD, dim_t offset)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t y=coord_t::truncate(offset);

	cur_pos.y= -y;
	if (!attempt_scroll_to(IN_THREAD, cur_pos))
		peephole_element_impl->update_current_position(IN_THREAD,
							       cur_pos);
}

void peepholeObj::layoutmanager_implObj
::process_updated_position(ONLY IN_THREAD,
			   const rectangle &position)
{
	recalculate(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
