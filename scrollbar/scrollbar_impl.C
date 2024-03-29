/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/rectangle.H"
#include "x/w/pixmap.H"
#include "x/w/picture.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/callback_trigger.H"
#include "x/w/impl/background_color.H"
#include "x/w/scrollbar_appearance.H"
#include "busy.H"
#include "scrollbar/scrollbar_impl.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/focus/focusable_element.H"
#include "x/w/impl/icon.H"
#include "x/w/impl/pixmap_with_picture.H"
#include "icon_images_set_element.H"
#include "themedim_element_minoverride.H"
#include "x/w/impl/metrics_horizvertobj.H"
#include "catch_exceptions.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

#define ICONTAG(group,tag) icon_1tag<scrollbar_icons_tags<group>::tag>
#define ICON(group,tag) (ICONTAG(group, tag)::tagged_icon(IN_THREAD))

#define NORMAL(tag) ICON(scrollbar_normal,tag)
#define PRESSED(tag) ICON(scrollbar_pressed,tag)

// Configure the scrollbar for a horizontal or vertical orientation

struct scrollbar_orientation {

	// Use this scratch buffer
	const char *scratch_buffer_id;

	// The vertical axis for horizontal scrollbars and the horizontal
	// axis for vertical scrollbars.
	//
	// Determines which metrics axis gets fixed to the icons' size.

	metrics::axis metrics::horizvert_axi::*minor_metrics_axis;

	// width for horizontal scrollbars, height for vertical scrollbars.
	dim_t rectangle::*major_size;

	// height for horizontal scrollbars, width for vertical scrollbars,
	dim_t rectangle::*minor_size;

	// x for horizontal scrollbar, y for vertical scorllbars
	coord_t rectangle::*major_coord;

	// y for horizontal scrollbar, x for vertical scorllbars
	coord_t rectangle::*minor_coord;

	// Use minimum_size to override the minimum width of the element.
	bool minimum_width;

	// Use minimum_size to override the minimum height of the element.
	bool minimum_height;
};

const scrollbar_orientation horizontal_scrollbar={
	"horiz_scrollbar@libcxx.com",
	&metrics::horizvert_axi::vert,
	&rectangle::width,
	&rectangle::height,
	&rectangle::x,
	&rectangle::y,
	true,
	false,
};

const scrollbar_orientation vertical_scrollbar={
	"vert_scrollbar@libcxx.com",
	&metrics::horizvert_axi::horiz,
	&rectangle::height,
	&rectangle::width,
	&rectangle::y,
	&rectangle::x,
	false,
	true,
};

static child_element_init_params
create_c_e_init_params(const std::string &id,
		       const const_scrollbar_appearance &appearance)
{
	child_element_init_params params{id};

	params.background_color=appearance->background_color;

	return params;
}

static dim_arg minimum_width(const scrollbar_orientation &orientation,
			     const scrollbar_config &config)
{
	return orientation.minimum_width ? config.minimum_size:dim_arg{0};
}

static dim_arg minimum_height(const scrollbar_orientation &orientation,
			      const scrollbar_config &config)
{
	return orientation.minimum_height ? config.minimum_size:dim_arg{0};
}

scrollbar_focusframelayoutimplObj
::scrollbar_focusframelayoutimplObj(const scrollbar_orientation &orientation,
				    const container_impl &parent_container,
				    const ref<focusframecontainer_implObj>
					  &focusframe_container_impl,
				    const element &initial_element)
	: focusframelayoutimplObj{
			parent_container,
			focusframe_container_impl,
			initial_element
		},
	  orientation{orientation}
{
}

scrollbar_focusframelayoutimplObj
::~scrollbar_focusframelayoutimplObj()=default;

