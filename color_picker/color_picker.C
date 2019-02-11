/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/color_picker_config.H"
#include "x/w/canvas.H"
#include "x/w/button.H"
#include "x/w/button_event.H"
#include "popup/popup_attachedto_element.H"
#include "color_picker/color_picker_impl.H"
#include "color_picker/color_picker_selector_impl.H"
#include "color_picker/color_picker_square_impl.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/label.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/button.H"
#include "x/w/rgb.H"
#include "x/w/impl/theme_font_element.H"
#include "gridlayoutmanager.H"
#include "messages.H"
#include "dialog.H"
#include "gridtemplate.H"

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
	: focusable_containerObj{impl->impl, container_layoutmanager},
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

namespace {
#if 0
}
#endif

#pragma GCC visibility push(hidden)
#include "color_picker/color_picker.inc.H"

struct color_picker_layout_helper : public color_picker_popup_fieldsptr {

	const color_picker_config &config;

	static constexpr auto digits=
		std::numeric_limits<rgb_component_t>::digits10+1;

	// The input field has one more position, for the trailing cursor.
	input_field_config input_fields_conf{digits+1};

	color_picker_layout_helper(const color_picker_config &config)
		: config{config}
	{
		input_fields_conf.alignment=halign::right;
		input_fields_conf.maximum_size=digits;
		input_fields_conf.autoselect=true;
		input_fields_conf.set_default_spin_control_factories();
	}

	dim_arg bw{"color_picker_buffer_width"};
	dim_arg bh{"color_picker_buffer_height"};

	dim_arg sw{"color_picker_strip_width"};
	dim_arg sh{"color_picker_strip_height"};

	std::vector<std::tuple<rgb, button>> basic_colors;

	inline void create_basic_colors(const gridlayoutmanager &glm)
	{
		static const rgb rgb_values[2][8]={
			{
				black,
				gray,
				silver,
				white,
				maroon,
				red,
				olive,
				yellow,
			},
			{
				green,
				lime,
				teal,
				aqua,
				navy,
				blue,
				fuchsia,
				purple,
			}};

		basic_colors.reserve(16);

		for (const auto &row:rgb_values)
		{
			auto f=glm->append_row();

			for (const auto &col:row)
			{
				auto b=f->create_normal_button
					([&]
					 (const auto &f)
					 {
						 f->create_canvas
						 ([&]
						  (const auto &c) {
							 c->set_background_color
							 (col);
						 },
					{"color_picker_basic_color_width"},
					{"color_picker_basic_color_height"});
					 });

				basic_colors.push_back(std::tuple{col, b});
			}
		}
	}

	standard_dialog_elements_t elements();
};

