/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/rectangle.H"
#include "x/w/pixmap.H"
#include "x/w/picture.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "scrollbar/scrollbar_impl.H"
#include "focus/focusable_element.H"
#include "icon.H"
#include "icon_image.H"
#include "metrics_horizvertobj.H"
#include "catch_exceptions.H"
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

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
};

const scrollbar_orientation horizontal_scrollbar={
	"horiz_scrollbar@libcxx",
	&metrics::horizvert_axi::vert,
	&rectangle::width,
	&rectangle::height,
	&rectangle::x,
	&rectangle::y,
};

const scrollbar_orientation vertical_scrollbar={
	"vert_scrollbar@libcxx",
	&metrics::horizvert_axi::horiz,
	&rectangle::height,
	&rectangle::width,
	&rectangle::y,
	&rectangle::x,
};

scrollbarObj::implObj::implObj(const scrollbar_impl_init_params &init_params)
	: superclass_t(init_params.container, metrics::horizvert_axi(),
		       init_params.orientation.scratch_buffer_id),
	  orientation(init_params.orientation),
	  icon_set_1(init_params.icon_set_1),
	  icon_set_2(init_params.icon_set_2),
	  conf(init_params.conf)
{
	validate_conf();
}

void scrollbarObj::implObj::validate_conf()
{
	if (conf.range <= 0)
		conf.range=1;

	if (conf.page_size == 0)
		conf.page_size=1;

	if (conf.page_size > conf.range)
		conf.page_size=conf.range;

	if (conf.value > conf.range-conf.page_size)
		conf.value=conf.range-conf.page_size;
	dragged_value=conf.value;
}

scrollbarObj::implObj::~implObj()=default;

void scrollbarObj::implObj::initialize(IN_THREAD_ONLY)
{
	icon_set_1.initialize(IN_THREAD);
	icon_set_2.initialize(IN_THREAD);
	recalculate_metrics(IN_THREAD);
	calculate_scrollbar_metrics(IN_THREAD);
	superclass_t::initialize(IN_THREAD);
}

void scrollbarObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	icon_set_1.theme_updated(IN_THREAD);
	icon_set_2.theme_updated(IN_THREAD);
	recalculate_metrics(IN_THREAD);
	superclass_t::theme_updated(IN_THREAD);
}

void scrollbarObj::implObj::update_config(IN_THREAD_ONLY,
					  const scrollbar_config &new_config)
{
	if (new_config == conf)
		return;

	reset_state(IN_THREAD);
	conf=new_config;
	validate_conf();
	calculate_scrollbar_metrics(IN_THREAD);
	draw_slider(IN_THREAD);
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
			i->image->icon_pixmap->get_width(),
			i->image->icon_pixmap->get_height()};
}

dim_t scrollbarObj::implObj::major_icon_size(const icon &i) const
{
	return major_size(icon_size(i));
}

dim_t scrollbarObj::implObj::minor_icon_size(const icon &i) const
{
	return minor_size(icon_size(i));
}

dim_t scrollbarObj::implObj::current_position_major_size(IN_THREAD_ONLY) const
{
	return major_size(data(IN_THREAD).current_position);
}

dim_t scrollbarObj::implObj::current_position_minor_size(IN_THREAD_ONLY) const
{
	return minor_size(data(IN_THREAD).current_position);
}

dim_t scrollbarObj::implObj::scroll_low_size() const
{
	return major_icon_size(icon_set_1.scroll_low_icon);
}

dim_t scrollbarObj::implObj::scroll_high_size() const
{
	return major_icon_size(icon_set_1.scroll_high_icon);
}

coord_t scrollbarObj::implObj::scroll_high_position(IN_THREAD_ONLY) const
{
	return coord_t::truncate(data(IN_THREAD).current_position
				 .*(orientation.major_size)-
				 scroll_high_size());
}

dim_t scrollbarObj::implObj::slider_size(IN_THREAD_ONLY) const
{
	return data(IN_THREAD).current_position
		.*(orientation.major_size)
		- scroll_low_size()
		- scroll_high_size();
}

