/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_attachedto_element.H"
#include "color_picker/color_picker_impl.H"
#include "color_picker/color_picker_square_impl.H"
#include "image_button_internal.H"
#include "popup/popup.H"
#include "x/w/color_picker_config.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "catch_exceptions.H"
#include "busy.H"
#include <utility>
#include <algorithm>
#include <cmath>

LIBCXXW_NAMESPACE_START

// Update the horizontal, vertical, and fixed RGB component elements.

// The vertical horizontal strip button on the top, the vertical one on
// the left, and the bottom horizontal strip that adjusts the fixed color
// component. After moving the components between them (or for other reasons),
// rebuild their colors to reflect that.

static void update_gradients(const element &horiz_component_gradient,
			     rgb_component_t rgb::*horiz_component,
			     const element &vert_component_gradient,
			     rgb_component_t rgb::*vert_component,
			     const element &fixed_component_gradient,
			     rgb_component_t rgb::*fixed_component,
			     const rgb &current_color)
{
	rgb end_color;
	linear_gradient g;

	end_color.*horiz_component=rgb::maximum;

	g.y2=0;
	g.gradient={{0, {}}, {1, end_color}};

	horiz_component_gradient->set_background_color(g);

	end_color={};

	end_color.*vert_component=rgb::maximum;
	g.x2=0;
	g.y2=1;
	g.gradient={{0, {}}, {1, end_color}};
	vert_component_gradient->set_background_color(g);

	end_color={};

	end_color.*fixed_component=rgb::maximum;
	g.x2=1;
	g.y2=0;
	g.gradient={{0, {}}, {1, end_color}};
	fixed_component_gradient->set_background_color(g);
}

// Translate r/g/b values into h/s/v values.

std::tuple<rgb_component_t, rgb_component_t, rgb_component_t>
color_pickerObj::implObj::compute_hsv(const rgb &color)
{
	auto min=std::min({color.r, color.g, color.b});
	auto max=std::max({color.r, color.g, color.b});

	auto v=max; // Dominant color: value

	auto delta=max-min;

	if (delta == 0) // All colors at maximum, a shade of gray.
	{
		return {0, 0, v};
	}

	// Saturation measures the difference between the dominant color
	// and the minority color, the color with the smallest intensity.
	//
	// "delta" is the integer difference, between 0 and max.
	// Express the saturation in the range of 0-maximum corresponding
	// value of delta between 0 and max. In other words:
	//
	//     delta
	//   =========      scaled to the range of [0, maximum]
	//      max

	auto s=std::round(static_cast<double>(delta)/max * rgb::maximum);

	// The hue is based on the value of the "middle" color channel, that
	// is the one that's not the minimum or the maximum.
	//
	// Establish six partitions. If the red is the dominant color:
	// the difference between green and blue ranges from -delta to +delta,
	// and scale this range from 0 to 2. 0 through 1: green is smaller than
	// delta, 1 through 2: blue is smaller than green. Scale green-blue
	// into the range of 0..2

	auto h=std::round
		((color.r == max
		  ? 1.0 + (static_cast<double>(color.g)-color.b) / delta

		  // Partition 2 through 4, green is the dominant: blue to red.

		  : color.g == max
		  ? 3.0 + (static_cast<double>(color.b)-color.r) / delta

		  // Partition five through 6: blue is dominant: red to green.
		  : 5.0 + (static_cast<double>(color.r)-color.g) / delta)
		 * (rgb::maximum / 6.0));

	// s if 0 is special. Can't have that here.
	if (s == 0)
		s=1;

	if (h >= rgb::maximum)
		h=rgb::maximum;

	if (h < 0)
		h=0; // Shouldn't happen.

	return {
		static_cast<rgb_component_t>(h),
			static_cast<rgb_component_t>(s),
			v};
}

color_pickerObj::implObj::implObj(const popup_attachedto_element_impl &impl,
				  const image_button_internal &popup_button,
				  const element &current_color_element,

				  const color_picker_config &config,
				  const official_color &initial_color,
				  const element &horiz_component_gradient,
				  const element &vert_component_gradient,
				  const element &fixed_component_gradient,
				  const color_picker_square &variable_gradients,
				  const label &error_message_field)
	: impl{impl}, popup_button{popup_button},
	  current_color_element{current_color_element},

	  horiz_component_gradient{horiz_component_gradient},
	  vert_component_gradient{vert_component_gradient},
	  fixed_component_gradient{fixed_component_gradient},
	  variable_gradients{variable_gradients},
	  error_message_field{error_message_field},
	  callback_thread_only{config.initial_callback},
	  current_color_thread_only{config.initial_color},
	  current_official_color{initial_color}
{
	update_gradients(horiz_component_gradient,
			 initial_horiz_component,
			 vert_component_gradient,
			 initial_vert_component,
			 fixed_component_gradient,
			 initial_fixed_component,
			 config.initial_color);

}