void scrollbar_focusframelayoutimplObj
::update_metrics(ONLY IN_THREAD,
		 const metrics::axis &horiz,
		 const metrics::axis &vert)
{

	if (orientation.minimum_width)
	{
		focusframelayoutimplObj::update_metrics
			(IN_THREAD, {0, horiz.preferred(), horiz.maximum()},
			 vert);
		return;
	}

	if (orientation.minimum_height)
	{
		focusframelayoutimplObj::update_metrics
			(IN_THREAD, horiz,
			 {0, vert.preferred(), vert.maximum()});
		return;
	}

	focusframelayoutimplObj::update_metrics(IN_THREAD, horiz, vert);
}


scrollbarObj::implObj::implObj(const scrollbar_impl_init_params &init_params)
	: superclass_t{minimum_width(init_params.orientation, init_params.conf),
		       minimum_height(init_params.orientation,init_params.conf),

		       scrollbar_icon_tuple_t_get(init_params.icons),
		       init_params.container,
		       create_c_e_init_params(init_params.orientation
					      .scratch_buffer_id,
					      init_params.appearance)},
	  orientation{init_params.orientation},
	  state_thread_only{init_params.conf},
	  updated_value_thread_only{init_params.callback},
	  current_value{std::tuple{init_params.conf.value,
		init_params.conf.value}},
	  should_be_enabled_thread_only{true}
{
	validate_conf(THREAD);
}

scrollbarObj::implObj
::scrollbar_icon_set scrollbarObj::implObj::normal_icons(ONLY IN_THREAD)
{
	return {NORMAL(low), NORMAL(high), NORMAL(handlebar_start),
			NORMAL(handlebar), NORMAL(handlebar_end)};
}

scrollbarObj::implObj
::scrollbar_icon_set scrollbarObj::implObj::pressed_icons(ONLY IN_THREAD)
{
	return {PRESSED(low), PRESSED(high), PRESSED(handlebar_start),
			PRESSED(handlebar), PRESSED(handlebar_end)};
}

void scrollbarObj::implObj::validate_conf(ONLY IN_THREAD)
{
	if (state(IN_THREAD).range <= 0)
		state(IN_THREAD).range=1;

	if (state(IN_THREAD).page_size == 0)
		state(IN_THREAD).page_size=1;

	if (state(IN_THREAD).page_size > state(IN_THREAD).range)
		state(IN_THREAD).page_size=state(IN_THREAD).range;

	if (state(IN_THREAD).value > state(IN_THREAD).range-state(IN_THREAD).page_size)
		state(IN_THREAD).value=state(IN_THREAD).range-state(IN_THREAD).page_size;
	dragged_value=state(IN_THREAD).value;

	status=state(IN_THREAD);
}

scrollbarObj::implObj::~implObj()=default;

void scrollbarObj::implObj::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	recalculate_metrics(IN_THREAD);
	calculate_scrollbar_metrics(IN_THREAD);
}

void scrollbarObj::implObj::theme_updated(ONLY IN_THREAD,
					  const const_defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	recalculate_metrics(IN_THREAD);
}

void scrollbarObj::implObj::reconfigure(ONLY IN_THREAD,
					const scrollbar_config &new_config)
{
	if (new_config == state(IN_THREAD))
		return;

	reset_state(IN_THREAD);

	bool minimum_override_changed=
		new_config.minimum_size != state(IN_THREAD).minimum_size;

	state(IN_THREAD)=new_config;
	validate_conf(IN_THREAD);
	calculate_scrollbar_metrics(IN_THREAD);

	if (minimum_override_changed)
	{
		update_minimum_override(IN_THREAD,
					minimum_width(orientation,
						      state(IN_THREAD)),
					minimum_height(orientation,
						       state(IN_THREAD)));
		schedule_full_redraw(IN_THREAD);
	}
	else
	{
		draw_slider(IN_THREAD);
	}
	report_changed_values(IN_THREAD, state(IN_THREAD).value,
			      state(IN_THREAD).value);

}

dim_t scrollbarObj::implObj::major_size(const rectangle &r) const
{
	return r.*(orientation.major_size);
}

dim_t scrollbarObj::implObj::minor_size(const rectangle &r) const
{
	return r.*(orientation.minor_size);
}

