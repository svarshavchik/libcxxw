/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/canvas.H"
#include "x/w/gridfactory.H"
#include "focus/focusable.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "peephole/peepholed_element.H"
#include "scrollbar/scrollbar_impl.H"
#include "catch_exceptions.H"
#include "container.H"
#include "run_as.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::peepholeObj::layoutmanager_implObj)
LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
//
// An intermediate object used by scrollbars' update callback.
//
// When the peephole scrollbar gets dragged, the scrollbar's update
// callback invokes update_value().
//
// The constructor stores either update_horizontal_scrollbar() or
// update_vertical_scroll() in update_func, so the callback value gets passed
// there.
//
// This is constructed before the layout manager, and the link to the layout
// manager must be a weak reference, to avoid circular references.

class peepholeObj::layoutmanager_implObj::callbackObj
	: virtual public obj {

public:
	typedef void (layoutmanager_implObj::*update_func_t)(IN_THREAD_ONLY,
							     dim_t);

	// update_horizontal_scroll() or update_vertical_scroll()
	const update_func_t update_func;

	// Need to find my layout manager.
	typedef mpobj<weakptr<ptr<layoutmanager_implObj>>> my_layoutmanager_t;

	my_layoutmanager_t my_layoutmanager;

	// Constructor
	callbackObj(update_func_t update_func)
		: update_func(update_func)
	{
	}

	inline auto get_layoutmanager()
	{
		my_layoutmanager_t::lock lock{my_layoutmanager};

		return lock->getptr();
	}

	// Scrollbar has been dragged.
	//
	// TODO: this always gets invoked in the connection thread, as such
	// run_as() is not really needed.

	inline void updated_value(const auto &config)
	{
		auto p=get_layoutmanager();

		if (!p)
			return;

		p->container_impl->get_element_impl().THREAD
			->run_as([p, update_func=this->update_func,
				  v=dim_t::truncate(config.dragged_value)]
				 (IN_THREAD_ONLY)
				 {
					 ((*p).*update_func)(IN_THREAD, v);
				 });
	}
};

peephole_scrollbars
create_peephole_scrollbars(const ref<containerObj::implObj> &container)
{

	auto horizontal_impl=
		ref<peepholeObj::layoutmanager_implObj::callbackObj>
		::create(&peepholeObj::layoutmanager_implObj
			 ::update_horizontal_scroll);

	auto vertical_impl=
		ref<peepholeObj::layoutmanager_implObj::callbackObj>
		::create(&peepholeObj::layoutmanager_implObj
			 ::update_vertical_scroll);

	auto horizontal=do_create_h_scrollbar
		(container, scrollbar_config(),
		 [=]
		 (const auto &config)
		 {
			 horizontal_impl->updated_value(config);
		 });

	auto vertical=do_create_v_scrollbar
		(container, scrollbar_config(),
		 [=]
		 (const auto &config)
		 {
			 vertical_impl->updated_value(config);
		 });

	return {horizontal, vertical, horizontal_impl, vertical_impl};
}

void install_peephole_scrollbars(const gridlayoutmanager &lm,
				 const scrollbar &vertical_scrollbar,
				 scrollbar_visibility v_visibility,
				 const gridfactory &row0_factory,
				 const scrollbar &horizontal_scrollbar,
				 scrollbar_visibility h_visibility,
				 const gridfactory &row1_factory)
{
	// Take this opportunity to set the peephole itself, in (0, 0)
	// to absorb any additional space given to the peephole.
	//
	// combo-box's popup peephole is stretched to make sure its width
	// matches the combo-box's element's width, so this gets attribute
	// to the peephole.
	lm->requested_col_width(0, 100);
	lm->requested_row_height(0, 100);

	// Install the scrollbars, and have the grid layout manager not
	// include them in the grid, when they are not visible.
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
	// Note that the resulting order corresponds to the
	// order specified in peepholed_focusableObj::get_impl().

	vertical_scrollbar->get_focus_after(element);

	set_peephole_scrollbar_focus_order(horizontal_scrollbar,
					   vertical_scrollbar);
}

void set_peephole_scrollbar_focus_order(const focusable &horizontal_scrollbar,
					const focusable &vertical_scrollbar)
{
	horizontal_scrollbar->get_focus_after(vertical_scrollbar);
}

void set_top_level_peephole_scrollbar_focus_order
(IN_THREAD_ONLY,
 focusableImplObj &new_element,
 const focusable &horizontal_scrollbar,
 const focusable &vertical_scrollbar)
{
	get_focus_impl_after_in_thread(IN_THREAD, vertical_scrollbar,
				       ref(&new_element));
	get_focus_after_in_thread(IN_THREAD, horizontal_scrollbar,
				  vertical_scrollbar);
}