dim_t scrollbarObj::implObj::handlebar_start_size() const
{
	return major_icon_size(icon_set_1.handlebar_start_icon);
}

dim_t scrollbarObj::implObj::handlebar_size() const
{
	return major_icon_size(icon_set_1.handlebar_icon);
}

dim_t scrollbarObj::implObj::handlebar_end_size() const
{
	return major_icon_size(icon_set_1.handlebar_end_icon);
}

void scrollbarObj::implObj::recalculate_metrics(IN_THREAD_ONLY)
{
	// Use scroll_low_icon's size to fix the height of the horizontal
	// scrollbar and the width of the vertical scrollbar.

	auto minor_size=minor_icon_size(icon_set_1.scroll_low_icon);

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

void scrollbarObj::implObj::current_position_updated(IN_THREAD_ONLY)
{
	// The current_position_updated() may get called even if the
	// element's size has not changed. Only reset_state() if the size
	// actually changed.
	if (calculate_scrollbar_metrics(IN_THREAD))
		reset_state(IN_THREAD);

	superclass_t::current_position_updated(IN_THREAD);
}

void scrollbarObj::implObj::reset_state(IN_THREAD_ONLY)
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

bool scrollbarObj::implObj::calculate_scrollbar_metrics(IN_THREAD_ONLY)
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

	metrics.calculate(dim_t::truncate(scroll_low_size()),
			  dim_t::truncate(scroll_high_size()),
			  dim_t::truncate(conf.range),
			  dim_t::truncate(conf.page_size),
			  dim_t::truncate(current_position
					  .*(orientation.major_size)),
			  dim_squared_t::truncate
			  (handlebar_start_size()+handlebar_end_size()+1));

	// If the size is big enough for the slider, resize the handlebar_icon
	// accordingly.
	if (!metrics.no_slider)
	{
		current_position.*(orientation.major_size)=
			dim_t::truncate
			(metrics.handlebar_pixel_size-
			 scroll_v_t::truncate(handlebar_start_size())-
			 scroll_v_t::truncate(handlebar_end_size()));

		icon_set_1.handlebar_icon=
			icon_set_1.handlebar_icon->resize
			(IN_THREAD,
			 current_position.width,
			 current_position.height);

		icon_set_2.handlebar_icon=
			icon_set_2.handlebar_icon->resize
			(IN_THREAD,
			 current_position.width,
			 current_position.height);

		current_pixel=metrics.value_to_pixel(dragged_value);
	}

	return changed;
}

///////////////////////////////////////////////////////////////////////////
//
// Draw all of, or redraw selected parts of the scrollbar.

void scrollbarObj::implObj::do_draw(IN_THREAD_ONLY,
				   const draw_info &di,
				   const rectangle_set &areas)
{
	// We just draw the entire universe.

	clip_region_set clipped{IN_THREAD, di};

	draw_using_scratch_buffer
		(IN_THREAD,
		 [&, this]
		 (const auto &picture,
		  const auto &pixmap,
		  const auto &gc)
		 {
			 this->do_draw_scroll_low(IN_THREAD,
						  picture);
			 this->do_draw_scroll_high(IN_THREAD,
						   picture,
						   this->scroll_high_position
						   (IN_THREAD));

			 this->do_draw_slider(IN_THREAD,
					      picture,
					      dim_t::truncate
					      (this->scroll_low_size()));
		 },
		 {0, 0, data(IN_THREAD).current_position.width,
				 data(IN_THREAD).current_position.height},
		 di, di, clipped);
}

void scrollbarObj::implObj::draw_scroll_low(IN_THREAD_ONLY)
{
	if (metrics.too_small)
		return;

	auto &di=get_draw_info(IN_THREAD);

	clip_region_set clipped{IN_THREAD, di};

	rectangle r=data(IN_THREAD).current_position;

	r.x=0;
	r.y=0;

	r.*(orientation.major_size)=scroll_low_size();

	draw_using_scratch_buffer
		(IN_THREAD,
		 [&, this]
		 (const auto &picture,
		  const auto &pixmap,
		  const auto &gc)
		 {
			 this->do_draw_scroll_low(IN_THREAD,
						  picture);
		 },
		 r, di, di, clipped);
}