inline standard_dialog_elements_t color_picker_layout_helper::elements()
{
	return {
		{"filler", dialog_filler()},
		{"h-button", [&](const auto &f)
			{
				h_button=f->create_normal_button
					([&]
					 (const auto &button_factory)
					 {
						 h_canvas=button_factory
						 ->create_canvas
						 ([] (const auto &){}, {
						 }, {sh});
					 });
			}},
		{"v-button", [&](const auto &f)
			{
				v_button=f->create_normal_button
					([&]
					 (const auto &button_factory)
					 {
						 v_canvas=button_factory
						 ->create_canvas
						 ([](const auto &) {}, {
							 sw}, {});
					 });
			}},
		{"h-button-spacing", [&](const auto &f)
			{
				f->create_canvas([](const auto &) {},
						 {},
						 {bh});
			}},
		{"v-button-spacing", [&](const auto &f)
			{
				f->create_canvas([](const auto &) {},
						 {bw},
						 {});
			}},
		{"fixed-canvas", [&](const auto &f)
			{
				fixed_canvas=f->create_canvas([]
							      (const auto &){},
							      {},
							      {sh});

			}},
		{"square", [&](const auto &f)
			{
				auto parent_container=f->get_container_impl();

				rgb initial_fixed_color;

				initial_fixed_color.*
					(color_pickerObj::
					 implObj::
					 initial_fixed_component
					 )=
					config.initial_color.*
					(color_pickerObj::
					 implObj::
					 initial_fixed_component
					 );

				auto impl=ref<color_picker_squareObj::implObj>
					::create(parent_container,
						 // Initial channel color,
						 initial_fixed_color,
						 // Vary the red and the
						 // green channels.
						 color_pickerObj
						 ::implObj
						 ::initial_horiz_component,
						 color_pickerObj
						 ::implObj
						 ::initial_vert_component,
						 canvas_init_params{
							 {"color_picker_square_width"},
							 {"color_picker_square_height"},
								 "color_picker_square@libcxx.com"});

				auto cps=color_picker_square::create(impl);
				square=cps;
				f->created_internally(cps);
			}},

		{"r-label", [&](const auto &f)
			{
				f->create_label(TAG(_("RED:R:")));
			}},
		{"g-label", [&](const auto &f)
			{
				f->create_label(TAG(_("GREEN:G:")));
			}},
		{"b-label", [&](const auto &f)
			{
				f->create_label(TAG(_("BLUE:B:")));
			}},
		{"h-label", [&](const auto &f)
			{
				f->create_label(TAG(_("HUE:H:")));
			}},
		{"s-label", [&](const auto &f)
			{
				f->create_label(TAG(_("SATURATION:S:")));
			}},
		{"v-label", [&](const auto &f)
			{
				f->create_label(TAG(_("VALUE:V:")));
			}},

		{"r-input-field", [&](const auto &f)
			{
				r_input_field=f->create_input_field
					("", input_fields_conf);
			}},
		{"g-input-field", [&](const auto &f)
			{
				g_input_field=f->create_input_field
					("", input_fields_conf);
			}},
		{"b-input-field", [&](const auto &f)
			{
				b_input_field=f->create_input_field
					("", input_fields_conf);
			}},
		{"h-input-field", [&](const auto &f)
			{
				h_input_field=f->create_input_field
					("", input_fields_conf);
			}},
		{"s-input-field", [&](const auto &f)
			{
				s_input_field=f->create_input_field
					("", input_fields_conf);
			}},
		{"v-input-field", [&](const auto &f)
			{
				v_input_field=f->create_input_field
					("", input_fields_conf);
			}},
		{"hexadecimal", [&](const auto &f)
			{
				hexadecimal=f->create_checkbox
					([]
					 (const auto &f)
					 {
						 f->create_label
							 (_("Hexadecimal"));
					 });
			}},
		{"full-precision", [&](const auto &f)
			{
				full_precision=f->create_checkbox
					([]
					 (const auto &f)
					 {
						 f->create_label
							 (_("Full Precision"));
					 });
			}},
		{"error-message-field", [&](const auto &f)
			{
				error_message_field=f->create_label(" ");
			}},
		{"basic-colors", [&](const auto &f)
			{
				f->create_container
					([&]
					 (const container &c)
					 {
						 create_basic_colors
							 (c->get_layoutmanager()
							  );
					 },
					 new_gridlayoutmanager{});
			}},

		{"ok", dialog_ok_button(_("Ok"),
					ok_button,
					0)},
		{"cancel", dialog_cancel_button
				(_("Cancel"),
				 cancel_button, 0)},
			};
}

#pragma GCC visibility pop

#if 0
{
#endif
}

/////////////////////////////////////////////////////////////////////////////

// Create the contents of the color picker popup. Factored out for readability.
//
// The top level popup implementation/handler container parent gets passed in,
// this creates a color_picker_selectorObj as its child, with a
// color_picker_selector as the element.

