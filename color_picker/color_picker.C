/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/color_picker_config.H"
#include "x/w/canvas.H"
#include "x/w/button.H"
#include "x/w/button_event.H"
#include "color_picker/color_picker_impl.H"
#include "color_picker/color_picker_handler.H"
#include "color_picker/color_picker_selector_impl.H"
#include "color_picker/color_picker_square_impl.H"
#include "popup/popup_attachedto_info.H"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_impl.H"
#include "peephole/peephole_toplevel.H"
#include "peephole/peepholed_fontelement.H"
#include "peephole/peepholed_toplevel_element.H"
#include "popup_imagebutton.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/focus/focusable.H"
#include "reference_font_element.H"
#include "gridlayoutmanager.H"
#include "messages.H"

#include <x/weakcapture.H>
#include <cmath>

LIBCXXW_NAMESPACE_START

color_pickerObj::color_pickerObj(const ref<implObj> &impl,
				 const layout_impl &container_layoutmanager)
	: focusable_containerObj{impl->handler, container_layoutmanager},
	  impl{impl}
{
}

color_pickerObj::~color_pickerObj()=default;

focusable_impl color_pickerObj::get_impl() const
{
	return impl->popup_button->get_impl();
}

///////////////////////////////////////////////////////////////////////////

color_picker factoryObj::create_color_picker()
{
	return create_color_picker({});
}

//////////////////////////////////////////////////////////////////////////
//
// The left half of the color picker selector popup.
//
// The layout looks like this
//
// +---+-------------------------------+
// |   | HHHHHHHHHHHHHHHHHHHHHHHHHHHHHH|
// +---+-------------------------------+
// |                                   |
// +-+-+-------------------------------+
// |V| | SSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
// |V| | SSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
// |V| | SSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
// |V| | SSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
//
//   .....
// |V| | SSSSSSSSSSSSSSSSSSSSSSSSSSSSSS|
// +-+-+--------------------------------
// |                                   |
// +---+-------------------------------+
// |   | CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC|
// +---+-------------------------------+
//
// H - the button that swaps the horizontal component with the third component.
//
// V - the button that swaps the vertical component with the third component
//
// S - the color_picker_square that displays the gradient of the horizontal
// and vertical components, with a fixed third component
//
// C - display-only strip that shows the third component's gradient.
//
// An explicit row and column provides a buffer between the color_picker_square
// and the elements around it, for visual separation, and so that we can use
// the grid layout manager's borders for it.

typedef std::tuple<button, canvas, button, canvas,
		   canvas, element> left_side_contents_t;

static inline left_side_contents_t
create_left_side_contents(const gridlayoutmanager &glm,
			  const color_picker_config &config)
{
	dim_arg bw{"color_picker_buffer_width"};
	dim_arg bh{"color_picker_buffer_height"};

	dim_arg sw{"color_picker_strip_width"};
	dim_arg sh{"color_picker_strip_height"};

	// Row 0
	auto f=glm->append_row();

	f->colspan(2).create_canvas();

	// HHHHH

	f->padding(0);
	canvasptr h_canvas;
	auto h_button=f->create_normal_button
		([&]
		 (const auto &button_factory)
		 {
			 h_canvas=button_factory->create_canvas
				([]
				 (const auto &){},
				 {},
				 {sh});
		 });

	// Row 1

	f=glm->append_row();
	f->colspan(3).create_canvas([](const auto &) {},
					   {},
					   {bh});

	// Row 2
	f=glm->append_row();

	// VVVVV

	canvasptr v_canvas;

	f->padding(0);
	auto v_button=f->create_normal_button
		([&]
		 (const auto &button_factory)
		 {
			 v_canvas=button_factory->create_canvas
				([](const auto &) {},
		                 {sw},
		                 {});
		 });

	f->create_canvas([](const auto &) {},
			       {bw},
			       {});

	// SSSSS

	f->padding(0).border("color_picker_gradient_border");

	auto parent_container=f->get_container_impl();

	rgb initial_fixed_color;

	initial_fixed_color.*(color_pickerObj::
			      implObj::
			      initial_fixed_component
			      )=
		config.initial_color.*(color_pickerObj::
				       implObj::
				       initial_fixed_component
				       );

	auto impl=ref<color_picker_squareObj::implObj>
		::create(parent_container,
			 // Initial channel color,
			 initial_fixed_color,
			 // Vary the red and the green channels.
			 color_pickerObj::implObj::initial_horiz_component,
			 color_pickerObj::implObj::initial_vert_component,
			 canvas_init_params{
				 {"color_picker_square_width"},
				 {"color_picker_square_height"},
					 "color_picker_square@libcxx.com"});

	auto square=color_picker_square::create(impl);

	f->created_internally(square);

	// Row 3

	f=glm->append_row();
	f->colspan(3).create_canvas([](const auto &) {},
					   {},
					   {bh});
	// Row 4
	f=glm->append_row();
	f->colspan(2).create_canvas();

	// CCCCCC

	f->padding(0).border("color_picker_fixed_component_border");
	auto fixed_canvas=f->create_canvas([] (const auto &){},
					   {},
					   {sh});

	// Tooltips
	text_param button_tooltip{_("Select this color channel for adjustment")};

	h_button->create_tooltip(button_tooltip);
	v_button->create_tooltip(button_tooltip);
	fixed_canvas->create_tooltip(_("Click to adjust this color channel in the currently selected color"));
	square->create_tooltip(_("Pick a color"));
	return {h_button, h_canvas,
			v_button, v_canvas,
			fixed_canvas,
			square
			};

}