void scrollbarObj::implObj::do_draw_scroll_low(IN_THREAD_ONLY,
					       const picture &buffer)
{
	if (metrics.too_small)
		return;
	do_draw_icon(IN_THREAD, buffer,
		     &scrollbar_icon_set::scroll_low_icon,
		     scroll_low_pressed,
		     0);
}

void scrollbarObj::implObj::draw_scroll_high(IN_THREAD_ONLY)
{
	if (metrics.too_small)
		return;

	auto &di=get_draw_info(IN_THREAD);

	clip_region_set clipped{IN_THREAD, di};

	rectangle r=data(IN_THREAD).current_position;

	r.x=0;
	r.y=0;

	r.*(orientation.major_coord)=scroll_high_position(IN_THREAD);
	r.*(orientation.major_size)=scroll_high_size();

	draw_using_scratch_buffer
		(IN_THREAD,
		 [&, this]
		 (const auto &picture,
		  const auto &pixmap,
		  const auto &gc)
		 {
			 this->do_draw_scroll_high(IN_THREAD,
						   picture, 0);
		 },
		 r, di, di, clipped);
}

void scrollbarObj::implObj::do_draw_scroll_high(IN_THREAD_ONLY,
						const picture &buffer,
						coord_t coordinate)
{
	if (metrics.too_small)
		return;
	do_draw_icon(IN_THREAD, buffer,
		     &scrollbar_icon_set::scroll_high_icon,
		     scroll_high_pressed,
		     coordinate);
}

void scrollbarObj::implObj::draw_slider(IN_THREAD_ONLY)
{
	if (metrics.too_small)
		return;

	auto &di=get_draw_info(IN_THREAD);

	clip_region_set clipped{IN_THREAD, di};

	rectangle r=data(IN_THREAD).current_position;

	r.x=0;
	r.y=0;

	r.*(orientation.major_coord)=coord_t::truncate(scroll_low_size());
	r.*(orientation.major_size)=slider_size(IN_THREAD);

	draw_using_scratch_buffer
		(IN_THREAD,
		 [&, this]
		 (const auto &picture,
		  const auto &pixmap,
		  const auto &gc)
		 {
			 if (metrics.no_slider)
				 return;
			 this->do_draw_slider(IN_THREAD, picture, 0);
		 },
		 r, di, di, clipped);
}

void scrollbarObj::implObj::do_draw_slider(IN_THREAD_ONLY,
					   const picture &buffer,
					   coord_t coordinate)
{
	if (metrics.too_small || metrics.no_slider)
		return;

	auto s=current_pixel+coord_t::truncate(coordinate);

	auto c=do_draw_icon(IN_THREAD, buffer,
			    &scrollbar_icon_set::handlebar_start_icon,
			    handlebar_pressed, coord_t::truncate(s));

	c=do_draw_icon(IN_THREAD, buffer,
		       &scrollbar_icon_set::handlebar_icon,
		       handlebar_pressed, c);

	do_draw_icon(IN_THREAD, buffer,
		     &scrollbar_icon_set::handlebar_end_icon,
		     handlebar_pressed, c);
}

coord_t scrollbarObj::implObj::do_draw_icon(IN_THREAD_ONLY,
					    const picture &buffer,
					    icon scrollbar_icon_set::*which_icon,
					    bool pressed,
					    coord_t major_coord)
{
	auto &icon=pressed ? icon_set_2.*which_icon:icon_set_1.*which_icon;

	const auto &p=icon->image;

	rectangle r{0, 0, p->icon_pixmap->get_width(),
			p->icon_pixmap->get_height()};

	r.*(orientation.major_coord)=major_coord;

	buffer->composite(p->icon_picture, 0, 0, r, render_pict_op::op_over);

	return coord_t::truncate(major_coord +
				 r.*(orientation.major_size));
}

