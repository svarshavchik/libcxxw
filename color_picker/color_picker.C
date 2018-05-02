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
#include "x/w/label.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/button.H"
#include "reference_font_element.H"
#include "gridlayoutmanager.H"
#include "messages.H"

#include <x/chrcasecmp.H>
#include <x/weakcapture.H>
#include <x/weakptr.H>
#include <cmath>
#include <limits>
#include <sstream>
#include <iomanip>
#include <charconv>
#include <algorithm>

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

/////////////////////////////////////////////////////////////////////////////
//
// The right half of the popup, with the input fields for manually entering
// RGB and HSV values, and for selecting full precision and hexadecimal.

typedef std::tuple<input_field, input_field, input_field,
		   input_field, input_field, input_field,
		   image_button, image_button> right_side_contents_t;

static inline right_side_contents_t
create_right_side_contents(const gridlayoutmanager &glm,
			   const color_picker_config &config)
{
	// Two columns. The first column has the input field label, the
	// second column the input fields themselves. Right-align the
	// first column, to make the labels line up with their input fields.

	glm->col_alignment(0, halign::right);

	// Number of decimal digits required to represent all values of an
	// rgb_component_t.
	constexpr auto digits=std::numeric_limits<rgb_component_t>::digits10+1;

	// The input field has one more position, for the trailing cursor.
	input_field_config conf{digits+1};

	conf.alignment=halign::right;
	conf.maximum_size=digits;
	conf.autoselect=true;
	conf.set_default_spin_control_factories();

	// Meticulously create the six input fields.
	auto f=glm->append_row();

	f->create_label(TAG(_("RED:R:")));

	auto r=f->create_input_field("", conf);

	f=glm->append_row();

	f->create_label(TAG(_("GREEN:G:")));

	auto g=f->create_input_field("", conf);

	f=glm->append_row();

	f->create_label(TAG(_("BLUE:B:")));

	auto b=f->create_input_field("", conf);

	f=glm->append_row();

	f->create_label(TAG(_("HUE:H:")));

	auto h=f->create_input_field("", conf);

	f=glm->append_row();

	f->create_label(TAG(_("SATURATION:S:")));

	auto s=f->create_input_field("", conf);

	f=glm->append_row();

	f->create_label(TAG(_("VALUE:V:")));

	auto v=f->create_input_field("", conf);

	// It would be rather rude for the first one of them to grab the
	// input focus automatically.

	r->autofocus(false);
	g->autofocus(false);
	b->autofocus(false);
	h->autofocus(false);
	s->autofocus(false);
	v->autofocus(false);

	// The two option checkboxes are below them. Each one is on its
	// own row, and each one spans both columns.

	f=glm->append_row();

	auto hexadecimal=f->colspan(2).halign(halign::left).create_checkbox
		([]
		 (const auto &f)
		 {
			 f->create_label("Hexadecimal");
		 });

	f=glm->append_row();

	auto full_precision=f->colspan(2).halign(halign::left).create_checkbox
		([]
		 (const auto &f)
		 {
			 f->create_label("Full precision");
		 });

	return {r, g, b, h, s, v, hexadecimal, full_precision};
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

		std::optional<left_side_contents_t> &left_side_contents,
		std::optional<right_side_contents_t> &right_side_contents,
		labelptr &error_message_field,
		buttonptr &cancel,
		buttonptr &ok)
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

	glm->row_alignment(0, valign::middle);

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

	f->left_padding("color_picker_buffer_width");

	f->create_container
		([&]
		 (const auto &right_side)
		 {
			 right_side_contents=create_right_side_contents
				 (right_side->get_layoutmanager(), config);
		 },
		 new_gridlayoutmanager{});

	auto p=color_picker_selector::create(attachedto_info,
					     container_impl,
					     glm_impl);

	// A label for an error message, on the last row.

	f=glm->append_row();

	f->colspan(2);
	f->halign(halign::center);

	error_message_field=f->create_label(" ");

	// Ok and Cancel buttons on the last row. Need to create a separate
	// container for them, for alignment purposes.

	f=glm->append_row();
	f->colspan(2);
	f->halign(halign::right);
	f->padding(0);
	f->create_container
		([&]
		 (const auto &c)
		 {
			 gridlayoutmanager glm=c->get_layoutmanager();

			 auto f=glm->append_row();

			 cancel=f->create_normal_button_with_label
				 (_("Cancel"));

			 ok=f->create_special_button_with_label
				 (_("Ok"));
		 },
		 new_gridlayoutmanager{});
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