rectangle scrollbarObj::implObj::icon_size(const icon &i)
{
	return {0, 0,
			i->image->get_width(),
			i->image->get_height()};
}

dim_t scrollbarObj::implObj::major_icon_size(const icon &i) const
{
	return major_size(icon_size(i));
}

dim_t scrollbarObj::implObj::minor_icon_size(const icon &i) const
{
	return minor_size(icon_size(i));
}

dim_t scrollbarObj::implObj::current_position_major_size(ONLY IN_THREAD) const
{
	return major_size(data(IN_THREAD).current_position);
}

dim_t scrollbarObj::implObj::current_position_minor_size(ONLY IN_THREAD) const
{
	return minor_size(data(IN_THREAD).current_position);
}

dim_t scrollbarObj::implObj::scroll_low_size(ONLY IN_THREAD) const
{
	return major_icon_size(NORMAL(low));
}

dim_t scrollbarObj::implObj::scroll_high_size(ONLY IN_THREAD) const
{
	return major_icon_size(NORMAL(high));
}

coord_t scrollbarObj::implObj::scroll_high_position(ONLY IN_THREAD) const
{
	return coord_t::truncate(data(IN_THREAD).current_position
				 .*(orientation.major_size)-
				 scroll_high_size(IN_THREAD));
}

dim_t scrollbarObj::implObj::slider_size(ONLY IN_THREAD) const
{
	return data(IN_THREAD).current_position
		.*(orientation.major_size)
		- scroll_low_size(IN_THREAD)
		- scroll_high_size(IN_THREAD);
}

dim_t scrollbarObj::implObj::handlebar_start_size(ONLY IN_THREAD) const
{
	return major_icon_size(NORMAL(handlebar_start));
}

dim_t scrollbarObj::implObj::handlebar_size(ONLY IN_THREAD) const
{
	return major_icon_size(NORMAL(handlebar));
}

dim_t scrollbarObj::implObj::handlebar_end_size(ONLY IN_THREAD) const
{
	return major_icon_size(NORMAL(handlebar_end));
}

void scrollbarObj::implObj::recalculate_metrics(ONLY IN_THREAD)
{
	// Use low's size to fix the height of the horizontal
	// scrollbar and the width of the vertical scrollbar.

	auto minor_size=minor_icon_size(NORMAL(low));

	metrics::horizvert_axi new_metrics={
		{0, 0, dim_t::infinite()},
		{0, 0, dim_t::infinite()}
	};

	new_metrics.*(orientation.minor_metrics_axis)=
		{minor_size, minor_size, minor_size};
	get_horizvert(IN_THREAD)->set_element_metrics(IN_THREAD,
						      new_metrics.horiz,
						      new_metrics.vert);
}

void scrollbarObj::implObj::current_position_updated(ONLY IN_THREAD)
{
	// The current_position_updated() may get called even if the
	// element's size has not changed. Only reset_state() if the size
	// actually changed.
	if (calculate_scrollbar_metrics(IN_THREAD))
		reset_state(IN_THREAD);

	superclass_t::current_position_updated(IN_THREAD);
}

void scrollbarObj::implObj::reset_state(ONLY IN_THREAD)
{
	bool was_dragging=dragging;
	bool was_scroll_low_pressed=scroll_low_pressed;
	bool was_scroll_high_pressed=scroll_high_pressed;

	scroll_low_pressed=handlebar_pressed=scroll_high_pressed=dragging=false;

	if (was_scroll_low_pressed)
		draw_scroll_low(IN_THREAD);
	if (was_scroll_high_pressed)
		draw_scroll_high(IN_THREAD);

	if (was_dragging)
		abort_dragging(IN_THREAD);

	if (was_dragging || handlebar_pressed)
		draw_slider(IN_THREAD);
}

void scrollbarObj::implObj::set_enabled(ONLY IN_THREAD, bool flag)
{
	should_be_enabled(IN_THREAD)=flag;

	update_focusable_enabled(IN_THREAD);
}