/////////////////////////////////////////////////////////////////////////////
//
// Process keyboard or pointer button events.

bool scrollbarObj::implObj::process_key_event(IN_THREAD_ONLY,
					      const key_event &ke)
{
	switch (ke.keysym) {
	case XK_Left:
	case XK_KP_Left:
	case XK_Right:
	case XK_KP_Right:
	case XK_Up:
	case XK_KP_Up:
	case XK_Down:
	case XK_KP_Down:

		// Common processing. The main switch loop below ends up
		// handling only a valid key release, for these keys.

		if (ke.keypress)
		{
			if (!handlebar_pressed)
			{
				handlebar_pressed=true;
				draw_slider(IN_THREAD);
			}
			return true;
		}
		else
		{
			if (!handlebar_pressed)
				return true;
			handlebar_pressed=false;
		}
	}

	switch (ke.keysym) {
	case XK_Left:
	case XK_Up:
	case XK_KP_Left:
	case XK_KP_Up:
		to_low(IN_THREAD, ke);
		return true;

	case XK_Right:
	case XK_KP_Right:
	case XK_Down:
	case XK_KP_Down:
		to_high(IN_THREAD, ke);
		return true;

	case XK_Page_Up:
	case XK_KP_Page_Up:
		if (ke.keypress)
		{
			if (!scroll_low_pressed)
			{
				scroll_low_pressed=true;
				draw_scroll_low(IN_THREAD);
			}
		}
		else
		{
			if (scroll_low_pressed)
			{
				scroll_low_pressed=false;
				draw_scroll_low(IN_THREAD);
				page_up(IN_THREAD);
			}
		}
		return true;
	case XK_Page_Down:
	case XK_KP_Page_Down:
		if (ke.keypress)
		{
			if (!scroll_high_pressed)
			{
				scroll_high_pressed=true;
				draw_scroll_high(IN_THREAD);
			}
		}
		else
		{
			if (scroll_high_pressed)
			{
				scroll_high_pressed=false;
				draw_scroll_high(IN_THREAD);
				page_down(IN_THREAD);
			}
		}
		return true;
	}
	return superclass_t::process_key_event(IN_THREAD, ke);
}