namespace {
#if 0
}
#endif

// A bunch of callbacks need to weakly-capture the implementation object.
// We'll do it just once.

class weak_implObj : virtual public obj {

public:

	weakptr<ptr<color_pickerObj::implObj>> impl;

	weak_implObj(const ref<color_pickerObj::implObj> &impl)
		: impl{impl}
	{
	};

	inline auto get() const { return impl.getptr(); }
};

#if 0
{
#endif
}

typedef ref<weak_implObj> weak_impl;

// Internally the channel value is always stored in full precision. But
// by default we show only the most significant byte. This is purely a
// display-only conversion. Helper function for extracting the high byte
// from the value for display puroses...
static constexpr uint8_t get_rgb_high_byte(rgb_component_t c)
{
	return static_cast<uint8_t>
		(c >> ((sizeof(rgb_component_t)-1)*8));
}

// ... and for taking the entered value and scaling it to full precision.
//
// Note we don't simply set the low byte to zeroes. If a maximum value is
// entered we expected to get rgb::maximum. Instead, whatever byte is
// entered, we double it.

static constexpr rgb_component_t rgb_from_high_byte(uint8_t v)
{
	rgb_component_t c=v;

	size_t n=sizeof(rgb_component_t)-1;

	while (n)
	{
		c=(c << 8) | v;
		--n;
	}
	return c;
}

// Build an input field validator for one of the manual entry fields.