void scrollbarObj::implObj::update_focusable_enabled(ONLY IN_THREAD)
{
	auto flag=should_be_enabled(IN_THREAD);

	if (metrics.too_small)
		flag=false;
	superclass_t::set_enabled(IN_THREAD, flag);
}

bool scrollbarObj::implObj::calculate_scrollbar_metrics(ONLY IN_THREAD)
{
	auto current_position=data(IN_THREAD).current_position;

	bool changed=current_position.width != metrics_based_on_width ||
		current_position.height != metrics_based_on_height;

	metrics_based_on_width=current_position.width;
	metrics_based_on_height=current_position.height;

	// We must still go through the motions. This could be the initial
	// calculation, in which case metrics_based_on's original contents
	// do not apply.

	// Recalculate metrics based on current size.

	metrics.calculate(dim_t::truncate(scroll_low_size(IN_THREAD)),
			  dim_t::truncate(scroll_high_size(IN_THREAD)),
			  dim_t::truncate(state(IN_THREAD).range),
			  dim_t::truncate(state(IN_THREAD).page_size),
			  dim_t::truncate(current_position
					  .*(orientation.major_size)),
			  dim_squared_t::truncate
			  (handlebar_start_size(IN_THREAD)
			   +handlebar_end_size(IN_THREAD)+1));

	// If the size is big enough for the slider, resize the handlebar
	// accordingly.
	if (!metrics.no_slider)
	{
		current_position.*(orientation.major_size)=
			dim_t::truncate
			(metrics.handlebar_pixel_size-
			 scroll_v_t::truncate(handlebar_start_size(IN_THREAD))-
			 scroll_v_t::truncate(handlebar_end_size(IN_THREAD)));

		ICONTAG(scrollbar_normal, handlebar)::resize
			(IN_THREAD,
			 current_position.width,
			 current_position.height,
			 icon_scale::nomore);

		ICONTAG(scrollbar_pressed, handlebar)::resize
			(IN_THREAD,
			 current_position.width,
			 current_position.height,
			 icon_scale::nomore);

		current_pixel=metrics.value_to_pixel(dragged_value);
	}

	update_focusable_enabled(IN_THREAD);
	return changed;
}

///////////////////////////////////////////////////////////////////////////
//
// Draw all of, or redraw selected parts of the scrollbar.

void scrollbarObj::implObj::do_draw(ONLY IN_THREAD,
				   const draw_info &di,
				   const rectarea &areas)
{
	auto entire_area=bounds(areas);

	clip_region_set clipped{IN_THREAD, get_window_handler(), di};

	const auto &[low, slider, high]=regions(IN_THREAD);

	if (low.overlaps(entire_area))
	{
		draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const auto &picture,
			  const auto &pixmap,
			  const auto &gc)
			 {
				 this->do_draw_scroll_low(IN_THREAD, picture);
			 },
			 low, di, di, clipped);
	}

	if (slider.overlaps(entire_area))
	{
		draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const auto &picture,
			  const auto &pixmap,
			  const auto &gc)
			 {
				 this->do_draw_slider(IN_THREAD, picture);
			 },
			 slider, di, di, clipped);
	}

	if (high.overlaps(entire_area))
	{
		draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const auto &picture,
			  const auto &pixmap,
			  const auto &gc)
			 {
				 this->do_draw_scroll_high(IN_THREAD, picture);
			 },
			 high, di, di, clipped);
	}

#ifdef DEBUG_SCROLL
	DEBUG_SCROLL();
#endif
}

void scrollbarObj::implObj::draw_scroll_low(ONLY IN_THREAD)
{
	const auto &[low, slider, high]=regions(IN_THREAD);

	schedule_redraw(IN_THREAD, low);
}

void scrollbarObj::implObj::do_draw_scroll_low(ONLY IN_THREAD,
					       const picture &buffer)
{
	if (metrics.too_small)
		return;
	do_draw_icon(IN_THREAD, buffer,
		     &scrollbar_icon_set::low,
		     scroll_low_pressed,
		     0);
}