static inline auto
create_contents(const ref<color_picker_selectorObj::implObj>
		&contents_container_impl,
		color_picker_layout_helper &helper,
		const ref<gridlayoutmanagerObj::implObj> &lm_impl,
		const popup_attachedto_info &attachedto_info,
		const color_picker_config &config)
{
	auto glm=lm_impl->create_gridlayoutmanager();

	gridtemplate tmpl{helper.elements()};

	glm->create("color-picker-popup", tmpl);

	auto p=color_picker_selector::create(attachedto_info,
					     contents_container_impl,
					     lm_impl);

	// Tooltips
	text_param button_tooltip{
		_("Select this color channel for adjustment")};

	helper.h_button->create_tooltip(button_tooltip);
	helper.v_button->create_tooltip(button_tooltip);
	helper.fixed_canvas->create_tooltip
		(_("Click to adjust this color channel in the currently"
		   " selected color"));
	helper.square->create_tooltip(_("Click to pick a color"));

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
			 uint8_t value_c{};

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

	// As long as we're here, install a filter.

	f->on_filter([wimpl]
		     (ONLY IN_THREAD, const auto &info)
		     {
			 auto impl=wimpl->get();

			 if (!impl)
				 return;

			 const auto &str=info.new_contents;

			 for (auto c:str)
				 if ( (char32_t)(char)c != c)
					 return;

			 auto s=str.size();
			 if (s == 0)
			 {
				 info.update();
				 return;
			 }

			 char buf[s];

			 std::copy(str.begin(), str.end(), buf);

			 rgb_component_t value{};

			 auto ret=std::from_chars(buf, buf+s,
						  value,
						  impl->hexadecimal.get()
						  ? 16:10);

			 if (ret.ec != std::errc{} || ret.ptr != buf+s)
				 return;

			 info.update();
		     });

	return validator;
}