static auto
make_manual_input_validator(const input_field &f,
			    const weak_impl &wimpl,
			    void (color_pickerObj::implObj::*n)(ONLY IN_THREAD))
{
	auto validator=f->set_validator
		([wimpl]
		 (ONLY IN_THREAD,
		  std::string s,
		  const input_field &f,
		  const auto &ignore) -> std::optional<rgb_component_t>
		 {
			 auto impl=wimpl->get();

			 if (!impl)
				 return std::nullopt;

			 // Convert whatever's entered to lowercase, then
			 // compute the entered value at full precision,
			 // based on the currently-set options.

			 std::transform(s.begin(), s.end(),
					s.begin(),
					chrcasecmp::tolower);

			 rgb_component_t value{};
			 uint8_t value_c;

			 auto c=s.c_str();

			 bool hexadecimal=impl->hexadecimal.get();
			 bool full_precision=impl->full_precision.get();

			 auto ret=full_precision ?
				 std::from_chars(c, c+s.size(), value,
						 hexadecimal ? 16:10)
				 :
				 std::from_chars(c, c+s.size(), value_c,
						 hexadecimal ? 16:10);

			 // Was a valid value entered?

			 if (ret.ec != std::errc{} || *ret.ptr)
			 {
				 std::ostringstream min, max;

				 // A lot of work to create an informative
				 // error message.
				 if (hexadecimal)
				 {
					 min << std::hex << std::uppercase
					     << std::setw((full_precision ?
							   sizeof(value):1)*2)
					     << std::setfill('0');
					 max << std::hex << std::uppercase
					     << std::setw((full_precision ?
							   sizeof(value):1)*2)
					     << std::setfill('0');
				 }
				 min << 0;
				 max << (full_precision ? rgb::maximum
					 : get_rgb_high_byte(rgb::maximum));

				 impl->error_message_field->update
					 (gettextmsg(_("Entered value must be"
						       " between %1% and %2%"),
						     min.str(), max.str()));
				 return std::nullopt;
			 }

			 if (!full_precision)
				 value=rgb_from_high_byte(value_c);

			 //! Clear any error message
			 impl->error_message_field->update(" ");
			 return value;
		 },
		 [wimpl]
		 (rgb_component_t v) -> std::string
		 {
			 // Format currently value for display.

			 constexpr auto digits=std::numeric_limits<
				 rgb_component_t>::digits10+1;

			 bool full_precision=false;
			 bool hexadecimal=false;

			 auto impl=wimpl->get();

			 if (impl)
			 {
				 full_precision=impl->full_precision.get();
				 hexadecimal=impl->hexadecimal.get();
			 }

			 char buffer[digits+1];

			 if (!full_precision)
				 v = get_rgb_high_byte(v);

			 auto ret=std::to_chars(buffer, buffer+digits,
						v,
						hexadecimal ? 16:10);

			 if (ret.ec != std::errc{})
				 throw EXCEPTION("Unexpected to_chars() "
						 "failure");

			 *ret.ptr=0;

			 if (hexadecimal) // Pad hexadecimal values
			 {
				 std::transform(buffer, ret.ptr,
						buffer,
						chrcasecmp::toupper);
				 std::ostringstream o;

				 o << std::setw((full_precision ? sizeof(v):1)
						*2)
				   << std::setfill('0')
				   << buffer;

				 return o.str();
			 }

			 return buffer;
		 },
		 [wimpl, n]
		 (ONLY IN_THREAD, const std::optional<rgb_component_t> &v)
		 {
			 // Setting a new R/G/B value recomputes the H/S/V
			 // values.
			 //
			 // Setting a new H/S/V value recomputes the R/G/B
			 // values.
			 //
			 // In either case we may need to redraw the gradient
			 // square and the third channel strip value, and set
			 // the currently entered color.
			 if (!v)
				 return;

			 auto impl=wimpl->get();

			 if (!impl)
				 return;

			 // Invoke new_rgb_values() or new_hsv_values()

			 ((*impl).*n)(IN_THREAD);
		 });

	// Make the input field's spin buttons do something useful.
	//
	// No matter what the currently displayed options are, the net effect
	// of a spin button is to adjust the color value's most significant
	// byte.

	f->on_spin([wimpl, n, validator]
		   (ONLY IN_THREAD,
		    const auto &trigger,
		    const auto &busy)
		   {
			   auto impl=wimpl->get();

			   if (!impl)
				   return;

			   auto current_value=validator->validated_value.get();

			   if (!current_value)
				   return;

			   auto high_byte=get_rgb_high_byte(*current_value);

			   if (high_byte)
				   --high_byte;

			   validator->set(rgb_from_high_byte(high_byte));

			   // Invoke new_rgb_values() or new_hsv_values()

			   ((*impl).*n)(IN_THREAD);
		   },
		   [wimpl, n, validator]
		   (ONLY IN_THREAD,
		    const auto &trigger,
		    const auto &busy)
		   {
			   auto impl=wimpl->get();

			   if (!impl)
				   return;

			   auto current_value=validator->validated_value.get();

			   if (!current_value)
				   return;

			   auto high_byte=get_rgb_high_byte(*current_value);

			   if (++high_byte == 0)
				   --high_byte;

			   validator->set(rgb_from_high_byte(high_byte));

			   // Invoke new_rgb_values() or new_hsv_values()

			   ((*impl).*n)(IN_THREAD);
		   });

	return validator;
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
	std::optional<right_side_contents_t> right_side_contents;
	labelptr error_message_field;
	buttonptr cancel;
	buttonptr ok;

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
						left_side_contents,
						right_side_contents,
						error_message_field,
						cancel, ok);
			 selector=p;
			 p->show_all();
			 return p;
		 });

	// Grab the elements that were created in the popup, and finish
	// creating it.

	auto &[h_button, h_canvas, v_button, v_canvas, fixed_canvas, square]=
		left_side_contents.value();

	auto &[r_input_field, g_input_field, b_input_field,
	       h_input_field, s_input_field, v_input_field,
	       hexadecimal, full_precision]=
		right_side_contents.value();

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
			 square, error_message_field);

	auto wimpl=weak_impl::create(impl);

	// Install validators for manual R, G, and B fields.
	impl->r_value=make_manual_input_validator
		(r_input_field, wimpl,
		 &color_pickerObj::implObj::new_rgb_values);
	impl->g_value=make_manual_input_validator
		(g_input_field, wimpl,
		 &color_pickerObj::implObj::new_rgb_values);
	impl->b_value=make_manual_input_validator
		(b_input_field, wimpl,
		 &color_pickerObj::implObj::new_rgb_values);

	// And set their initial values
	impl->r_value->set(config.initial_color.r);
	impl->g_value->set(config.initial_color.g);
	impl->b_value->set(config.initial_color.b);

	// Install validators for manual H, S, and V fields.

	impl->h_value=make_manual_input_validator
		(h_input_field, wimpl,
		 &color_pickerObj::implObj::new_hsv_values);
	impl->s_value=make_manual_input_validator
		(s_input_field, wimpl,
		 &color_pickerObj::implObj::new_hsv_values);
	impl->v_value=make_manual_input_validator
		(v_input_field, wimpl,
		 &color_pickerObj::implObj::new_hsv_values);

	// And set their initial values.

	{
		auto [h, s, v]=color_pickerObj::implObj
			::compute_hsv(config.initial_color);

		impl->h_value->set(h);
		impl->s_value->set(s);
		impl->v_value->set(v);
	}

	hexadecimal->set_value(impl->hexadecimal.get() ? 1:0);
	full_precision->set_value(impl->full_precision.get() ? 1:0);

	// Invoke reformat_values() when the display options change.

	hexadecimal->on_activate
		([wimpl]
		 (ONLY IN_THREAD,
		  size_t value,
		  const callback_trigger_t &trigger,
		  const busy &mcguffin)
		 {
			 if (std::holds_alternative<initial>(trigger))
				 return; // Ignore initial callback.

			 auto impl=wimpl->get();

			 if (impl)
			 {
				 impl->hexadecimal=value > 0;
				 impl->reformat_values(IN_THREAD);
			 }
		 });

	full_precision->on_activate
		([wimpl]
		 (ONLY IN_THREAD,
		  size_t value,
		  const callback_trigger_t &trigger,
		  const busy &mcguffin)
		 {
			 if (std::holds_alternative<initial>(trigger))
				 return; // Ignore initial callback.
			 auto impl=wimpl->get();

			 if (impl)
			 {
				 impl->full_precision=value > 0;
				 impl->reformat_values(IN_THREAD);
			 }
		 });

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
		([wimpl]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &ignore)
		 {
			 auto impl=wimpl->get();

			 if (!impl)
				 return;

			 impl->swap_horiz_gradient(IN_THREAD);
		 });

	v_button->on_activate
		([wimpl]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &ignore)
		 {
			 auto impl=wimpl->get();

			 if (!impl)
				 return;

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

	///////////////////////////////////////////////////////////////////

	color_picker_popup->on_state_update
		([wimpl]
		 (ONLY IN_THREAD,
		  const element_state &new_state,
		  const busy &mcguffin)
		 {
			 if (new_state.state_update !=
			     new_state.before_hiding)
				 return;

			 auto impl=wimpl->get();

			 if (impl)
				 impl->popup_closed(IN_THREAD);
		 });

	cancel->on_activate
		([popup=make_weak_capture(color_picker_popup)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto got=popup.get();

			 if (!got)
				 return;

			 auto &[popup]=*got;

			 popup->hide();
		 });

	ok->on_activate
		([wimpl, popup=make_weak_capture(color_picker_popup)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto got=popup.get();

			 if (!got)
				 return;

			 auto &[popup]=*got;

			 auto impl=wimpl->get();

			 if (impl)
				 impl->set_official_color(IN_THREAD);
			 popup->hide();
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
			  impl->official_color_updated(IN_THREAD, initial{});
		  });
}

rgb color_pickerObj::current_color() const
{
	return impl->official_color.get();
}

LIBCXXW_NAMESPACE_END