void scrollbarObj::implObj::draw_scroll_high(ONLY IN_THREAD)
{
	const auto &[low, slider, high]=regions(IN_THREAD);

	schedule_redraw(IN_THREAD, high);
}

void scrollbarObj::implObj::do_draw_scroll_high(ONLY IN_THREAD,
						const picture &buffer)
{
	if (metrics.too_small)
		return;

	do_draw_icon(IN_THREAD, buffer,
		     &scrollbar_icon_set::high,
		     scroll_high_pressed,
		     0);
}

void scrollbarObj::implObj::draw_slider(ONLY IN_THREAD)
{
	const auto &[low, slider, high]=regions(IN_THREAD);

	schedule_redraw(IN_THREAD, slider);
}

void scrollbarObj::implObj::do_draw_slider(ONLY IN_THREAD,
					   const picture &buffer)
{
	if (metrics.too_small || metrics.no_slider)
		return;

	auto c=do_draw_icon(IN_THREAD, buffer,
			    &scrollbar_icon_set::handlebar_start,
			    handlebar_pressed,
			    coord_t::truncate(current_pixel));

	c=do_draw_icon(IN_THREAD, buffer,
		       &scrollbar_icon_set::handlebar,
		       handlebar_pressed, c);

	do_draw_icon(IN_THREAD, buffer,
		     &scrollbar_icon_set::handlebar_end,
		     handlebar_pressed, c);
}

coord_t scrollbarObj::implObj::do_draw_icon(ONLY IN_THREAD,
					    const picture &buffer,
					    icon scrollbar_icon_set::*which_icon,
					    bool pressed,
					    coord_t major_coord)
{
	auto group=pressed ? pressed_icons(IN_THREAD):normal_icons(IN_THREAD);
	auto &icon=group.*which_icon;

	const auto &p=icon->image;

	rectangle r{0, 0, p->get_width(), p->get_height()};

	r.*(orientation.major_coord)=major_coord;

	buffer->composite(p->icon_picture, 0, 0, r, render_pict_op::op_over);

	return coord_t::truncate(major_coord +
				 r.*(orientation.major_size));
}

/////////////////////////////////////////////////////////////////////////////
//
// Process keyboard or pointer button events.

bool scrollbarObj::implObj::process_key_event(ONLY IN_THREAD,
					      const key_event &ke)
{
	switch (ke.keysym) {
	case XK_Left:
	case XK_Up:
	case XK_KP_Left:
	case XK_KP_Up:
		abort_handlebar(IN_THREAD);
		if (scroll_low_pressed == ke.keypress)
			break;

		scroll_low_pressed=ke.keypress;
		draw_scroll_low(IN_THREAD);

		if (!activate_for(ke))
			to_low(IN_THREAD, ke);
		break;

	case XK_Right:
	case XK_KP_Right:
	case XK_Down:
	case XK_KP_Down:
		abort_handlebar(IN_THREAD);
		if (scroll_high_pressed == ke.keypress)
			break;

		scroll_high_pressed=ke.keypress;
		draw_scroll_high(IN_THREAD);

		if (!activate_for(ke))
			to_high(IN_THREAD, ke);
		break;
	case XK_Page_Up:
	case XK_KP_Page_Up:
		abort_handlebar(IN_THREAD);
		if (scroll_low_pressed == ke.keypress)
			break;

		scroll_low_pressed=ke.keypress;
		draw_scroll_low(IN_THREAD);

		if (!activate_for(ke))
			page_up(IN_THREAD);
		break;
	case XK_Page_Down:
	case XK_KP_Page_Down:
		abort_handlebar(IN_THREAD);
		if (scroll_high_pressed == ke.keypress)
			break;

		scroll_high_pressed=ke.keypress;
		draw_scroll_high(IN_THREAD);

		if (!activate_for(ke))
			page_down(IN_THREAD);
		break;
	default:
		return superclass_t::process_key_event(IN_THREAD, ke);
	}
	return true;
}