color_pickerObj::implObj::~implObj()
{
}

void color_pickerObj::implObj::update_hv_components(ONLY IN_THREAD,
						    rgb_component_t h,
						    rgb_component_t v,
						    const callback_trigger_t
						    &trigger)
{
	auto c=current_color(IN_THREAD);

	c.*horiz_component(IN_THREAD)=h;
	c.*vert_component(IN_THREAD)=v;

	set_color(IN_THREAD, c);
}

void color_pickerObj::implObj::set_color(ONLY IN_THREAD,
					 const rgb &c)
{
	current_color(IN_THREAD)=c;

	r_value->set(c.r);
	g_value->set(c.g);
	b_value->set(c.b);
	new_rgb_values(IN_THREAD);
}

void color_pickerObj::implObj::update_fixed_component(ONLY IN_THREAD,
						      rgb_component_t v,
						      const callback_trigger_t
						      &trigger)
{
	auto &c=current_color(IN_THREAD);

	c.*fixed_component(IN_THREAD)=v;

	r_value->set(c.r);
	g_value->set(c.g);
	b_value->set(c.b);
	new_rgb_values(IN_THREAD);
}

void color_pickerObj::implObj::new_color(ONLY IN_THREAD,
					 const callback_trigger_t &trigger)
{
	current_color_element->set_background_color(current_color(IN_THREAD));
}

void color_pickerObj::implObj::swap_horiz_gradient(ONLY IN_THREAD)
{
	std::swap(horiz_component(IN_THREAD), fixed_component(IN_THREAD));
	refresh_component_gradients(IN_THREAD);
	refresh_variable_gradients(IN_THREAD);
}

void color_pickerObj::implObj::swap_vert_gradient(ONLY IN_THREAD)
{
	std::swap(vert_component(IN_THREAD), fixed_component(IN_THREAD));
	refresh_component_gradients(IN_THREAD);
	refresh_variable_gradients(IN_THREAD);
}

void color_pickerObj::implObj::refresh_component_gradients(ONLY IN_THREAD)
{
	update_gradients(horiz_component_gradient,
			 horiz_component(IN_THREAD),
			 vert_component_gradient,
			 vert_component(IN_THREAD),
			 fixed_component_gradient,
			 fixed_component(IN_THREAD),
			 current_color(IN_THREAD));
}

void color_pickerObj::implObj::refresh_variable_gradients(ONLY IN_THREAD)
{
	variable_gradients->impl->update(IN_THREAD,
					 current_color(IN_THREAD),
					 horiz_component(IN_THREAD),
					 vert_component(IN_THREAD));
}

void color_pickerObj::implObj::new_rgb_values(ONLY IN_THREAD)
{
	auto r=r_value->validated_value.get();
	auto g=g_value->validated_value.get();
	auto b=b_value->validated_value.get();

	if (!r || !g || !b)
		return; // Just in case, but shouldn't happen

	// Update the gradient strips and gradient squares
	auto &c=current_color(IN_THREAD);

	c.r=*r;
	c.g=*g;
	c.b=*b;

	refresh_component_gradients(IN_THREAD);
	refresh_variable_gradients(IN_THREAD);
	{
		auto [h, s, v]=compute_hsv(c);

		h_value->set(h);
		s_value->set(s);
		v_value->set(v);
	}

	//! New official color
	new_color(IN_THREAD, {});
}

