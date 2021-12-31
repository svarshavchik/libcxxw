/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/color_picker_config.H"
#include "x/w/color_picker_appearance.H"
#include "x/w/input_field_appearance.H"
#include "x/w/button_appearance.H"
#include "x/w/impl/canvas.H"
#include "x/w/canvas.H"
#include "x/w/button.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/label.H"
#include "generic_window_handler.H"
#include "screen_positions_impl.H"
#include "popup/popup_attachedto_element.H"
#include "color_picker/color_picker_impl.H"
#include "color_picker/color_picker_selector_impl.H"
#include "color_picker/color_picker_square_impl.H"
#include "color_picker/color_picker_alpha_canvas_impl.H"
#include "color_picker/color_picker_current_canvas_impl.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/label.H"
#include "x/w/input_field.H"
#include "x/w/input_field_filter.H"
#include "x/w/image_button.H"
#include "x/w/button.H"
#include "x/w/rgb.H"
#include "x/w/impl/theme_font_element.H"
#include "gridlayoutmanager.H"
#include "messages.H"
#include "dialog.H"
#include "x/w/uielements.H"
#include "defaulttheme.H"

#include <x/chrcasecmp.H>
#include <x/weakcapture.H>
#include <x/mpweakptr.H>
#include <x/xml/readlock.H>
#include <x/xml/xpath.H>
#include <cmath>
#include <limits>
#include <utility>
#include <sstream>
#include <iomanip>
#include <charconv>
#include <algorithm>

LIBCXXW_NAMESPACE_START

color_picker_config_appearance::color_picker_config_appearance()
	: const_color_picker_appearance{color_picker_appearance::base::theme()}
{
}

color_picker_config_appearance::~color_picker_config_appearance()=default;

color_picker_config_appearance::color_picker_config_appearance
(const color_picker_config_appearance &)=default;

color_picker_config_appearance &color_picker_config_appearance
::operator=(const color_picker_config_appearance &)=default;

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

static const struct {
	rgb color;
	const char name[20];
} basic_colors[16]={
		    {black,  "basic-color-black"},
		    {gray,   "basic-color-gray"},
		    {silver, "basic-color-silver"},
		    {white,  "basic-color-white"},
		    {maroon, "basic-color-maroon"},
		    {red,    "basic-color-red"},
		    {olive,  "basic-color-olive"},
		    {yellow, "basic-color-yellow"},

		    {green,  "basic-color-green"},
		    {lime,   "basic-color-lime"},
		    {teal,   "basic-color-teal"},
		    {aqua,   "basic-color-aqua"},
		    {navy,   "basic-color-navy"},
		    {blue,   "basic-color-blue"},
		    {fuchsia,"basic-color-fuchsia"},
		    {purple, "basic-color-purple"},
};

namespace {
#if 0
}
#endif

// Need a helper template that will grab the basic_colors' buttons out of
// the generated uielements, and place them into an array together with
// their names, for construction purposes.

template<typename=std::make_index_sequence<std::size(basic_colors)>>
struct get_basic_colors;

template<size_t ...i>
struct get_basic_colors<std::index_sequence<i...>> {

	static auto get(const uielements &ui)
	{
		return std::array<button, sizeof...(i)>{
			ui.get_element(basic_colors[i].name)...
		};
	}
};

#if 0
{
#endif
}

struct color_picker_layout_helper : public color_picker_popup_fieldsptr {

	const color_picker_config &config;
	rgb &initial_color_rgb;
	color_picker_layout_helper(const color_picker_config &config,
				   rgb &initial_color_rgb)
		: config{config}, initial_color_rgb{initial_color_rgb}
	{
	}

	standard_dialog_elements_t elements();
};