void scrollbarObj::implObj::abort_handlebar(ONLY IN_THREAD)
{
	if (handlebar_pressed)
	{
		handlebar_pressed=false;
		draw_slider(IN_THREAD);
	}
}

bool scrollbarObj::implObj::to_low(ONLY IN_THREAD,
				   const input_mask &mask)
{
	if (state(IN_THREAD).value > 0)
	{
		state(IN_THREAD).value=mask.ctrl ? state(IN_THREAD).value-1
			: state(IN_THREAD).value < state(IN_THREAD).increment
				       ? 0:state(IN_THREAD).value-state(IN_THREAD).increment;
		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::to_high(ONLY IN_THREAD,
				    const input_mask &mask)
{
	if (state(IN_THREAD).value < state(IN_THREAD).range-state(IN_THREAD).page_size)
	{
		state(IN_THREAD).value=mask.ctrl ? state(IN_THREAD).value+1
			: state(IN_THREAD).value+state(IN_THREAD).increment;

		if (state(IN_THREAD).value > state(IN_THREAD).range-state(IN_THREAD).page_size)
			state(IN_THREAD).value=state(IN_THREAD).range-state(IN_THREAD).page_size;

		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::page_up(ONLY IN_THREAD)
{
	if (state(IN_THREAD).value > 0)
	{
		state(IN_THREAD).value=state(IN_THREAD).value <
			state(IN_THREAD).page_size ? 0:
			state(IN_THREAD).value - state(IN_THREAD).page_size;
		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::page_down(ONLY IN_THREAD)
{
	if (state(IN_THREAD).value < state(IN_THREAD).range-state(IN_THREAD).page_size)
	{
		state(IN_THREAD).value += state(IN_THREAD).page_size;

		if (state(IN_THREAD).value >
		    state(IN_THREAD).range-state(IN_THREAD).page_size)
			state(IN_THREAD).value=state(IN_THREAD).range-
				state(IN_THREAD).page_size;
		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::process_button_event(ONLY IN_THREAD,
						 const button_event &be,
						 xcb_timestamp_t timestamp)
{
	bool flag=superclass_t::process_button_event(IN_THREAD, be, timestamp);
	if (be.button != 1)
		return flag;

	rectangle r;

	r.x=data(IN_THREAD).last_motion_x;
	r.y=data(IN_THREAD).last_motion_y;

	bool within_boundaries=
		(r.*(orientation.major_coord) >= 0 &&
		 dim_t::truncate(r.*(orientation.major_coord)) <
		 data(IN_THREAD).current_position.*(orientation.major_size)) &&
		r.*(orientation.minor_coord) >= 0 &&
		dim_t::truncate(r.*(orientation.minor_coord)) <
			data(IN_THREAD).current_position
			.*(orientation.minor_size);

	// If something is already pressed, release it.

	bool was_dragging=dragging;
	dragging=false;

	if (handlebar_pressed)
	{
		// Shouldn't have scroll states as pressed, but if so,
		// un-press them.

		if (scroll_low_pressed)
		{
			scroll_low_pressed=false;
			draw_scroll_low(IN_THREAD);
		}

		if (scroll_high_pressed)
		{
			scroll_high_pressed=false;
			draw_scroll_high(IN_THREAD);
		}

		handlebar_pressed=false;

		if (!be.press)
		{
			if (within_boundaries)
			{
				auto clicked_pixel_value=
					scroll_v_t::truncate(r.*(orientation
							      .major_coord));
				auto what=metrics.pixel_to_value
					(clicked_pixel_value);

				if (!what.lo && !what.hi && was_dragging)
					// Finished dragging
				{
					if (dragged_value != state(IN_THREAD).value)
					{
						state(IN_THREAD).value=dragged_value;
						report_changed_values
							(IN_THREAD,
							 state(IN_THREAD).value,
							 state(IN_THREAD).value)
							;
					}
				}
			}

#if 0
			else
			{
				// Drag released with pointer outside of the
				// slider.
				//
				// -- This proved to be counter-intuitive
				if (was_dragging &&
				    dragged_value != state(IN_THREAD).value)
					abort_dragging(IN_THREAD);
			}
#endif
		}
		draw_slider(IN_THREAD);
		return true;
	}

	// Figure out what was pressed.

	auto clicked_pixel_value=
			scroll_v_t::truncate(r.*(orientation.major_coord));
	auto what=metrics.pixel_to_value(clicked_pixel_value);

	if (what.lo)
	{
		if (scroll_low_pressed == be.press)
			return true;

		scroll_low_pressed=be.press;

		draw_scroll_low(IN_THREAD);

		if (activate_for(be))
			to_low(IN_THREAD, be);
	}
	else if (what.hi)
	{
		if (scroll_high_pressed == be.press)
			return true;

		scroll_high_pressed=be.press;

		draw_scroll_high(IN_THREAD);
		if (activate_for(be))
			to_high(IN_THREAD, be);
	}
	else
	{
		if (scroll_low_pressed)
		{
			scroll_low_pressed=false;
			draw_scroll_low(IN_THREAD);
		}

		if (scroll_high_pressed)
		{
			scroll_high_pressed=false;
			draw_scroll_high(IN_THREAD);
		}

		if (clicked_pixel_value < 0 ||
		    dim_squared_t::truncate(clicked_pixel_value) <
		    dim_squared_t::truncate(current_pixel) +
		    scroll_low_size(IN_THREAD))
		{
			if (activate_for(be))
				page_up(IN_THREAD);
			return true;
		}

		if (clicked_pixel_value >= 0 &&
		    dim_squared_t::truncate(clicked_pixel_value) >
		    dim_squared_t::truncate(current_pixel) +
		    scroll_low_size(IN_THREAD)
		    + dim_squared_t::truncate(metrics.handlebar_pixel_size))
		{
			if (activate_for(be))
				page_down(IN_THREAD);
			return true;
		}
		handlebar_pressed=be.press;
		dragging=be.press;

		if (!be.press)
			return true;

		drag_start=clicked_pixel_value;
		drag_start_current_pixel=current_pixel;
		dragged_value=state(IN_THREAD).value;
		draw_slider(IN_THREAD);
		grab(IN_THREAD, &be);
	}

	return true;
}

void scrollbarObj::implObj::abort_dragging(ONLY IN_THREAD)
{
	dragged_value=state(IN_THREAD).value;
	current_pixel=drag_start_current_pixel;
	report_changed_values(IN_THREAD, state(IN_THREAD).value,
			     state(IN_THREAD).value);
}

void scrollbarObj::implObj::report_motion_event(ONLY IN_THREAD,
						const motion_event &me)
{
	// Previous motion position.
	rectangle r{data(IN_THREAD).last_motion_x,
			data(IN_THREAD).last_motion_y};

	auto prev_pointer_pos=r.*(orientation.major_coord);

	superclass_t::report_motion_event(IN_THREAD, me);

	if (metrics.too_small || metrics.no_slider || !dragging)
		return;

	r.x=me.x;
	r.y=me.y;

	auto pointer_pos=r.*(orientation.major_coord);

	if (prev_pointer_pos == pointer_pos)
		return; // Didn't move in the major direction

	// Rather then taking clicked_pixed_value as is, we saved the
	// pointer position when dragging started as drag_start, and the
	// current_pixel value, at the time, as drag_start_current_pixel.
	//
	// First, compute how far clicked_pixed_value has moved away from
	// drag_start, then add it to drag_start_current_pixel, and use that
	// as the real pointer_pos.

	if (pointer_pos > drag_start)
	{
		pointer_pos=coord_t::truncate
			(scroll_v_t::truncate(pointer_pos-drag_start)+
			 drag_start_current_pixel);
	}
	else
	{
		pointer_pos=coord_t::truncate
			(coord_squared_t(coord_squared_t::truncate
					 (drag_start_current_pixel))
			 - dim_t::truncate(drag_start-pointer_pos));
	}

	// pixel_to_value() expects to see the absolute pixel value,
	// interpreting it as a click on the center of the handlebar,
	// adjust pointer_pos by scroll_low_size, and half the
	// handlebar's size, then see what it says.

	auto v=metrics.pixel_to_value
		(scroll_v_t::truncate
		 (pointer_pos+scroll_low_size(IN_THREAD)+
		  scroll_v_t::truncate(metrics.handlebar_pixel_size/2)));


	if (v.lo)
	{
		pointer_pos=0;
		v.value=0;
	}
	else if (v.hi)
	{
		pointer_pos=
			dim_t::truncate(slider_size(IN_THREAD)-
					dim_t::truncate(metrics
							.handlebar_pixel_size));
		v.value=state(IN_THREAD).range-state(IN_THREAD).page_size;
	}

	auto old_value=dragged_value;

	dragged_value=v.value;

	if (pointer_pos < 0)
		pointer_pos=0;

	auto max_value=dim_t::truncate(slider_size(IN_THREAD))
		- metrics.handlebar_pixel_size;

	if (scroll_v_t::truncate(pointer_pos) > max_value)
		pointer_pos=coord_t::truncate(max_value);

	if (pointer_pos != coord_t::truncate(current_pixel))
	{
		current_pixel=coord_t::truncate(pointer_pos);
		draw_slider(IN_THREAD);
	}

	if (old_value != dragged_value)
		report_changed_values(IN_THREAD, state(IN_THREAD).value,
				     dragged_value);
}

void scrollbarObj::implObj::keyboard_focus(ONLY IN_THREAD,
					   const callback_trigger_t &trigger)
{
	superclass_t::keyboard_focus(IN_THREAD, trigger);

	if (!current_keyboard_focus(IN_THREAD))
		reset_state(IN_THREAD);
}

//////////////////////////////////////////////////////////////////////////
//
// Report discrete changes to the scrollbar's value.

void scrollbarObj::implObj::updated_value_no_drag(ONLY IN_THREAD)
{
	current_pixel=metrics.value_to_pixel(state(IN_THREAD).value);
	dragged_value=state(IN_THREAD).value;

	draw_slider(IN_THREAD);
	report_changed_values(IN_THREAD, state(IN_THREAD).value, dragged_value);
}

void scrollbarObj::implObj::update_callback(ONLY IN_THREAD,
					    const scrollbar_cb_t &callback)
{
	updated_value(IN_THREAD)=callback;

	auto v=current_value.get();

	report_current_values(IN_THREAD, std::get<0>(v), std::get<1>(v),
			      initial{});
}

void scrollbarObj::implObj::report_changed_values(ONLY IN_THREAD,
						  scroll_v_t value,
						  scroll_v_t dragged_value)
{
	std::tuple new_values{value, dragged_value};

	{
		current_value_t::lock lock{current_value};

		if (*lock == new_values)
			return;

		*lock=new_values;
	}

	report_current_values(IN_THREAD, value, dragged_value, {});
}

void scrollbarObj::implObj::report_current_values(ONLY IN_THREAD,
						  scroll_v_t value,
						  scroll_v_t dragged_value,
						  const callback_trigger_t &why)
{
	try {
		updated_value(IN_THREAD)
			(IN_THREAD,
			 scrollbar_info_t{(scroll_v_t::value_type)value,
					(scroll_v_t::value_type)dragged_value,
					why,
					busy_impl{*this}
			});
	} REPORT_EXCEPTIONS(this);
}

std::tuple<rectangle, rectangle,
	   rectangle> scrollbarObj::implObj::regions(ONLY IN_THREAD) const
{
	rectangle all=data(IN_THREAD).current_position;

	all.x=0;
	all.y=0;

	std::tuple<rectangle, rectangle, rectangle> ret{all, all, all};

	auto &[low, slider, high]=ret;

	metrics.regions(low, slider, high,
			orientation.major_coord,
			orientation.major_size);

	return ret;
}

LIBCXXW_NAMESPACE_END
