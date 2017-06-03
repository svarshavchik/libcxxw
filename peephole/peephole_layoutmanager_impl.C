/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/canvas.H"
#include "x/w/gridfactory.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "peephole/peepholed_element.H"
#include "scrollbar/scrollbar_impl.H"
#include "catch_exceptions.H"
#include "container.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::peepholeObj::layoutmanager_implObj)
LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////

class peepholeObj::layoutmanager_implObj::scrollbar_implObj
	: public scrollbarObj::implObj {

public:
	typedef void (layoutmanager_implObj::*update_func_t)(IN_THREAD_ONLY,
							     dim_t);

	// update_horizontal_scroll() or update_vertical_scroll()
	const update_func_t update_func;

	// Need to find my layout manager.
	weakptr<ptr<layoutmanager_implObj>> my_layoutmanager;

	// Constructor
	scrollbar_implObj(const auto &init_params,
			  update_func_t update_func)
		: scrollbarObj::implObj(init_params),
		update_func(update_func)
	{
	}

	// Scrollbar has been dragged.
	void updated_value(IN_THREAD_ONLY,
			   scroll_v_t value,
			   scroll_v_t dragged_value) override
	{
		auto p=my_layoutmanager.getptr();

		if (p)
			((*p).*update_func)(IN_THREAD,
					    dim_t::truncate(dragged_value));
	}
};

peephole_scrollbars
create_peephole_scrollbars(const ref<containerObj::implObj> &container)
{
	// The scrollbar implementation object is scrollbar_implObj.
	// We'll capture both the implementation object, and the display
	// element, which will be the one that's shown or hidden.

	ptr<peepholeObj::layoutmanager_implObj::scrollbar_implObj>
		horizontal_impl, vertical_impl;

	auto horizontal=create_horizontal_scrollbar
		(container, scrollbar_config(),
		 [&]
		 (const auto &init_params)
		 {
			 auto impl=ref<peepholeObj::layoutmanager_implObj
			 ::scrollbar_implObj>::create(init_params,
						      &peepholeObj
						      ::layoutmanager_implObj
						      ::update_horizontal_scroll
						      );

			 horizontal_impl=impl;

			 return impl;
		 });

	auto vertical=create_vertical_scrollbar
		(container, scrollbar_config(),
		 [&]
		 (const auto &init_params)
		 {
			 auto impl=ref<peepholeObj::layoutmanager_implObj
			 ::scrollbar_implObj>::create(init_params,
						      &peepholeObj
						      ::layoutmanager_implObj
						      ::update_vertical_scroll
						      );

			 vertical_impl=impl;

			 return impl;
		 });

	return {horizontal, vertical, horizontal_impl, vertical_impl};
}

void install_peephole_scrollbars(const scrollbar &vertical_scrollbar,
				 scrollbar_visibility v_visibility,
				 const gridfactory &row0_factory,
				 const scrollbar &horizontal_scrollbar,
				 scrollbar_visibility h_visibility,
				 const gridfactory &row1_factory)
{
	row0_factory->remove_when_hidden(v_visibility !=
					 scrollbar_visibility
					 ::automatic_reserved)
		.padding(0).created_internally(vertical_scrollbar);

	row1_factory->remove_when_hidden(h_visibility !=
					 scrollbar_visibility
					 ::automatic_reserved)
		.padding(0).created_internally(horizontal_scrollbar);

	// Create a canvas to fill in the unused bottom-right corner.
	row1_factory->padding(0).create_canvas([]
					       (const auto &ignore)
					       {
					       },
					       {0, 0, 0},
					       {0, 0, 0});
}