inline standard_dialog_elements_t color_picker_layout_helper::elements()
{
	return {
		{"square", [&](const auto &f)
			{
				auto parent_container=f->get_container_impl();

				rgb initial_fixed_color;

				initial_fixed_color.*
					(color_pickerObj::
					 implObj::
					 initial_fixed_component
					 )=
					initial_color_rgb.*
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
						 canvas_init_params
						 {
							 {config.appearance->picker_width},
							 {config.appearance->picker_height},
							 "color_picker_square@libcxx.com",
							 {image_color{"color-picker-alpha-background"}}});

				auto cps=color_picker_square::create(impl);
				square=cps;
				f->created_internally(cps);
			}},

		{"alpha", [&](const auto &f)
			  {
				  auto parent_container=f->get_container_impl();

				  auto impl=ref<color_picker_alpha_canvasObj
						::implObj>
					  ::create(parent_container,
						   initial_color_rgb);

				  auto c=color_picker_alpha_canvas
					  ::create(impl);

				  f->created_internally(c);
				  alpha=c;
			  }},
		{"ok", dialog_ok_button(_("Ok"),
					ok_button,
					0)},
		{"cancel", dialog_cancel_button
				(_("Cancel"),
				 cancel_button, 0)},
			};
}


static inline canvas
create_color_picker_canvas(
	const factory &f,
	const color_picker_config &config,
	const screen_positions_handleptr &config_handle,
	const color_pickerObj::implObj::official_color &initial_color)
{
	canvas_init_params ciparams{{config.appearance->width},
				    {config.appearance->height}};

	ciparams.background_color=
		image_color{"color-picker-alpha-background"};

	auto impl=ref<color_picker_current_canvasObj::implObj>
		::create(f->get_container_impl(), config_handle, initial_color,
			 ciparams);

	auto canvas=color_picker_current_canvas::create(impl);

	f->created_internally(canvas);

	return canvas;
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
		uielements &tmpl,
		const ref<gridlayoutmanagerObj::implObj> &lm_impl,
		const popup_attachedto_info &attachedto_info,
		const color_picker_config &config)
{
	auto glm=lm_impl->create_gridlayoutmanager();

	glm->generate("color-picker-popup", tmpl);

	auto p=color_picker_selector::create(attachedto_info,
					     contents_container_impl,
					     lm_impl);

	helper.square->create_tooltip(_("Click to pick a color"));

	return p;
}

// Clicked in an element. Translate the click's position into 0..rgb::maximum

static rgb_component_t compute_from_click(const element &e,
					  coord_t x,
					  dim_t s)
{
	auto xc=(coord_t::value_type)x;

	if (s > 0)
		--s; // If 's' is 10, our coordinates will range 0-9.

	auto sc=coord_t::truncate(s);

	if (xc < 0)
		return 0;

	if (xc >= sc)
		return rgb::maximum;

	return std::round(xc * 1.0 / sc * rgb::maximum);
}

typedef mpweakptr<ptr<color_pickerObj::implObj>> weak_impl;

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

// Define input field validators in the color picker popup, this is factored
// out for readability.

static void
create_manual_input_validator(
	uielements &tmpl,
	const char *name,
	rgb_component_t initial_value,
	const weak_impl &wimpl,
	void (color_pickerObj::implObj::*notify)(ONLY IN_THREAD)
)
{
	tmpl.create_validated_input_field
		(name,
		 [wimpl]
		 (ONLY IN_THREAD,
		  std::string s,
		  const input_lock &lock,
		  const auto &ignore) -> std::optional<rgb_component_t>
		 {
			 auto impl=wimpl->getptr();

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
					 (static_cast<std::string>
					  (gettextmsg(_("Entered value must be"
							" between %1% and %2%"),
						      min.str(), max.str())));
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
			 // Format current value for display.

			 constexpr auto digits=std::numeric_limits<
				 rgb_component_t>::digits10+1;

			 bool full_precision=false;
			 bool hexadecimal=false;

			 auto impl=wimpl->getptr();

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
		 initial_value,
		 [wimpl, notify]
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

			 auto impl=wimpl->getptr();

			 if (!impl)
				 return;

			 // Invoke new_rgb_values() or new_hsv_values()

			 ((*impl).*notify)(IN_THREAD);
		 });
}

// Configure the validated input fields now that they are created.