// Create the contents of the color picker popup. Factored out for readability.
//
// The top level popup implementation/handler container parent gets passed in,
// this creates a color_picker_selectorObj as its child, with a
// color_picker_selector as the element.

static inline color_picker_selector
create_contents(const popup_attachedto_info &attachedto_info,
		const color_picker_config &config,
		const container_impl &parent,

		// We initialize this

		std::optional<left_side_contents_t> &left_side_contents)
{
	child_element_init_params init_params;

	init_params.background_color=
		config.popup_background_color;

	auto container_impl=
		ref<color_picker_selectorObj::implObj>::create(parent,
							       init_params);

	auto glm_impl=ref<gridlayoutmanagerObj::implObj>
		::create(container_impl);

	auto glm=glm_impl->create_gridlayoutmanager();

	auto f=glm->append_row();

	f->padding(0);

	f->create_container
		([&]
		 (const auto &left_side)
		 {
			 left_side_contents=create_left_side_contents
				 (left_side->get_layoutmanager(), config);
		 },
		 new_gridlayoutmanager{});

	auto p=color_picker_selector::create(attachedto_info,
					     container_impl,
					     glm_impl);

	return p;
}

// Clicked in an element. Translate the click's position into 0..rgb::maximum

static rgb_component_t compute_from_click(const element &e,
					  coord_t x,
					  dim_t s)
{
	auto xc=(coord_t::value_type)x;
	auto sc=coord_t::truncate(s);

	if (xc < 0)
		return 0;

	if (xc >= sc)
		return rgb::maximum;

	return std::round(xc * 1.0 / sc * rgb::maximum);
}