void set_peephole_scrollbar_focus_order(const focusable &element,
					const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
{
	vertical_scrollbar->get_focus_after(element);

	set_peephole_scrollbar_focus_order(horizontal_scrollbar,
					   vertical_scrollbar);
}

void set_peephole_scrollbar_focus_order(const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
{
	horizontal_scrollbar->get_focus_after(vertical_scrollbar);
}

peephole_scrollbars
::peephole_scrollbars(const scrollbar &horizontal_scrollbar,
		      const scrollbar &vertical_scrollbar,
		      const ref<peepholeObj::layoutmanager_implObj
		      ::scrollbar_implObj> &horizontal_scrollbar_impl,
		      const ref<peepholeObj::layoutmanager_implObj
		      ::scrollbar_implObj> &vertical_scrollbar_impl)
	: horizontal_scrollbar(horizontal_scrollbar),
	  vertical_scrollbar(vertical_scrollbar),
	  horizontal_scrollbar_impl(horizontal_scrollbar_impl),
	  vertical_scrollbar_impl(vertical_scrollbar_impl)
{
}

peephole_scrollbars::peephole_scrollbars(const peephole_scrollbars &)=default;

peephole_scrollbars::peephole_scrollbars(peephole_scrollbars &&)=default;

peephole_scrollbars::~peephole_scrollbars()=default;

//////////////////////////////////////////////////////////////////////////////

peepholeObj::layoutmanager_implObj
::layoutmanager_implObj(const ref<containerObj::implObj> &container_impl,
			const peepholed &element_in_peephole,

			const peephole_scrollbars &scrollbars,
			const scrollbar_visibility horizontal_scrollbar_visibility,
			const scrollbar_visibility vertical_scrollbar_visibility)
	: layoutmanagerObj::implObj(container_impl),
	element_in_peephole(element_in_peephole),
	horizontal_scrollbar_visibility_thread_only(horizontal_scrollbar_visibility),
	vertical_scrollbar_visibility_thread_only(vertical_scrollbar_visibility),
	horizontal_scrollbar(scrollbars.horizontal_scrollbar_impl),
	vertical_scrollbar(scrollbars.vertical_scrollbar_impl),
	horizontal_scrollbar_element(scrollbars.horizontal_scrollbar->elementObj::impl),
	vertical_scrollbar_element(scrollbars.vertical_scrollbar->elementObj::impl)
{
}

void peepholeObj::layoutmanager_implObj::initialize_scrollbars()
{
	auto me=ref<layoutmanager_implObj>(this);

	horizontal_scrollbar->my_layoutmanager=me;
	vertical_scrollbar->my_layoutmanager=me;
}

peepholeObj::layoutmanager_implObj::~layoutmanager_implObj()=default;

void peepholeObj::layoutmanager_implObj
::child_metrics_updated(IN_THREAD_ONLY)
{
	recalculate(IN_THREAD);
}

void peepholeObj::layoutmanager_implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const element &e)> &callback)
{
	callback(element_in_peephole->get_element());
}

layoutmanager peepholeObj::layoutmanager_implObj::create_public_object()
{
	return layoutmanager::create(ref<layoutmanagerObj::implObj>(this));
}