void color_pickerObj::implObj::new_hsv_values(ONLY IN_THREAD)
{
	auto h=h_value->validated_value.get();
	auto s=s_value->validated_value.get();
	auto v=v_value->validated_value.get();

	if (!h || !s || !v)
		return;

	// Compute R/G/B values from the current H/S/V values.

	auto c=current_color(IN_THREAD);

	if (s == 0)
	{
		// Gray scale.

		c.r=*v;
		c.g=*v;
		c.b=*v;

		// Reset h to 0, to make things consistent.

		if (h != 0)
			h_value->set(0);
	}
	else
	{
		// Multiply the value by saturation to get the
		// delta.
		//
		rgb_component_t delta=
			static_cast<rgb_component_t>
			(std::round(*v * (*s /
					  (rgb::maximum + 0.0)))
			 );

		rgb_component_t min=*v-delta;

		// The hue range of [0, rgb::maximum]
		// corresponds to six partitions. Scale
		// [0, rgb::maximum] into the range [0, 6),
		// and get the integer and the fractional
		// amount. The integral part indicate the
		//partition number.

		double fi;
		double f=std::modf(*h / ((rgb::maximum + 1.0)
					 / 6.0),
				   &fi);
		unsigned i=static_cast<unsigned>(fi);

		// The fractional component derives the value
		// of the channel between the minimal component
		// and the dominant component.

		rgb_component_t diff=
			static_cast<rgb_component_t>
			(std::round(f * delta));

		// Depending on the quadrant, a fraction of 0
		// could mean either the minimal component end,
		// or the dominant component end. Compute
		// either way.

		rgb_component_t min_to_max = min + diff;
		rgb_component_t max_to_min = *v - diff;

		// From 0 to 2, red is the dominant color.
		//
		// 0 to 1 green is smaller than blue.
		// 1 to 2 blue is smaller than green.
		//
		// From 2 to 4, green is the dominant color
		//
		// 2 to 3 blue is smaller than red
		// 3 to 4 red is smaller than blue
		//
		// From 5 to 6, blue is the dominant color
		//
		// 4 to 5 red is smaller than green
		// 5 to 6 green is smaller than red

		switch (i) {
		case 0:
			c.r = *v;
			c.g = min;
			c.b = max_to_min;
			break;
		case 1:
			c.r = *v;
			c.g = min_to_max;
			c.b = min;
			break;
		case 2:
			c.r = max_to_min;
			c.g = *v;
			c.b = min;
			break;

		case 3:
			c.r = min;
			c.g = *v;
			c.b = min_to_max;
			break;
		case 4:
			c.r = min;
			c.g = max_to_min;
			c.b = *v;
			break;
		case 5:
			c.r = min_to_max;
			c.g = min;
			c.b = *v;
			break;
		}
	}

	r_value->set(c.r);
	g_value->set(c.g);
	b_value->set(c.b);
	current_color(IN_THREAD)=c;
	refresh_component_gradients(IN_THREAD);
	refresh_variable_gradients(IN_THREAD);
	new_color(IN_THREAD, {});
}

void color_pickerObj::implObj::reformat_values(ONLY IN_THREAD)
{
	// Just tickle the validators. They know what to do.

	r_value->set(r_value->validated_value.get());
	g_value->set(g_value->validated_value.get());
	b_value->set(b_value->validated_value.get());

	h_value->set(h_value->validated_value.get());
	s_value->set(s_value->validated_value.get());
	v_value->set(v_value->validated_value.get());
}


void color_pickerObj::implObj::set_official_color(ONLY IN_THREAD)
{
	{
		mpobj<rgb>::lock lock{current_official_color->official_color};

		if (*lock == current_color(IN_THREAD))
			return; // Not really.

		*lock=current_color(IN_THREAD);
	}

	official_color_updated(IN_THREAD, {});
}

void color_pickerObj::implObj::popup_closed(ONLY IN_THREAD)
{
	auto &c=current_color(IN_THREAD);

	{
		mpobj<rgb>::lock lock{current_official_color->official_color};

		if (*lock == current_color(IN_THREAD))
			return;

		// Restore the current_color from the official_color

		c=*lock;
	}

	r_value->set(c.r);
	g_value->set(c.g);
	b_value->set(c.b);
	new_rgb_values(IN_THREAD);
}

void color_pickerObj::implObj
::official_color_updated(ONLY IN_THREAD,
			 const callback_trigger_t &trigger)
{
	if (!callback(IN_THREAD))
		return;

	auto c=current_official_color->official_color.get();

	auto e_impl=current_color_element->impl;

	try {
		callback(IN_THREAD)(IN_THREAD,
				    c,
				    trigger,
				    busy_impl{*e_impl});
	} REPORT_EXCEPTIONS(e_impl);
}

LIBCXXW_NAMESPACE_END