peephole_scrollbars
::peephole_scrollbars(const scrollbar &horizontal_scrollbar,
		      const scrollbar &vertical_scrollbar,
		      const ref<peepholeObj::layoutmanager_implObj
		      ::callbackObj> &h_callback,
		      const ref<peepholeObj::layoutmanager_implObj
		      ::callbackObj> &v_callback)
	: horizontal_scrollbar(horizontal_scrollbar),
	  vertical_scrollbar(vertical_scrollbar),
	  h_callback(h_callback),
	  v_callback(v_callback)
{
}

peephole_scrollbars::peephole_scrollbars(const peephole_scrollbars &)=default;

peephole_scrollbars::peephole_scrollbars(peephole_scrollbars &&)=default;

peephole_scrollbars::~peephole_scrollbars()=default;

//////////////////////////////////////////////////////////////////////////////

peepholeObj::layoutmanager_implObj
::layoutmanager_implObj(const ref<containerObj::implObj> &container_impl,
			peephole_style style,
			const peepholed &element_in_peephole,

			const peephole_scrollbars &scrollbars,
			const scrollbar_visibility horizontal_scrollbar_visibility,
			const scrollbar_visibility vertical_scrollbar_visibility)
	: layoutmanagerObj::implObj(container_impl),
	style(style),
	element_in_peephole(element_in_peephole),
	h_scrollbar(scrollbars.horizontal_scrollbar),
	v_scrollbar(scrollbars.vertical_scrollbar),
	horizontal_scrollbar_visibility_thread_only(horizontal_scrollbar_visibility),
	vertical_scrollbar_visibility_thread_only(vertical_scrollbar_visibility),
	h_callback(scrollbars.h_callback),
	v_callback(scrollbars.v_callback),
	horizontal_scrollbar_element(scrollbars.horizontal_scrollbar->elementObj::impl),
	vertical_scrollbar_element(scrollbars.vertical_scrollbar->elementObj::impl)
{
}

void peepholeObj::layoutmanager_implObj::initialize_scrollbars()
{
	auto me=ref(this);

	h_callback->my_layoutmanager=me;
	v_callback->my_layoutmanager=me;
}

void peepholeObj::layoutmanager_implObj::vert_scroll_low(IN_THREAD_ONLY,
							 const input_mask &m)
{
	v_scrollbar->impl->to_low(IN_THREAD, m);
}

void peepholeObj::layoutmanager_implObj::vert_scroll_high(IN_THREAD_ONLY,
							  const input_mask &m)
{
	v_scrollbar->impl->to_high(IN_THREAD, m);
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
	callback(element_in_peephole->get_peepholed_element());
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
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	peephole_element_impl->initialize_if_needed(IN_THREAD);

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

	// Stretch the combo-box's peephole to fill its alloted width.
	if (element_pos.width < current_position.width)
	{
		element_pos.width=current_position.width;
	}
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
			 h_scrollbar->impl,
			 horizontal_scrollbar_element,
			 horizontal_scrollbar_visibility(IN_THREAD),
			 element_pos.x, element_pos.width,
			 current_position.width,
			 element_in_peephole->horizontal_increment(IN_THREAD));

	update_scrollbar(IN_THREAD,
			 v_scrollbar->impl,
			 vertical_scrollbar_element,
			 vertical_scrollbar_visibility(IN_THREAD),
			 element_pos.y, element_pos.height,
			 current_position.height,
			 element_in_peephole->vertical_increment(IN_THREAD));
}

void peepholeObj::layoutmanager_implObj
::update_horizontal_scroll(IN_THREAD_ONLY, dim_t offset)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t x=coord_t::truncate(offset);

	cur_pos.x= -x;
	peephole_element_impl->update_current_position(IN_THREAD, cur_pos);
}

void peepholeObj::layoutmanager_implObj
::update_vertical_scroll(IN_THREAD_ONLY, dim_t offset)
{
	auto peephole_element_impl=
		element_in_peephole->get_peepholed_element()->impl;

	auto cur_pos=peephole_element_impl->data(IN_THREAD).current_position;

	coord_t y=coord_t::truncate(offset);

	cur_pos.y= -y;
	peephole_element_impl->update_current_position(IN_THREAD, cur_pos);
}

void peepholeObj::layoutmanager_implObj
::update_scrollbar(IN_THREAD_ONLY,
		   const ref<scrollbarObj::implObj> &scrollbar,
		   const elementimpl &visibility_element,
		   const scrollbar_visibility visibility,
		   coord_t pos,
		   dim_t size,
		   dim_t peephole_size,
		   dim_t increment)
{
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

	// Even when we're not visible we must still religiously do the above
	// and update_config(), so that the scrollbar configuration reflects
	// reality. Scroll wheel-initiated scrolling (pointer button 4/5)
	// may initiate scrolling even when this vertical scrollbar is not
	// visible, and if it's not visible because there's nothing to scroll,
	// the scrollbar metrics should reflect that.

	if (visibility == scrollbar_visibility::never)
		visibility_element->request_visibility(IN_THREAD, false);
	else if (visibility == scrollbar_visibility::always)
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