void peepholeObj::layoutmanager_implObj
::ensure_visibility(IN_THREAD_ONLY,
		    elementObj::implObj &e,
		    const rectangle &r)
{
	requested_visibility=r;
	recalculate_with_requested_visibility(IN_THREAD, true);
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

void peepholeObj::layoutmanager_implObj::recalculate(IN_THREAD_ONLY)
{
	recalculate_with_requested_visibility(IN_THREAD, false);
}

void peepholeObj::layoutmanager_implObj
::recalculate_with_requested_visibility(IN_THREAD_ONLY, bool flag)
{
	auto peephole_element_impl=element_in_peephole->get_element()->impl;

	try {
		peephole_element_impl->initialize_if_needed(IN_THREAD);
	} CATCH_EXCEPTIONS;

	auto &current_position=container_impl->get_element_impl()
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

	LOG_DEBUG("Minimum X position is " << min_scroll_x);
	LOG_DEBUG("Minimum Y position is " << min_scroll_y);

	// Call for adjust_visibility. Notably we scroll bottom/right
	// requested_visibility corner into view first. This makes sure that
	// the bottom/right is scrolled into view first, then top/left.

	if (flag)
	{
		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+
				      requested_visibility.width,
				      requested_visibility.y+
				      requested_visibility.height
				      );

		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+dim_t{0},
				      requested_visibility.y+
				      requested_visibility.height
				      );

		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+
				      requested_visibility.width,
				      requested_visibility.y+dim_t{0});

		adjust_for_visibility(element_pos, current_position,
				      requested_visibility.x+dim_t{0},
				      requested_visibility.y+dim_t{0});
	}

	LOG_DEBUG("Element's requested visibility: " << requested_visibility
		  << std::endl
		  << "   adjusted element position to: "
		  << element_pos);

	// Final set of sanity checks.

	if (element_pos.x > 0)
		element_pos.x=0;

	if (element_pos.y > 0)
		element_pos.y=0;

	if (element_pos.x < min_scroll_x)
		element_pos.x=min_scroll_x;

	if (element_pos.y < min_scroll_y)
		element_pos.y=min_scroll_y;

	LOG_DEBUG("Positioning element to: " << element_pos);
	peephole_element_impl->update_current_position(IN_THREAD,
						       element_pos);

	update_scrollbar(IN_THREAD,
			 horizontal_scrollbar,
			 horizontal_scrollbar_element,
			 horizontal_scrollbar_visibility(IN_THREAD),
			 element_pos.x, element_pos.width,
			 current_position.width,
			 element_in_peephole->horizontal_increment(IN_THREAD));

	update_scrollbar(IN_THREAD,
			 vertical_scrollbar,
			 vertical_scrollbar_element,
			 vertical_scrollbar_visibility(IN_THREAD),
			 element_pos.y, element_pos.height,
			 current_position.height,
			 element_in_peephole->vertical_increment(IN_THREAD));
}

void peepholeObj::layoutmanager_implObj
::update_horizontal_scroll(IN_THREAD_ONLY, dim_t offset)
{
	auto peephole_element_impl=element_in_peephole->get_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t x=coord_t::truncate(offset);

	cur_pos.x= -x;
	peephole_element_impl->update_current_position(IN_THREAD, cur_pos);
}

void peepholeObj::layoutmanager_implObj
::update_vertical_scroll(IN_THREAD_ONLY, dim_t offset)
{
	auto peephole_element_impl=element_in_peephole->get_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t y=coord_t::truncate(offset);

	cur_pos.y= -y;
	peephole_element_impl->update_current_position(IN_THREAD, cur_pos);
}

void peepholeObj::layoutmanager_implObj
::update_scrollbar(IN_THREAD_ONLY,
		   const ref<scrollbar_implObj> &scrollbar,
		   const elementimpl &visibility_element,
		   const scrollbar_visibility visibility,
		   coord_t pos,
		   dim_t size,
		   dim_t peephole_size,
		   dim_t increment)
{
	if (visibility == scrollbar_visibility::never)
	{
		visibility_element->request_visibility(IN_THREAD, false);
		return;
	}

	scrollbar_config new_config;

	new_config.range=scroll_v_t::truncate(size);
	new_config.page_size=scroll_v_t::truncate(peephole_size);
	new_config.value=scroll_v_t::truncate(-pos);
	new_config.increment=scroll_v_t::truncate(increment);

	if (new_config.page_size >= new_config.range)
	{
		// Easier for update_config() to optimize itself away.
		new_config.range=new_config.page_size;
		new_config.value=0;
	}

	scrollbar->update_config(IN_THREAD, new_config);

	if (visibility == scrollbar_visibility::always)
		visibility_element->request_visibility(IN_THREAD, true);
	else
		visibility_element->request_visibility(IN_THREAD,
						       new_config.range
						       > new_config.page_size);
}

void peepholeObj::layoutmanager_implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	recalculate(IN_THREAD);
}


LIBCXXW_NAMESPACE_END