static void configure_manual_input_validator(
	ONLY IN_THREAD,
	const input_field &f,
	const weak_impl &wimpl,
	const validated_input_field<rgb_component_t>
	color_pickerObj::implObj::*validator,
	void (color_pickerObj::implObj::*notify)(ONLY IN_THREAD)
)
{
	// Make the input field's spin buttons do something useful.
	//
	// No matter what the currently displayed options are, the net effect
	// of a spin button is to adjust the color value's most significant
	// byte.

	f->on_spin(IN_THREAD,
		   [wimpl, notify, validator]
		   (ONLY IN_THREAD,
		    auto &lock,
		    const auto &trigger,
		    const auto &busy)
		   {
			   auto impl=wimpl->getptr();

			   if (!impl)
				   return;

			   auto &contents=((*impl).*validator)->contents;

			   auto current_value=contents->value();

			   if (!current_value)
				   return;

			   auto high_byte=get_rgb_high_byte(*current_value);

			   if (high_byte)
				   --high_byte;

			   contents->set(IN_THREAD,
					 lock,
					 rgb_from_high_byte(high_byte));

			   // Invoke new_rgb_values() or new_hsv_values()

			   ((*impl).*notify)(IN_THREAD);
		   },
		   [wimpl, notify, validator]
		   (ONLY IN_THREAD,
		    auto &lock,
		    const auto &trigger,
		    const auto &busy)
		   {
			   auto impl=wimpl->getptr();

			   if (!impl)
				   return;

			   auto &contents=((*impl).*validator)->contents;

			   auto current_value=contents->value();

			   if (!current_value)
				   return;

			   auto high_byte=get_rgb_high_byte(*current_value);

			   if (++high_byte == 0)
				   --high_byte;

			   contents->set(IN_THREAD,
					 lock,
					 rgb_from_high_byte(high_byte));

			   // Invoke new_rgb_values() or new_hsv_values()

			   ((*impl).*notify)(IN_THREAD);
		   });

	// As long as we're here, install a filter.

	f->on_filter(IN_THREAD,
		     [wimpl]
		     (ONLY IN_THREAD, const auto &info)
		     {
			 auto impl=wimpl->getptr();

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
}

color_picker factoryObj
::create_color_picker(const color_picker_config &config)
{
	color_picker_current_canvasptr color_picker_current;

	ptr<color_picker_selectorObj::implObj> color_picker_selector_impl;

	auto attachedto_info=popup_attachedto_info::create();

	auto initial_color_rgb=config.initial_color;

	auto parent_container=get_container_impl();

	screen_positions_handleptr config_handle;

	if (!config.name.empty())
	{
		auto handle=parent_container->get_window_handler()
			.widget_config_handle(
				libcxx_uri,
				"color_picker",
				config.name,
				attachedto_info
			);

		auto lock=handle->config();

		try {
			for (size_t i=0; i<4; i++)
			{
				auto lock2=lock->clone();

				auto xpath=lock2->get_xpath(rgb_channels[i]);

				if (xpath->count() != 1)
					continue;

				xpath->to_node();

				initial_color_rgb.*(rgb_fields[i])=
					lock2->get_text<rgb_component_t>();
			}
		} catch (const exception &e)
		{
			auto ee=EXCEPTION( "Error restoring color picker \""
					   << config.name << "\": " << e );

			ee->caught();
		}

		config_handle=handle;
	}

	color_picker_layout_helper helper{config, initial_color_rgb};

	auto initial_color=color_pickerObj::implObj::official_color
		::create(initial_color_rgb);

	// Weakly-captured implementation object, referenced by the various
	// callbacks.

	auto wimpl=weak_impl::create();

	uielements tmpl{helper.elements()};

	// Add the validators for manual entry input fields.
	{
		auto [h, s, v]=color_pickerObj::implObj
			::compute_hsv(initial_color_rgb);

		create_manual_input_validator(
			tmpl, "r-input-field", initial_color_rgb.r, wimpl,
			&color_pickerObj::implObj::new_rgb_values);
		create_manual_input_validator(
			tmpl, "g-input-field", initial_color_rgb.g, wimpl,
			&color_pickerObj::implObj::new_rgb_values);
		create_manual_input_validator(
			tmpl, "b-input-field", initial_color_rgb.b, wimpl,
			&color_pickerObj::implObj::new_rgb_values);
		create_manual_input_validator(
			tmpl, "a-input-field", initial_color_rgb.a, wimpl,
			&color_pickerObj::implObj::new_alpha_value);

		create_manual_input_validator(
			tmpl, "h-input-field", h, wimpl,
			&color_pickerObj::implObj::new_hsv_values);
		create_manual_input_validator(
			tmpl, "s-input-field", s, wimpl,
			&color_pickerObj::implObj::new_hsv_values);
		create_manual_input_validator(
			tmpl, "v-input-field", v, wimpl,
			&color_pickerObj::implObj::new_hsv_values);
	}

	auto [real_impl, popup_imagebutton, glm, color_picker_popup]
		=create_popup_attachedto_element
		(parent_container, attachedto_info,
		 config.appearance->attached_popup_appearance,
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
				 (color_picker_selector_impl, helper, tmpl,
				  lm_impl, info, config);
		 },
		 [&](const auto &f)
		 {
			 // The current value element shows the current
			 // color.

			 color_picker_current=
				 create_color_picker_canvas(f, config,
							    config_handle,
							    initial_color);
			 color_picker_current->show();
		 });

	// Grab the elements that were created in the popup, and finish
	// creating it.

	color_picker_popup_fields contents{helper};

	if (config.enable_alpha_channel)
	{
		pagelayoutmanager plm=tmpl.get_layoutmanager
			("color-picker-a-canvas-page-layout");

		plm->open(0);

		plm=tmpl.get_layoutmanager("color-picker-a-input-field-layout");

		plm->open(0);
	}
	else
	{
		label l=tmpl.get_element("a-label");

		l->update("");
	}

	auto fixed_canvas=tmpl.get_element("fixed-canvas");
	color_picker_alpha_canvas alpha=contents.alpha;

	// Capture the elements in the implementation object.

	auto impl=ref<color_pickerObj::implObj>
		::create(real_impl, popup_imagebutton,
			 color_picker_current,
			 config,
			 initial_color,
			 tmpl,
			 alpha,
			 fixed_canvas,
			 contents.square);

	wimpl->setptr(impl);

	// Finish a bunch of initializations IN_THREAD.
	//
	// By calling the IN_THREAD overloads we avoid a context switch for
	// each individual method call. We just need to capture all the objects
	// we created here and toss them over IN_THREAD.

	fixed_canvas->in_thread(
		[v_button=button{tmpl.get_element("v-button")},
		 h_button=button{tmpl.get_element("h-button")},
		 basic_color_buttons=get_basic_colors<>::get(tmpl),
		 color_channels=std::array<std::tuple<element,
		 const validated_input_field<rgb_component_t>
		 color_pickerObj::implObj::*,
		 void (color_pickerObj::implObj::*)(ONLY IN_THREAD)
		 >, 7>{
			 {
				 { tmpl.get_element("r-input-field"),
				   &color_pickerObj::implObj::r_value,
				   &color_pickerObj::implObj::new_rgb_values},

				 { tmpl.get_element("g-input-field"),
				   &color_pickerObj::implObj::g_value,
				   &color_pickerObj::implObj::new_rgb_values},

				 { tmpl.get_element("b-input-field"),
				   &color_pickerObj::implObj::b_value,
				   &color_pickerObj::implObj::new_rgb_values},

				 { tmpl.get_element("a-input-field"),
				   &color_pickerObj::implObj::a_value,
				   &color_pickerObj::implObj::new_alpha_value},

				 { tmpl.get_element("h-input-field"),
				   &color_pickerObj::implObj::h_value,
				   &color_pickerObj::implObj::new_hsv_values},

				 { tmpl.get_element("s-input-field"),
				   &color_pickerObj::implObj::s_value,
				   &color_pickerObj::implObj::new_hsv_values},

				 { tmpl.get_element("v-input-field"),
				   &color_pickerObj::implObj::v_value,
				   &color_pickerObj::implObj::new_hsv_values},
			 }
		 },
		 impl,
		 wimpl,
		 hexadecimal=image_button{
			 tmpl.get_element("hexadecimal")
		 },
		 full_precision=image_button{
			 tmpl.get_element("full-precision")
		 },
		 square=contents.square,
		 fixed_canvas,
		 alpha,
		 color_picker_popup,
		 cancel_button=contents.cancel_button,
		 ok_button=contents.ok_button
		]
		(ONLY IN_THREAD)
		{
			hexadecimal->set_value(
				IN_THREAD,
				impl->hexadecimal.get() ? 1:0);
			full_precision->set_value(
				IN_THREAD,
				impl->full_precision.get() ? 1:0);


			// Configure R, G, B, A, H, S, and V fields.

			for (const auto &[field, validator, notify]
				     : color_channels)
			{
				configure_manual_input_validator(
					IN_THREAD,
					field,
					wimpl,
					validator,
					notify
				);
			}

			// Load basic colors

			for (size_t color_num=0;
			     color_num < std::size(basic_colors);
			     ++color_num)
			{
				const auto &b=basic_color_buttons[color_num];

				b->on_activate(
					IN_THREAD,
					[color=basic_colors[color_num].color,
					 wimpl]
					(ONLY IN_THREAD,
					 const auto &trigger,
					 const auto &busy)
					{
						auto impl=wimpl->getptr();

						if (!impl)
							return;

						impl->set_color(IN_THREAD,
								color);
					});
			}

			// Invoke reformat_values() when the display options
			// change.

			hexadecimal->on_activate(
				IN_THREAD,
				[wimpl]
				(ONLY IN_THREAD,
				 size_t value,
				 const callback_trigger_t &trigger,
				 const busy &mcguffin)
				{
					if (std::holds_alternative<initial>(
						    trigger
					    ))
						// Ignore initial callback.
						return;

					auto impl=wimpl->getptr();

					if (impl)
					{
						impl->hexadecimal=value > 0;
						impl->reformat_values(
							IN_THREAD
						);
					}
				});

			full_precision->on_activate(
				IN_THREAD,
				[wimpl]
				(ONLY IN_THREAD,
				 size_t value,
				 const callback_trigger_t &trigger,
				 const busy &mcguffin)
				{
					if (std::holds_alternative<initial>(
						    trigger))
						// Ignore initial callback.
						return;

					auto impl=wimpl->getptr();

					if (impl)
					{
						impl->full_precision=value > 0;
						impl->reformat_values(
							IN_THREAD
						);
					}
				});

			// Clicking on the square gradient.

			square->on_button_event(
				IN_THREAD,
				[wimpl,
				 get=make_weak_capture(square)]
				(ONLY IN_THREAD,
				 const auto &be,
				 bool activated,
				 const auto &ignore)
				{
					auto got=get.get();

					if (!got)
						return false;

					auto &[square]=*got;

					auto color_picker_impl=wimpl->getptr();

					if (!color_picker_impl)
						return false;

					if (!activated || be.button != 1)
						return false;

					// Compute the new component values.

					auto &data=
						square->impl->data(IN_THREAD);
					auto h=compute_from_click
						(square,
						 data.last_motion_x,
						 data.current_position.width);

					auto v=compute_from_click
						(square,
						 data.last_motion_y,
						 data.current_position.height);

					// And update the color.

					color_picker_impl
						->update_hv_components
						(IN_THREAD, h, v, &be);

					return true;
				});

			// Clicking on the gradient strips

			h_button->on_activate(
				IN_THREAD,
				[wimpl]
				(ONLY IN_THREAD,
				 const auto &trigger,
				 const auto &ignore)
				{
					auto impl=wimpl->getptr();

					if (!impl)
						return;

					impl->swap_horiz_gradient(IN_THREAD);
				});

			v_button->on_activate(
				IN_THREAD,
				[wimpl]
				(ONLY IN_THREAD,
				 const auto &trigger,
				 const auto &ignore)
				{
					auto impl=wimpl->getptr();

					if (!impl)
						return;

					impl->swap_vert_gradient(IN_THREAD);
				});

			// Clicking or dragging the button on the bottom
			// fixed component strip adjusts the fixed component.

			fixed_canvas->on_button_event(
				IN_THREAD,
				[get=make_weak_capture(impl, fixed_canvas)]
				(ONLY IN_THREAD,
				 const auto &be,
				 bool activate_for,
				 const auto &ignore)
				{
					auto got=get.get();

					if (!got)
						return false;

					auto &[color_picker_impl, square]=*got;

					if (be.press != activate_for
					    || be.button != 1)
						return false;

					auto &data=
						square->impl->data(IN_THREAD);
					auto v=compute_from_click
						(square,
						 data.last_motion_x,
						 data.current_position.width);

					color_picker_impl
						->update_fixed_component
						(IN_THREAD, v, &be);

					return true;
				});

			fixed_canvas->on_motion_event(
				IN_THREAD,
				[wimpl, get=make_weak_capture(fixed_canvas)]
				(ONLY IN_THREAD,
				 const auto &me)
				{
					if (me.type !=
					    motion_event_type::real_motion ||
					    !(me.mask.buttons & 1))
						// Dragging with button 1
						// pressed
						return;

					auto got=get.get();

					if (!got)
						return;

					auto &[square]=*got;

					auto color_picker_impl=wimpl->getptr();

					if (!color_picker_impl)
						return;
					auto &data=square->impl
						->data(IN_THREAD);

					auto v=compute_from_click
						(square,
						 me.x,
						 data.current_position.width);

					color_picker_impl
						->update_fixed_component
						(IN_THREAD, v, &me);
				});

			// Clicking on the alpha channel strip.
			alpha->on_button_event(
				IN_THREAD,
				[wimpl,
				 get=make_weak_capture(alpha)]
				(ONLY IN_THREAD,
				 const auto &be,
				 bool activate_for,
				 const auto &ignore)
				{
					auto got=get.get();

					if (!got)
						return false;

					auto &[alpha]=*got;

					if (be.press != activate_for
					    || be.button != 1)
						return false;

					auto color_picker_impl=wimpl->getptr();

					if (!color_picker_impl)
						return false;

					auto &data=alpha->impl->data(IN_THREAD);
					auto v=compute_from_click
						(alpha,
						 data.last_motion_y,
						 data.current_position.height);

					color_picker_impl
						->update_alpha_component
						(IN_THREAD, v, &be);

					return true;
				});

			alpha->on_motion_event(
				IN_THREAD,
				[wimpl,
				 get=make_weak_capture(alpha)]
				(ONLY IN_THREAD,
				 const auto &me)
				{
					if (me.type !=
					    motion_event_type::real_motion ||
					    !(me.mask.buttons & 1))
						// Dragging with button 1
						// pressed
						return;

					auto got=get.get();

					if (!got)
						return;

					auto &[alpha]=*got;

					auto color_picker_impl=wimpl->getptr();

					if (!color_picker_impl)
						return;

					auto &data=alpha->impl->data(IN_THREAD);
					auto v=compute_from_click
						(alpha,
						 me.y,
						 data.current_position.height);

					color_picker_impl
						->update_alpha_component
						(IN_THREAD, v, &me);
				});

			/////////////////////////////////////////////////////

			// If the popup gets closed for any reason,
			// invoke popup_closed().

			color_picker_popup->on_state_update(
				IN_THREAD,
				[wimpl]
				(ONLY IN_THREAD,
				 const element_state &new_state,
				 const busy &mcguffin)
				{
					// popup_closed() must be tied to
					// "after_hiding".
					//
					// If one of the channel input fields
					// is modified and Esc is hit
					// triggering the popup closing, the
					// channel input validator executes,
					// updating the current color value
					// swatch, as part of hiding the popup,
					// since it loses the input focus.
					//
					// We need to call popup_closed() after
					// that occurs, to restore the
					// official color, so trigger this
					// to happen after the popup is hidden.

					if (new_state.state_update !=
					    new_state.after_hiding)
						return;

					auto impl=wimpl->getptr();

					if (impl)
						impl->popup_closed(IN_THREAD);
				});

			// The cancel button closes the popup.
			cancel_button->on_activate(
				IN_THREAD,
				[popup=make_weak_capture(color_picker_popup)]
				(ONLY IN_THREAD,
				 const auto &trigger,
				 const auto &busy)
				{
					auto got=popup.get();

					if (!got)
						return;

					auto &[popup]=*got;

					popup->elementObj::impl
						->request_visibility(IN_THREAD,
								     false);
				});

			//! The ok button calls set_official_color(),
			// then closes the popup.

			ok_button->on_activate(
				IN_THREAD,
				[wimpl,
				 popup=make_weak_capture(color_picker_popup)]
				(ONLY IN_THREAD,
				 const auto &trigger,
				 const auto &busy)
				{
					auto got=popup.get();

					if (!got)
						return;

					auto &[popup]=*got;

					auto impl=wimpl->getptr();

					if (impl)
						impl->set_official_color(
							IN_THREAD
						);

					popup->elementObj::impl
						->request_visibility(IN_THREAD,
								     false);
				});
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
	return impl->current_official_color->official_color.get();
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
	impl->current_official_color->official_color=c;
	impl->set_color(IN_THREAD, c);
	impl->official_color_updated(IN_THREAD, {});
}

LIBCXXW_NAMESPACE_END
