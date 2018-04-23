/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_impl.H"
#include "color_picker/color_picker_square_impl.H"
#include "image_button_internal.H"
#include "popup/popup.H"
#include "x/w/color_picker_config.H"
#include "catch_exceptions.H"
#include "busy.H"
#include <utility>

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


color_pickerObj::implObj::implObj(const ref<handlerObj> &handler,
				  const image_button_internal &popup_button,
				  const popup &color_picker_popup,
				  const element &current_color_element,

				  const color_picker_config &config,
				  const element &horiz_component_gradient,
				  const element &vert_component_gradient,
				  const element &fixed_component_gradient,
				  const color_picker_square &variable_gradients)
	: handler{handler}, popup_button{popup_button},
	  color_picker_popup{color_picker_popup},
	  current_color_element{current_color_element},

	  horiz_component_gradient{horiz_component_gradient},
	  vert_component_gradient{vert_component_gradient},
	  fixed_component_gradient{fixed_component_gradient},
	  variable_gradients{variable_gradients},
	  callback_thread_only{config.initial_callback},
	  current_color{config.initial_color}
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
	{
		mpobj<rgb>::lock lock{current_color};

		(*lock).*horiz_component(IN_THREAD)=h;
		(*lock).*vert_component(IN_THREAD)=v;
	}

	new_color(IN_THREAD, trigger);
	refresh_variable_gradients(IN_THREAD);
}

void color_pickerObj::implObj::update_fixed_component(ONLY IN_THREAD,
						      rgb_component_t v,
						      const callback_trigger_t
						      &trigger)
{
	{
		mpobj<rgb>::lock lock{current_color};

		(*lock).*fixed_component(IN_THREAD)=v;
	}

	new_color(IN_THREAD, trigger);
}

void color_pickerObj::implObj::new_color(ONLY IN_THREAD,
					 const callback_trigger_t &trigger)
{
	auto c=current_color.get();

	current_color_element->set_background_color(c);

	if (callback(IN_THREAD))
	{
		auto e_impl=current_color_element->impl;

		try {
			callback(IN_THREAD)(IN_THREAD,
					    c,
					    trigger,
					    busy_impl{*e_impl});
		} REPORT_EXCEPTIONS(e_impl);
	}
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
			 current_color.get());
}

void color_pickerObj::implObj::refresh_variable_gradients(ONLY IN_THREAD)
{
	variable_gradients->impl->update(IN_THREAD,
					 current_color.get(),
					 horiz_component(IN_THREAD),
					 vert_component(IN_THREAD));
}
LIBCXXW_NAMESPACE_END