bool scrollbarObj::implObj::to_low(IN_THREAD_ONLY,
				   const input_mask &mask)
{
	if (conf.value > 0)
	{
		conf.value=mask.ctrl ? conf.value-1
			: conf.value < conf.increment
				       ? 0:conf.value-conf.increment;
		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::to_high(IN_THREAD_ONLY,
				    const input_mask &mask)
{
	if (conf.value < conf.range-conf.page_size)
	{
		conf.value=mask.ctrl ? conf.value+1
			: conf.value+conf.increment;

		if (conf.value > conf.range-conf.page_size)
			conf.value=conf.range-conf.page_size;

		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::page_up(IN_THREAD_ONLY)
{
	if (conf.value > 0)
	{
		conf.value=conf.value <
			conf.page_size ? 0:
			conf.value - conf.page_size;
		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::page_down(IN_THREAD_ONLY)
{
	if (conf.value < conf.range-conf.page_size)
	{
		conf.value += conf.page_size;

		if (conf.value >
		    conf.range-conf.page_size)
			conf.value=conf.range-
				conf.page_size;
		updated_value_no_drag(IN_THREAD);
		return true;
	}
	return false;
}

bool scrollbarObj::implObj::process_button_event(IN_THREAD_ONLY,
						 const button_event &be,
						 xcb_timestamp_t timestamp)
{
	if (be.button != 1)
		return superclass_t::process_button_event(IN_THREAD, be,
							  timestamp);

	if (be.press)
		set_focus_only(IN_THREAD);

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

	if (handlebar_pressed)
	{
		handlebar_pressed=false;

		if (!be.press)
		{
			if (within_boundaries)
			{
				auto clicked_pixel_value=
					coord_t::truncate(r.*(orientation
							      .major_coord));
				auto what=metrics.pixel_to_value
					(clicked_pixel_value);

				if (!what.lo && !what.hi && was_dragging)
					// Finished dragging
				{
					if (dragged_value != conf.value)
					{
						conf.value=dragged_value;
						updated_value(IN_THREAD,
							      conf.value,
							      conf.value);
					}
				}
			}
			else
			{
				// Drag released with pointer outside of the
				// slider.
				if (was_dragging &&
				    dragged_value != conf.value)
					abort_dragging(IN_THREAD);
			}
		}
		draw_slider(IN_THREAD);
	}

	// If this is a button release, we're done.
	if (!be.press)
		return true;

	// Figure out what was pressed.

	auto clicked_pixel_value=
			coord_t::truncate(r.*(orientation.major_coord));
	auto what=metrics.pixel_to_value(clicked_pixel_value);

	if (what.lo)
	{
		scroll_low_pressed=true;
		to_low(IN_THREAD, be);
		draw_scroll_low(IN_THREAD);
	}
	else if (what.hi)
	{
		scroll_high_pressed=true;
		to_high(IN_THREAD, be);
		draw_scroll_high(IN_THREAD);
	}
	else
	{
		if (clicked_pixel_value < 0 ||
		    dim_squared_t::truncate(clicked_pixel_value) <
		    dim_squared_t::truncate(current_pixel) + scroll_low_size())
		{
			page_up(IN_THREAD);
			return true;
		}

		if (clicked_pixel_value >= 0 &&
		    dim_squared_t::truncate(clicked_pixel_value) >
		    dim_squared_t::truncate(current_pixel) + scroll_low_size()
		    + dim_squared_t::truncate(metrics.handlebar_pixel_size))
		{
			page_down(IN_THREAD);
			return true;
		}
		handlebar_pressed=true;
		dragging=true;
		drag_start=clicked_pixel_value;
		drag_start_current_pixel=current_pixel;
		dragged_value=conf.value;
		draw_slider(IN_THREAD);
		grab(IN_THREAD);
	}

	return true;
}

void scrollbarObj::implObj::abort_dragging(IN_THREAD_ONLY)
{
	dragged_value=conf.value;
	current_pixel=drag_start_current_pixel;
	updated_value(IN_THREAD, conf.value, conf.value);
}

void scrollbarObj::implObj::motion_event(IN_THREAD_ONLY, coord_t x, coord_t y,
					 const input_mask &mask)
{
	// Previous motion position.
	rectangle r{data(IN_THREAD).last_motion_x,
			data(IN_THREAD).last_motion_y};

	auto prev_pointer_pos=r.*(orientation.major_coord);

	superclass_t::motion_event(IN_THREAD, x, y, mask);

	if (metrics.too_small || metrics.no_slider || !dragging)
		return;

	r.x=x;
	r.y=y;

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
		(coord_t::truncate
		 (pointer_pos+scroll_low_size()+
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
		v.value=conf.range-conf.page_size;
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
		updated_value(IN_THREAD, conf.value, dragged_value);
}

void scrollbarObj::implObj::keyboard_focus(IN_THREAD_ONLY,
					   focus_change event,
					   const ref<elementObj::implObj> &ptr)
{
	superclass_t::keyboard_focus(IN_THREAD, event, ptr);

	if (!current_keyboard_focus(IN_THREAD))
		reset_state(IN_THREAD);
}

//////////////////////////////////////////////////////////////////////////
//
// Report discrete changes to the scrollbar's value.

void scrollbarObj::implObj::updated_value_no_drag(IN_THREAD_ONLY)
{
	current_pixel=metrics.value_to_pixel(conf.value);
	dragged_value=conf.value;

	draw_slider(IN_THREAD);
	updated_value(IN_THREAD, conf.value, dragged_value);
}

LIBCXXW_NAMESPACE_END