color_picker factoryObj
::create_color_picker(const color_picker_config &config)
{
	canvasptr color_picker_current;

	ptr<color_picker_selectorObj::implObj> color_picker_selector_impl;

	color_picker_layout_helper helper{config};

	auto [real_impl, popup_imagebutton, glm, color_picker_popup]
		=create_popup_attachedto_element
		(*this, config.popup_config,

		 [&](const container_impl &parent,
		     const child_element_init_params &init_params)
		 {
			 auto impl=ref<color_picker_selectorObj::implObj>
			 ::create(parent, init_params);
			 color_picker_selector_impl=impl;

			 return impl;
		 },
		 [&](const popup_attachedto_info &info,
		     const ref<gridlayoutmanagerObj::implObj> &lm_impl)

		 {
			 return create_contents
			 (color_picker_selector_impl, helper,
			  lm_impl, info, config);
		 },
		 [&](const auto &f)
		 {
			 // The current value element shows the current
			 // color.

			 dim_arg w{"color_picker_current_width"};
			 dim_arg h{"color_picker_current_height"};

			 color_picker_current=f->create_canvas
			 ([&](const auto &c) {
				 c->set_background_color(config.initial_color);
			 }, {w}, {h});

			 color_picker_current->show();
		 });

	// Grab the elements that were created in the popup, and finish
	// creating it.

	color_picker_popup_fields contents{helper};

	// Capture the elements in the implementation object.

	auto impl=ref<color_pickerObj::implObj>
		::create(real_impl, popup_imagebutton,
			 color_picker_current,

			 config,
			 contents.h_canvas,
			 contents.v_canvas,
			 contents.fixed_canvas,
			 contents.square,
			 contents.error_message_field);

	auto wimpl=weak_impl::create(impl);

	// Install validators for manual R, G, and B fields.
	impl->r_value=make_manual_input_validator
		(contents.r_input_field, wimpl,
		 &color_pickerObj::implObj::new_rgb_values);
	impl->g_value=make_manual_input_validator
		(contents.g_input_field, wimpl,
		 &color_pickerObj::implObj::new_rgb_values);
	impl->b_value=make_manual_input_validator
		(contents.b_input_field, wimpl,
		 &color_pickerObj::implObj::new_rgb_values);

	// And set their initial values
	impl->r_value->set(config.initial_color.r);
	impl->g_value->set(config.initial_color.g);
	impl->b_value->set(config.initial_color.b);

	// Install validators for manual H, S, and V fields.

	impl->h_value=make_manual_input_validator
		(contents.h_input_field, wimpl,
		 &color_pickerObj::implObj::new_hsv_values);
	impl->s_value=make_manual_input_validator
		(contents.s_input_field, wimpl,
		 &color_pickerObj::implObj::new_hsv_values);
	impl->v_value=make_manual_input_validator
		(contents.v_input_field, wimpl,
		 &color_pickerObj::implObj::new_hsv_values);

	// And set their initial values.

	{
		auto [h, s, v]=color_pickerObj::implObj
			::compute_hsv(config.initial_color);

		impl->h_value->set(h);
		impl->s_value->set(s);
		impl->v_value->set(v);
	}

	contents.hexadecimal->set_value(impl->hexadecimal.get() ? 1:0);
	contents.full_precision->set_value(impl->full_precision.get() ? 1:0);

	// Load basic colors

	for (const auto &basic_color:helper.basic_colors)
	{
		auto &[color,b]=basic_color;

		b->on_activate([color,wimpl]
			       (ONLY IN_THREAD,
				const auto &trigger,
				const auto &busy)
			       {
				       auto impl=wimpl->get();

				       if (!impl)
					       return;

				       impl->set_color(IN_THREAD, color);
			       });
	}
	// Invoke reformat_values() when the display options change.

	contents.hexadecimal->on_activate
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

	contents.full_precision->on_activate
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

	contents.square->on_button_event
		([get=make_weak_capture(impl, contents.square)]
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

	contents.h_button->on_activate
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

	contents.v_button->on_activate
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

	contents.fixed_canvas->on_button_event
		([get=make_weak_capture(impl, contents.fixed_canvas)]
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

	// If the popup gets closed for any reason, invoke popup_closed().

	color_picker_popup->on_state_update
		([wimpl]
		 (ONLY IN_THREAD,
		  const element_state &new_state,
		  const busy &mcguffin)
		 {
			 // popup_closed() must be tied to "after_hiding".
			 //
			 // If one of the channel input fields is modified
			 // and Esc is hit triggering the popup closing, the
			 // channel input validator executes, updating the
			 // current color value swatch, as part of hiding
			 // the popup, since it loses the input focus.
			 //
			 // We need to call popup_closed() after that occurs,
			 // to restore the official color, so trigger this
			 // to happen after the popup is hidden.
			 if (new_state.state_update !=
			     new_state.after_hiding)
				 return;

			 auto impl=wimpl->get();

			 if (impl)
				 impl->popup_closed(IN_THREAD);
		 });

	// The cancel button closes the popup.
	contents.cancel_button->on_activate
		([popup=make_weak_capture(color_picker_popup)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto got=popup.get();

			 if (!got)
				 return;

			 auto &[popup]=*got;

			 popup->elementObj::impl->request_visibility(IN_THREAD,
								     false);
		 });

	//! The ok button calls set_official_color(), then closes the popup.
	contents.ok_button->on_activate
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
			 popup->elementObj::impl->request_visibility(IN_THREAD,
								     false);
		 });

	auto p=color_picker::create(impl, glm->impl);

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

void color_pickerObj::current_color(const rgb &c)
{
	in_thread([me=ref(this), c]
		  (ONLY IN_THREAD)
		  {
			  me->current_color(IN_THREAD, c);
		  });
}

void color_pickerObj::current_color(ONLY IN_THREAD, const rgb &c)
{
	impl->official_color=c;
	impl->set_color(IN_THREAD, c);
	impl->official_color_updated(IN_THREAD, {});
}

LIBCXXW_NAMESPACE_END