color_picker factoryObj
::create_color_picker(const color_picker_config &config)
{
	auto parent_container=get_container_impl();

	auto attachedto_info=
		popup_attachedto_info::create(rectangle{},
					      attached_to::submenu_next);

	// Our container implementation object, for the current color field and
	// the popup button.
	auto handler=ref<color_pickerObj::handlerObj>
		::create(parent_container);

	// We will use the grid layout manager, of course.

	auto glm_impl=ref<gridlayoutmanagerObj::implObj>::create(handler);

	auto glm=glm_impl->create_gridlayoutmanager();

	// Create the current color picker.

	// We use the grid layout manager to draw the border around it
	// (config.border)...
	auto f=glm->append_row();
	f->padding(0);
	f->border(config.border);

	// The current color element

	dim_arg w{"color_picker_current_width"};
	dim_arg h{"color_picker_current_height"};

	auto color_picker_current=f->create_canvas
		([&](const auto &c)
		 {
			 c->set_background_color(config.initial_color);
		 },
		 {w, w, w},
		 {h, h, h});

	color_picker_current->show();

	// Before creating the button that pops up the color picker,
	// we need to create the popup itself.

	auto parent_handler=ref(&parent_container->get_window_handler());

	auto attachedto_handler=
		ref<popup_attachedto_handlerObj>::create
		(popup_attachedto_handler_args{
			&shared_handler_dataObj::opening_exclusive_popup,
			&shared_handler_dataObj::closing_exclusive_popup,
			"color_picker",
			parent_handler,
			attachedto_info,
			parent_container->container_element_impl()
				.nesting_level+2});

	attachedto_handler->set_window_type("popup_menu,dropdown_menu");

	auto popup_impl=ref<popupObj::implObj>::create(attachedto_handler,
						       parent_handler);

	peephole_style popup_peephole_style{halign::fill};

	color_picker_selectorptr selector;

	std::optional<left_side_contents_t> left_side_contents;

	// Create the contents of the popup.

	auto popup_lm=create_peephole_toplevel
		(attachedto_handler,
		 config.popup_border,
		 config.popup_background_color,
		 config.popup_background_color,
		 popup_peephole_style,
		 [&]
		 (const container_impl &parent)
		 {
			 auto p=create_contents(attachedto_info,
						config,
						parent,
						left_side_contents);
			 selector=p;
			 p->show_all();
			 return p;
		 });

	// Grab the elements that were created in the popup, and finish
	// creating it.

	auto &[h_button, h_canvas, v_button, v_canvas, fixed_canvas, square]=
		left_side_contents.value();

	auto color_picker_popup=popup::create(popup_impl, popup_lm->impl);

	// We can now create the popup button next to the current color
	// element.

	auto popup_imagebutton=create_standard_popup_imagebutton
		(f, attachedto_handler,
		 color_picker_popup->elementObj::impl,
		 {
			 config.border,
				 config.background_color,
				 "scroll-right1",
				 "scroll-right2",
				 config.focusoff_border,
				 config.focuson_border
		 });

	// Capture the elements in the implementation object.

	auto impl=ref<color_pickerObj::implObj>
		::create(handler, popup_imagebutton, color_picker_popup,
			 color_picker_current,

			 config,
			 h_canvas, v_canvas, fixed_canvas,
			 square);

	// Clicking on the square gradient.

	square->on_button_event
		([get=make_weak_capture(impl, square)]
		 (ONLY IN_THREAD,
		  const auto &be,
		  bool activated,
		  const auto &ignore)
		 {
			 auto got=get.get();

			 if (!got)
				 return false;

			 auto &[color_picker_impl, square]=*got;

			 if (!activated || be.button != 1)
				 return false;

			 // Compute the new component values.

			 auto &data=square->impl->data(IN_THREAD);
			 auto h=compute_from_click
				 (square,
				  data.last_motion_x,
				  data.current_position.width);

			 auto v=compute_from_click
				 (square,
				  data.last_motion_y,
				  data.current_position.height);

			 // And update the color.

			 color_picker_impl->update_hv_components
				 (IN_THREAD, h, v, &be);

			 return true;
		 });

	// Clicking on the gradient strips

	h_button->on_activate
		([get=make_weak_capture(impl)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &ignore)
		 {
			 auto got=get.get();

			 if (!got)
				 return;

			 auto &[impl]=*got;

			 impl->swap_horiz_gradient(IN_THREAD);
		 });

	v_button->on_activate
		([get=make_weak_capture(impl)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &ignore)
		 {
			 auto got=get.get();

			 if (!got)
				 return;

			 auto &[impl]=*got;

			 impl->swap_vert_gradient(IN_THREAD);
		 });

	fixed_canvas->on_button_event
		([get=make_weak_capture(impl, fixed_canvas)]
		 (ONLY IN_THREAD,
		  const auto &be,
		  bool activate_for,
		  const auto &ignore)
		 {
			 auto got=get.get();

			 if (!got)
				 return false;

			 auto &[color_picker_impl, square]=*got;

			 if (be.press != activate_for || be.button != 1)
				 return false;

			 auto &data=square->impl->data(IN_THREAD);
			 auto v=compute_from_click
				 (square,
				  data.last_motion_x,
				  data.current_position.width);

			 color_picker_impl->update_fixed_component
				 (IN_THREAD, v, &be);

			 return true;
		 });

	auto p=color_picker::create(impl, glm_impl);

	created(p);
	return p;
}

void color_pickerObj::on_color_update(const functionref<color_picker_callback_t>
				      &cb)
{
	in_thread([impl=this->impl, cb]
		  (ONLY IN_THREAD)
		  {
			  // Install the callback, and invoke it initially.

			  impl->callback(IN_THREAD)=cb;
			  impl->new_color(IN_THREAD, initial{});
		  });
}

rgb color_pickerObj::current_color() const
{
	return impl->current_color.get();
}

LIBCXXW_NAMESPACE_END
