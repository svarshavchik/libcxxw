#include "libcxxw_config.h"

#include "creator/app.H"
#include "x/w/uielements.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "x/w/input_field_config.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "x/w/uigenerators.H"
#include "messages.H"
#include "catch_exceptions.H"
#include <x/messages.H>
#include <x/visitor.H>
#include <x/xml/escape.H>
#include <x/weakcapture.H>
#include <x/locale.H>
#include <x/imbue.H>
#include <x/mpthreadlock.H>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <limits>

// Common initialization logic for color type radio buttons:
//
// Retrieve the generated radio button, install a callback that opens the
// appropriate page.

static x::w::image_button install_button_callback(const x::w::container
						  &page_container,
						  x::w::uielements &ui,
						  const char *button_name,
						  const char *page_name)
{
	x::w::image_button b=ui.get_element(button_name);

	b->on_activate
		([page_container, page_name]
		 (ONLY IN_THREAD,
		  size_t i,
		  const auto &trigger,
		  const auto &busy)
		 {
			 if (!i)
				 return;

			 auto lm=page_container->get_layoutmanager();

			 x::w::uielements ignore;

			 appinvoke([&]
				   (auto *app)
				   {
					   lm->generate(page_name,
							app->main_generator,
							ignore);
					   app->color_updated(IN_THREAD);
				   });
		 });

	return b;
}

// Shared on_selection_changed callback for a combo-box, invoking
// on_color_updated.

static void install_color_standard_combobox_layoutmanager
(const x::w::focusable_container &c)
{
	x::w::standard_comboboxlayoutmanager lm=c->get_layoutmanager();

	lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 if (info.list_item_status_info.selected)
				 appinvoke(&appObj::color_updated, IN_THREAD);
		 });
}

// Other pages' static widgets that enumerate available colors

namespace {
#if 0
}
#endif

struct other_color_widget {
	x::w::focusable_container appObj::*widget;
};

static const other_color_widget other_color_widgets[]=
	{
	 { &appObj::border_color },
	 { &appObj::border_color2 },
	};
#if 0
{
#endif
}

void appObj::colors_elements_initialize(app_elements_tptr &elements,
					x::w::uielements &ui,
					init_args &args)
{
	x::w::focusable_container color_name=
		ui.get_element("color-name-field");
	x::w::input_field color_new_name=
		ui.get_element("color-new-name-field");
	x::w::label color_new_name_label=
		ui.get_element("color-new-name-label");

	// Page container, with pages for each color type.

	x::w::container color_right_side_page=
		ui.get_element("color-right-side-page");

	// Radio buttons for each color type open the corresponding page.

	x::w::image_button color_basic_option_radio=
		install_button_callback(color_right_side_page, ui,
					"color-basic-option-radio",
					"color-right-side-page-open-basic");
	x::w::image_button color_scaled_option_radio=
		install_button_callback(color_right_side_page, ui,
					"color-scaled-option-radio",
					"color-right-side-page-open-scaled");
	x::w::image_button color_linear_gradient_option_radio=
		install_button_callback(color_right_side_page, ui,
					"color-linear-gradient-option-radio",
					"color-right-side-page-open-linear");
	x::w::image_button color_radial_gradient_option_radio=
		install_button_callback(color_right_side_page, ui,
					"color-radial-gradient-option-radio",
					"color-right-side-page-open-radial");


	// Basic color widget.

	x::w::color_picker color_basic=ui.get_element("color-basic");

	color_basic->on_color_update
		([]
		 (ONLY IN_THREAD,
		  const auto &new_color,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 appinvoke(&appObj::color_updated, IN_THREAD);
		 });
	// Scaled color
	x::w::focusable_container color_scaled_page_from=
		ui.get_element("color-scaled-page-from");
	x::w::input_field color_scaled_page_r=
		ui.get_element("color-scaled-page-contents-r-value-field");
	x::w::input_field color_scaled_page_g=
		ui.get_element("color-scaled-page-contents-g-value-field");
	x::w::input_field color_scaled_page_b=
		ui.get_element("color-scaled-page-contents-b-value-field");
	x::w::input_field color_scaled_page_a=
		ui.get_element("color-scaled-page-contents-a-value-field");

	elements.color_name=color_name;
	elements.color_new_name=color_new_name;
	elements.color_new_name_label=color_new_name_label;
	elements.color_right_side_page=color_right_side_page;
	elements.color_basic_option_radio=color_basic_option_radio;
	elements.color_scaled_option_radio=color_scaled_option_radio;
	elements.color_linear_gradient_option_radio=
		color_linear_gradient_option_radio;
	elements.color_radial_gradient_option_radio=
		color_radial_gradient_option_radio;
	elements.color_basic=color_basic;
	elements.color_scaled_page_from=color_scaled_page_from;

	elements.color_scaled_page_r=color_scaled_page_r;
	elements.color_scaled_page_g=color_scaled_page_g;
	elements.color_scaled_page_b=color_scaled_page_b;
	elements.color_scaled_page_a=color_scaled_page_a;

	// Linear gradient

	x::w::input_field color_linear_x1=
		ui.get_element("color-linear-x1");
	x::w::input_field color_linear_y1=
		ui.get_element("color-linear-y1");
	x::w::input_field color_linear_x2=
		ui.get_element("color-linear-x2");
	x::w::input_field color_linear_y2=
		ui.get_element("color-linear-y2");
	x::w::input_field color_linear_width=
		ui.get_element("color-linear-width");
	x::w::input_field color_linear_height=
		ui.get_element("color-linear-height");
	x::w::button color_add_linear_gradient_button=
		ui.get_element("color-add-linear-gradient-button");

	x::w::container color_linear_page_values_grid=
		ui.get_element("color-linear-page-values-grid");

	elements.color_linear_x1=color_linear_x1;
	elements.color_linear_y1=color_linear_y1;
	elements.color_linear_x2=color_linear_x2;
	elements.color_linear_y2=color_linear_y2;
	elements.color_linear_width=color_linear_width;
	elements.color_linear_height=color_linear_height;
	elements.color_add_linear_gradient_button=
		color_add_linear_gradient_button;
	elements.color_linear_page_values_grid=color_linear_page_values_grid;

	// Radial gradient
	x::w::input_field color_radial_inner_x=
		ui.get_element("color-radial-inner-x");
	x::w::input_field color_radial_inner_y=
		ui.get_element("color-radial-inner-y");
	x::w::input_field color_radial_inner_radius=
		ui.get_element("color-radial-inner-radius");
	x::w::focusable_container color_radial_inner_axis=
		ui.get_element("color-radial-inner-axis");

	x::w::input_field color_radial_outer_x=
		ui.get_element("color-radial-outer-x");
	x::w::input_field color_radial_outer_y=
		ui.get_element("color-radial-outer-y");
	x::w::input_field color_radial_outer_radius=
		ui.get_element("color-radial-outer-radius");
	x::w::focusable_container color_radial_outer_axis=
		ui.get_element("color-radial-outer-axis");
	x::w::input_field color_radial_fixed_width=
		ui.get_element("color-radial-fixed-width");
	x::w::input_field color_radial_fixed_height=
		ui.get_element("color-radial-fixed-height");
	x::w::button color_add_radial_gradient_button=
		ui.get_element("color-add-radial-gradient-button");

	x::w::container color_radial_page_values_grid=
		ui.get_element("color-radial-page-values-grid");

	elements.color_radial_inner_x=color_radial_inner_x;
	elements.color_radial_inner_y=color_radial_inner_y;
	elements.color_radial_inner_radius=color_radial_inner_radius;
	elements.color_radial_inner_axis=color_radial_inner_axis;
	elements.color_radial_outer_x=color_radial_outer_x;
	elements.color_radial_outer_y=color_radial_outer_y;
	elements.color_radial_outer_radius=color_radial_outer_radius;
	elements.color_radial_outer_axis=color_radial_outer_axis;
	elements.color_radial_fixed_width=color_radial_fixed_width;
	elements.color_radial_fixed_height=color_radial_fixed_height;
	elements.color_add_radial_gradient_button=
		color_add_radial_gradient_button;
	elements.color_radial_page_values_grid=color_radial_page_values_grid;

	color_new_name->on_validate([]
				    (ONLY IN_THREAD,
				     const auto &trigger)
				    {
					    appinvoke(&appObj::color_updated,
						      IN_THREAD);

					    return true;
				    });

	// Preview canvas.

	elements.color_preview_cell_border_container=
		ui.get_element("color-preview-cell-border-container");

	elements.color_preview_cell_canvas=
		ui.get_element("color-preview-cell-canvas");

	// Callbacks on various combo-boxes

	x::w::standard_comboboxlayoutmanager color_name_lm=
		color_name->get_layoutmanager();

	color_name_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::color_selected, IN_THREAD,
				   info);
		 });

	install_color_standard_combobox_layoutmanager(color_radial_inner_axis);
	install_color_standard_combobox_layoutmanager(color_radial_outer_axis);

	x::w::editable_comboboxlayoutmanager
	{color_scaled_page_from->get_layoutmanager()}
	->on_validate
		  ([]
		   (ONLY IN_THREAD,
		    const auto &info)
		   {
			   appinvoke(&appObj::color_updated, IN_THREAD);
			   return true;
		   });

	x::w::button color_update_button=
		ui.get_element("color_update_button");
	x::w::button color_reset_button=
		ui.get_element("color_reset_button");
	x::w::button color_delete_button=
		ui.get_element("color_delete_button");

	elements.color_update_button=color_update_button;
	elements.color_reset_button=color_reset_button;
	elements.color_delete_button=color_delete_button;

	color_update_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::color_update);
		 });

	color_reset_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 appinvoke(&appObj::color_reset, IN_THREAD);
		 });

	color_delete_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::color_delete);
		 });
}

void appObj::colors_initialize(ONLY IN_THREAD)
{
	colors_info_t::lock lock{colors_info};

	auto existing_colors=theme.get()->readlock();

	existing_colors->get_root();
	auto xpath=existing_colors->get_xpath("/theme/color");
	auto n=xpath->count();

	lock->ids.clear();
	lock->ids.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		lock->ids.push_back(existing_colors->get_any_attribute("id"));
	}

	std::sort(lock->ids.begin(), lock->ids.end());

	// Duplicate IDS: TODO - report this.

	lock->ids.erase(std::unique(lock->ids.begin(), lock->ids.end()),
			lock->ids.end());

	// For the color_name combo-box, put "-- New Color--", followed
	// by all loaded colors.
	std::vector<x::w::list_item_param> combobox_items;

	combobox_items.reserve(lock->ids.size()+1+x::w::n_rgb_colors);

	combobox_items.push_back(_("-- New Color --"));
	combobox_items.insert(combobox_items.end(),
			      lock->ids.begin(),
			      lock->ids.end());

	x::w::standard_comboboxlayoutmanager lm=
		color_name->get_layoutmanager();

	lm->replace_all_items(IN_THREAD, combobox_items);

	// The autoselect will initialize the rest.
	lm->autoselect(IN_THREAD, 0, {});

	// Initialize widgets on other pages with standard colors and the
	// current colors.

	combobox_items[0]=x::w::separator{};
	combobox_items.insert(combobox_items.begin(),
			      x::w::rgb_color_names,
			      x::w::rgb_color_names+
			      x::w::n_rgb_colors);

	for (const auto &other:other_color_widgets)
	{
		lm= (this->*(other.widget))->get_layoutmanager();
		lm->replace_all_items(combobox_items);
	}
}

void appObj::color_reset(ONLY IN_THREAD)
{
	colors_info_t::lock lock{colors_info};

	color_selected_locked(IN_THREAD, lock);

	if (lock->current_selection)
		color_name->request_focus();
	else
		color_new_name->request_focus();
}

void appObj::color_selected(ONLY IN_THREAD,
			    const
			    x::w::standard_combobox_selection_changed_info_t
			    &info)
{
	colors_info_t::lock lock{colors_info};

	if (!info.list_item_status_info.selected)
	{
		color_unselected_locked(IN_THREAD, lock);
		return;
	}

	size_t n=info.list_item_status_info.item_number;

	if (n == 0) // New color, no selection.
		lock->current_selection.reset();
	else
	{
		// Color selected.

		lock->current_selection.emplace();
		auto &orig_params=*lock->current_selection;
		orig_params.index=--n;
	}
	color_selected_locked(IN_THREAD, lock);
}

void appObj::color_unselected_locked(ONLY IN_THREAD,
				     colors_info_t::lock &lock)
{
	// When a selection is changed, the old value is always
	// deselected first. So, hook this to clear and reset things.

	color_new_name->set("");
	color_new_name_label->hide(IN_THREAD);
	color_new_name->hide(IN_THREAD);
	return;
}

void appObj::color_selected_locked(ONLY IN_THREAD,
				   colors_info_t::lock &lock)
{
	busy();
	if (!lock->current_selection) // New color entry
	{
		// Clear and show the new name field.
		// Basic color selected.
		lock->current_selection.reset();
		color_new_name_label->show(IN_THREAD);
		color_new_name->show(IN_THREAD);
		color_new_name->set(IN_THREAD, "");
		color_basic_option_radio->set_value(IN_THREAD, 1);
		color_reset_values(IN_THREAD, lock);
		return;
	}

	auto &orig_params=*lock->current_selection;

	const size_t n=orig_params.index;

	auto current_value=theme.get()->readlock();
	current_value->get_root();

	auto xpath=get_xpath_for(current_value,
				 "color",
				 n < lock->ids.size()
				 ? lock->ids[n]
				 : "" /* Boom, on next line */);

	// We expect one to be there, of course.
	xpath->to_node(1);

	// Use the UI parser to parse the color.
	auto parsed_color=x::w::ui::parse_color(current_value);

	std::visit
		(x::visitor
		 {[&](const x::w::rgb &color)
		  {
			  orig_params.loaded_color=color;
		  },
		  [&](const x::w::ui::parsed_scaled_color &color)
		  {
			  orig_params.loaded_color=color;
		  },
		  [&](const x::w::ui::parse_linear_gradient &p)
		  {
			  orig_params.loaded_color
				  .emplace<loaded_linear_gradient>();

			  auto &lg=std::get<loaded_linear_gradient>
				  (orig_params.loaded_color);

			  p.parse(current_value, lg,
				  [&]
				  (size_t value, const std::string &color)
				  {
					  lg.gradient.emplace(value, color);
				  });
		  },
		  [&](const x::w::ui::parse_radial_gradient &p)
		  {
			  orig_params.loaded_color
				  .emplace<loaded_radial_gradient>();

			  auto &rg=std::get<loaded_radial_gradient>
				  (orig_params.loaded_color);

			  p.parse(current_value, rg,
				  [&]
				  (size_t value, const std::string &color)
				  {
					  rg.gradient.emplace(value, color);
				  });
		  }},
		 parsed_color);

	// Hide the new name field.
	color_new_name_label->hide(IN_THREAD);
	color_new_name->hide(IN_THREAD);
	color_reset_values(IN_THREAD, lock);
}

// For the basic RGB color, enumerate:
//
// 1) XML element, <r>, <g>, <b>, or <a>.
// 2) The corresponding field in parsed_scaled_color
// 3) The validator for the on-screen field, with the current value.

static const struct {
	const char name[2];
	std::optional<double> x::w::ui::parsed_scaled_color::*field;
	const x::w::validated_input_field<std::optional<double>
					  > appObj::*validator;
} scaled_color_fields[]={
			 {"r",
			  &x::w::ui::parsed_scaled_color::r,
			  &appObj::color_scaled_r_validated},
			 {"g",
			  &x::w::ui::parsed_scaled_color::g,
			  &appObj::color_scaled_g_validated},
			 {"b",
			  &x::w::ui::parsed_scaled_color::b,
			  &appObj::color_scaled_b_validated},
			 {"a",
			  &x::w::ui::parsed_scaled_color::a,
			  &appObj::color_scaled_a_validated},
};

// For the linear gradient, enumerate:
//
// 1) Each XML element.
// 2) The correpsonding value in linear_gradient_values
// 3) The corresponding input_field and its validator.
// 4) The corresponding value i the loaded_linear_gradient

static const struct {
	const char name[9];
	double x::w::linear_gradient_values::*field;
	x::w::input_field appObj::*input_field;
	const x::w::validated_input_field<double> appObj::*validator;
	double appObj::loaded_linear_gradient::*loaded_field;
} linear_gradient_color_fields[]=
	{
	 {"x1",
	  &x::w::linear_gradient_values::x1,
	  &appObj::color_linear_x1,
	  &appObj::color_linear_x1_validated,
	  &appObj::loaded_linear_gradient::x1},
	 {"y1",
	  &x::w::linear_gradient_values::y1,
	  &appObj::color_linear_y1,
	  &appObj::color_linear_y1_validated,
	  &appObj::loaded_linear_gradient::y1},
	 {"x2",
	  &x::w::linear_gradient_values::x2,
	  &appObj::color_linear_x2,
	  &appObj::color_linear_x2_validated,
	  &appObj::loaded_linear_gradient::x2},
	 {"y2",
	  &x::w::linear_gradient_values::y2,
	  &appObj::color_linear_y2,
	  &appObj::color_linear_y2_validated,
	  &appObj::loaded_linear_gradient::y2},
	 {"widthmm",
	  &x::w::linear_gradient_values::fixed_width,
	  &appObj::color_linear_width,
	  &appObj::color_linear_width_validated,
	  &appObj::loaded_linear_gradient::fixed_width},
	 {"heightmm",
	  &x::w::linear_gradient_values::fixed_height,
	  &appObj::color_linear_height,
	  &appObj::color_linear_height_validated,
	  &appObj::loaded_linear_gradient::fixed_height}
	};


// For the radial gradient, enumerate:
//
// 1) Each XML element.
// 2) The correpsonding value in radial_gradient_values
// 3) The corresponding input_field and its validator.
// 4) The corresponding value i the loaded_linear_gradient

static const struct {
	char name[14];
	double x::w::radial_gradient_values::*field;
	x::w::input_field appObj::*input_field;
	const x::w::validated_input_field<double> appObj::*validator;
	double appObj::loaded_radial_gradient::*loaded_field;
} radial_gradient_color_fields[]=
	{
	 {"inner_x",
	  &x::w::radial_gradient_values::inner_center_x,
	  &appObj::color_radial_inner_x,
	  &appObj::color_radial_inner_x_validated,
	  &appObj::loaded_radial_gradient::inner_center_x},
	 {"inner_y",
	  &x::w::radial_gradient_values::inner_center_y,
	  &appObj::color_radial_inner_y,
	  &appObj::color_radial_inner_y_validated,
	  &appObj::loaded_radial_gradient::inner_center_y},
	 {"inner_radius",
	  &x::w::radial_gradient_values::inner_radius,
	  &appObj::color_radial_inner_radius,
	  &appObj::color_radial_inner_radius_validated,
	  &appObj::loaded_radial_gradient::inner_radius},
	 {"outer_x",
	  &x::w::radial_gradient_values::outer_center_x,
	  &appObj::color_radial_outer_x,
	  &appObj::color_radial_outer_x_validated,
	  &appObj::loaded_radial_gradient::outer_center_x},
	 {"outer_y",
	  &x::w::radial_gradient_values::outer_center_y,
	  &appObj::color_radial_outer_y,
	  &appObj::color_radial_outer_y_validated,
	  &appObj::loaded_radial_gradient::outer_center_y},
	 {"outer_radius",
	  &x::w::radial_gradient_values::outer_radius,
	  &appObj::color_radial_outer_radius,
	  &appObj::color_radial_outer_radius_validated,
	  &appObj::loaded_radial_gradient::outer_radius},
	 {"widthmm",
	  &x::w::radial_gradient_values::fixed_width,
	  &appObj::color_radial_fixed_width,
	  &appObj::color_radial_fixed_width_validated,
	  &appObj::loaded_radial_gradient::fixed_width},
	 {"heightmm",
	  &x::w::radial_gradient_values::fixed_height,
	  &appObj::color_radial_fixed_height,
	  &appObj::color_radial_fixed_height_validated,
	  &appObj::loaded_radial_gradient::fixed_height},
	};

// For the radial gradient, also enumerate the two axis combo-boxes.

static const struct {

	char name[18];
	x::w::radial_gradient_values::radius_axis
	x::w::radial_gradient_values::*field;
	x::w::focusable_container appObj::*combobox;
} radial_gradient_color_axises[]=
	{
	 {"inner_radius_axis",
	  &x::w::radial_gradient_values::inner_radius_axis,
	  &appObj::color_radial_inner_axis
	 },
	 {"outer_radius_axis",
	  &x::w::radial_gradient_values::outer_radius_axis,
	  &appObj::color_radial_outer_axis
	 },
	};

// Shared logic for creating the color combo-boxes for the gradients.
//
// We don't expect large, complicated gradients, so we'll create an
// individual combo-box for each gradient, listing the standard rgb colors,
// and all other colors in the theme file except the current one being
// shown.
//
// This is also used for the scaled-from color combo-box dropdown.

struct appObj::color_create_gradient_row {

	struct LIBCXX_HIDDEN in_thread;

	colors_info_t::lock &lock;

	// Colors without the current color
	std::vector<std::string> ids;

	// Converted to list_item_params.
	//
	// Optimizing the initial load.
	std::vector<x::w::list_item_param> existing_colors;

	color_create_gradient_row(colors_info_t::lock &lock)
		: lock{lock},
		  ids{lock->ids}
	{
		// We started by copying all colors from the theme file.
		// If there's a current selection showing, remove the
		// currently-selected color from the list.
		if (lock->current_selection &&
		    lock->current_selection->index < ids.size())
			ids.erase(ids.begin() + lock->current_selection->index);

		// Now, prepend the standard colors and the separator line
		// to the list. This forms the combo-box items.
		existing_colors.reserve(x::w::n_rgb_colors + ids.size()+1);

		existing_colors.insert(existing_colors.end(),
				       x::w::rgb_color_names,
				       x::w::rgb_color_names+
				       x::w::n_rgb_colors);
		existing_colors.emplace_back(x::w::separator{});
		existing_colors.insert(existing_colors.end(),
				       ids.begin(),
				       ids.end());
	}

	// A new color was entered, and we want to autoselect it in the
	// combo-box.

	static void autoselect_existing_color
	(ONLY IN_THREAD,
	 x::w::editable_comboboxlayoutmanager &lm,
	 const std::vector<std::string> &ids,
	 const std::string &color)
	{
		if (color.empty())
			return;

		// First, check it if it's one of the colors defined in the
		// theme file. They are sorted, so we can use lower_bound.

		auto iter=std::lower_bound(ids.begin(), ids.end(), color);

		if (iter != ids.end() && *iter == color)
		{
			// However in the combo-box, this list is preceded
			// by the standard RGB colors, and a separator line.
			// Adjust for them.
			lm->autoselect(IN_THREAD,
				       iter-ids.begin()
				       + x::w::n_rgb_colors+1, {});
			return;
		}

		// This must be one of the standard colors
		auto std_iter=std::find(x::w::rgb_color_names,
					x::w::rgb_color_names+
					x::w::n_rgb_colors,
					color);

		if (std_iter != x::w::rgb_color_names+
		    x::w::n_rgb_colors)
		{
			lm->autoselect(IN_THREAD,
				       std_iter-x::w::rgb_color_names, {});
			return;
		}

		// There's probably going to be an error, later, but for
		// now we can do what we're told by manually setting
		// the input field manually.

		lm->set(IN_THREAD, color);
	}

	// Shared code that adds a row with inputs for a new gradient color
	// value.
	//
	// Passing in the container (linear or gradient color tab), the
	// factory for inserting the row, and the initial contents.
	//
	// When creating the color inputs for a color, initial creation,
	// we pass in the initial value and the initial_color. The "Add"
	// button passes null and an empty string.

	std::tuple<x::w::input_field,
		   x::w::button>
		add(ONLY IN_THREAD,
		    const x::w::container &container,
		    const x::w::gridlayoutmanager &glm,
		    const x::w::gridfactory &f,
		    std::optional<size_t> initial_value,
		    const std::string &initial_color);

private:

	virtual void show(const x::w::element &e)
	{
		e->show();
	}
};

// Adding a gradient row in the connection thread, use IN_THREAD
// overloads of show().
struct appObj::color_create_gradient_row::in_thread
	: color_create_gradient_row {

	ONLY IN_THREAD;

	in_thread(ONLY IN_THREAD, colors_info_t::lock &lock)
		: color_create_gradient_row{lock},
		  IN_THREAD{IN_THREAD}
	{
	}

	void show(const x::w::element &e) override
	{
		e->show(IN_THREAD);
	}
};


void appObj::color_reset_values(ONLY IN_THREAD, colors_info_t::lock &lock)
{
	// Default values for each color alternative.
	x::w::rgb reset_rgb_color;
	x::w::ui::parsed_scaled_color reset_scaled_color;
	loaded_linear_gradient reset_linear_gradient;
	loaded_radial_gradient reset_radial_gradient;

	// Default gradient black to white.
	reset_linear_gradient.gradient=
		{
		 {0, x::w::rgb_color_names[0]},
		 {100, x::w::rgb_color_names[1]},
		};

	for (size_t i=0; i<x::w::n_rgb_colors; ++i)
	{
		if (x::w::rgb_colors[i] == x::w::black)
			reset_linear_gradient.gradient[0]=
				x::w::rgb_color_names[i];
		if (x::w::rgb_colors[i] == x::w::white)
			reset_linear_gradient.gradient[100]=
				x::w::rgb_color_names[i];
	}
	reset_radial_gradient.gradient=
		reset_linear_gradient.gradient;

	// Check the currently selected color type, and have it update one
	// of the above defaults.

	auto selected_option=color_basic_option_radio;

	if (lock->current_selection)
	{
		std::visit
			(x::visitor
			 {[&, this](const x::w::rgb &c)
			  {
				  reset_rgb_color=c;
			  },
			  [&, this](const x::w::ui::parsed_scaled_color &c)
			  {
				  reset_scaled_color=c;
				  selected_option=color_scaled_option_radio;
			  },
			  [&, this](const loaded_linear_gradient &c)
			  {
				  reset_linear_gradient=c;
				  selected_option=
					  color_linear_gradient_option_radio;
			  },
			  [&, this](const loaded_radial_gradient &c)
			  {
				  reset_radial_gradient=c;
				  selected_option=
					  color_radial_gradient_option_radio;
			  }},
			 lock->current_selection->loaded_color);
	}

	// We can now use the defaults to populate each color type's page.
	//
	// First, the basic color.
	color_basic->current_color(IN_THREAD, reset_rgb_color);


	// The linear gradient

	for (const auto &f:linear_gradient_color_fields)
		(this->*(f.validator))->set(reset_linear_gradient.*(f.field));

	// The radial gradient
	for (const auto &f:radial_gradient_color_fields)
		(this->*(f.validator))->set(reset_radial_gradient.*(f.field));

	for (const auto &f:radial_gradient_color_axises)
	{
		x::w::standard_comboboxlayoutmanager lm=
			(this->*f.combobox)->get_layoutmanager();
		lm->autoselect(IN_THREAD, static_cast<size_t>
			       (reset_radial_gradient.*(f.field)), {});
	}

	// Values of linear and radial gradients.

	struct {
		loaded_color_gradient_t &gradient;
		x::w::container &list_container;
		x::w::button &add_button;
	} linear_and_radial_gradient_values[]=
		  {
		   {reset_linear_gradient.gradient,
		    color_linear_page_values_grid,
		    color_add_linear_gradient_button},
		   {reset_radial_gradient.gradient,
		    color_radial_page_values_grid,
		    color_add_radial_gradient_button}
		  };

	color_create_gradient_row create_gradient_row{lock};

	for (const auto &values:linear_and_radial_gradient_values)
	{
		x::w::gridlayoutmanager glm=
			values.list_container->get_layoutmanager();

		// Remove existing rows.

		glm->remove_rows(1, glm->rows()-2);

		size_t row=1;

		for (const auto &v:values.gradient)
		{
			auto f=glm->insert_row(row++);

			const auto &[input_field, delete_button] =
				create_gradient_row.add(IN_THREAD,
							values.list_container,
							glm,
							f,
							v.first,
							v.second);

			// 0th gradient, and its Delete button, are disabled.
			if (v.first == 0)
			{
				input_field->set_enabled(false);
				delete_button->set_enabled(false);
			}
		}

		// The "Add" button

		values.add_button->on_activate
			([container=make_weak_capture(values.list_container)]
			 (ONLY IN_THREAD,
			  const auto &trigger,
			  const auto &mcguffin)
			 {
				 auto got=container.get();

				 if (!got)
					 return;

				 auto &[container]=*got;

				 x::w::gridlayoutmanager glm=
					 container->get_layoutmanager();

				 appinvoke
					 ([&]
					  (auto *app)
					  {
						  app->color_add_gradient_row
							  (IN_THREAD,
							   container, glm)
							  ->request_focus();

						  // Disable the "Add"
						  // button.
						  app->color_add_enable_disable
							  (IN_THREAD,
							   container, glm,
							   false);
					  });
			 });
	}


	// Now the scaled color.

	{
		x::w::editable_comboboxlayoutmanager lm=
			color_scaled_page_from->get_layoutmanager();

		lm->replace_all_items(IN_THREAD,
				      create_gradient_row.existing_colors);

		lm->set(IN_THREAD, reset_scaled_color.from_name,
			!reset_scaled_color.from_name.empty());

		if (reset_scaled_color.from_name.empty())
		{
			lm->unselect(IN_THREAD);
		}
		else
		{
			color_create_gradient_row::autoselect_existing_color
				(IN_THREAD,
				 lm,
				 create_gradient_row.ids,
				 reset_scaled_color.from_name);
		}

		for (const auto &field:scaled_color_fields)
		{
			std::optional<double> v;

			if (reset_scaled_color.*(field.field))
			{
				auto s=fmtdblval(*(reset_scaled_color.*
						   (field.field)));

				std::istringstream i{s};

				v.emplace(0);

				i >> *v;
			}
			(this->*(field.validator))->set(v);
		}
	}

	selected_option->set_value(IN_THREAD, 1);

	// The "Add" buttons should always be enabled now.

	color_add_button_enable_disable
		(IN_THREAD,
		 color_linear_page_values_grid->get_layoutmanager(), true);

	color_add_button_enable_disable
		(IN_THREAD,
		 color_radial_page_values_grid->get_layoutmanager(), true);

	// And do color update processing.
	color_updated_locked(IN_THREAD, lock);
}

x::w::input_field
appObj::color_add_gradient_row(ONLY IN_THREAD,
			       const x::w::container &container,
			       const x::w::gridlayoutmanager &glm)
{
	auto f=glm->insert_row(glm->rows()-1);

	colors_info_t::lock lock{colors_info};

	color_create_gradient_row create_gradient_row{lock};

	const auto &[input_field, delete_button]=
		create_gradient_row.add(IN_THREAD,
					container, glm,
					f,
					std::nullopt,
					"");
	return input_field;
}

void appObj::color_add_enable_disable(ONLY IN_THREAD,
				      const x::w::container &container,
				      const x::w::gridlayoutmanager &glm)
{
	color_add_enable_disable(IN_THREAD,
				 container,
				 glm,
				 parse_gradient_rows(container, glm,
						     []
						     (size_t,
						      const auto &)
						     {
						     }));
}

bool appObj::do_parse_gradient_rows(const x::w::container &container,
				    const x::w::gridlayoutmanager &glm,
				    const x::function<void
				    (size_t,
				     const std::string &)> &parser)
{
	size_t n=glm->rows()-1;

	for (size_t i=1; i<n; i++)
	{
		x::w::input_field f=glm->get(i, 0);

		// We store the validator here.
		x::w::validated_input_field<size_t> validator=f->appdata;

		auto v=validator->validated_value.get();

		if (!v)
			return false;

		parser(*v,
		       // Cell 1 is the combo-box with the color's value.
		       x::w::focusable_container{glm->get(i, 1)}
		       ->editable_combobox_get());
	}

	return true;
}

void appObj::color_add_enable_disable(ONLY IN_THREAD,
				      const x::w::container &container,
				      const x::w::gridlayoutmanager &glm,
				      bool enable_disable)
{
	color_add_button_enable_disable(IN_THREAD, glm, enable_disable);
	color_updated(IN_THREAD);
}

void appObj::color_add_button_enable_disable(ONLY IN_THREAD,
					     const x::w::gridlayoutmanager &glm,
					     bool enable_disable)
{
	x::w::button b=glm->get(glm->rows()-1, 0);

	b->set_enabled(IN_THREAD, enable_disable);
}

// Add a new gradient color row, initialize all fields to empty, ready to
// be inputed.

std::tuple<x::w::input_field, x::w::button>
appObj::color_create_gradient_row::add(ONLY IN_THREAD,
				       const x::w::container &container,
				       const x::w::gridlayoutmanager &glm,
				       const x::w::gridfactory &f,
				       std::optional<size_t> initial_value,
				       const std::string &initial_color)
{
	// Numeric gradient value

	x::w::input_field_config if_config;

	if_config.columns=6;
	if_config.alignment=x::w::halign::right;
	auto value=f->valign(x::w::valign::middle)
		.create_input_field("", if_config);

	// Validator for the gradient value
	//
	// Not only we will validate the numeric value, we'll check to make
	// sure all are unique, and we will automatically sort the rows
	// by value.

	auto validator=value->set_string_validator
		([container=make_weak_capture(container)]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  size_t *parsed_value,
		  const auto &field,
		  const auto &trigger)
		 -> std::optional<size_t>
		 {
			 if (!parsed_value)
			 {
				 field->stop_message
					 (_("Enter the gradient"
					    " scale position, "
					    " as a positive numeric"
					    " value"));
				 return std::nullopt;
			 }

			 // make sure this is not a dupe.

			 auto got=container.get();

			 if (!got)
				 return *parsed_value;

			 auto &[container]=*got;

			 x::w::gridlayoutmanager glm=
				 container->get_layoutmanager();

			 // skip this row, when checking for dupes.
			 auto rowcol=glm->lookup_row_col(field);

			 if (!rowcol)
				 return *parsed_value;

			 auto &[row, col]=*rowcol;

			 // 1) Check for dupes
			 // 2) Figure out where this index value should go,
			 //    keeping all rows in sorted order.

			 size_t real_row=1; // Where we think this row belongs.

			 auto n=glm->rows();

			 for (size_t i=1; i+1<n; ++i)
			 {
				 if (i == row)
					 continue; // This is me.

				 x::w::input_field f=glm->get(i, 0);

				 // We store the validator here
				 x::w::validated_input_field<size_t>
					 validator=f->appdata;

				 auto v=validator->validated_value.get();

				 // Not validated, pretend it's not there.
				 if (!v)
					 continue;
				 if (*v == *parsed_value)
				 {
					 field->stop_message
						 (_("All gradient values must"
						    " be unique"));
					 return std::nullopt;
				 }

				 // As long as we see a smaller value,
				 // make a very adamant statement that this
				 // value belongs on the next line.

				 if (*v < *parsed_value)
					 real_row=i+1;
			 }

			 if (real_row != row)
			 {
				 // Ok, move the row.
				 //
				 // Start by creating the sort_by index.

				 std::vector<size_t> indexes;

				 indexes.resize(n);
				 size_t i=0;
				 std::generate_n(indexes.begin(), n,
						 [&]
						 {
							 return i++;
						 });

				 indexes.erase(indexes.begin()+row);

				 if (real_row > row)
					 --real_row; // erase() moved it.

				 indexes.insert(indexes.begin()+real_row,
						row);

				 glm->resort_rows(indexes);

				 // Update tabbing order.
				 //
				 // There's always a focusable element on
				 // the next row. It could be the "Add"
				 // button.

				 x::w::focusable next_focusable=
					 glm->get(real_row+1, 0);

				 next_focusable->get_focus_before_me
					 ({ glm->get(real_row, 0),
					    glm->get(real_row, 1),
					    glm->get(real_row, 2)
					 });
			 }
			 return *parsed_value;
		 },
		 []
		 (size_t n) -> std::string
		 {
			 return std::to_string(n);
		 },
		 [container=make_weak_capture(container)]
		 (ONLY IN_THREAD, const std::optional<size_t> &value)
		 {
			 // Enable or disable the Add button depending on
			 // the results of this field's validation.

			 auto got=container.get();

			 if (!got)
				 return;

			 auto &[container]=*got;

			 x::w::gridlayoutmanager glm=
				 container->get_layoutmanager();

			 appinvoke
				 ([&]
				  (auto *app)
				  {
					  // If validation failed presume
					  // that "Add" is disabled

					  if (!value)
						  app->color_add_enable_disable
							  (IN_THREAD,
							   container,
							   glm,
							   false);
					  else
						  // Otherwise compute it
						  // the long way.
						  app->color_add_enable_disable
							  (IN_THREAD,
							   container,
							   glm);
				  });
		 });

	value->appdata=validator;

	show(value);

	validator->set(initial_value);

	// Drop-down list for the color.
	auto combo=f->valign(x::w::valign::middle)
		.create_focusable_container
		([&]
		 (const auto &c)
		 {
			 x::w::editable_comboboxlayoutmanager l=
				 c->get_layoutmanager();

			 l->append_items(IN_THREAD, existing_colors);

			 autoselect_existing_color(IN_THREAD,
						   l, ids, initial_color);

			 l->on_validate
				 ([]
				  (ONLY IN_THREAD,
				   const auto &trigger)
				  {
					  appinvoke(&appObj::color_updated,
						    IN_THREAD);

					  return true;
				  });
		 },
		 x::w::new_editable_comboboxlayoutmanager{});

	show(combo);

	// The "Delete" button

	auto delete_button=f->valign(x::w::valign::middle)
		.create_button(_("Delete"));

	show(delete_button);

	delete_button->on_activate
		([f=make_weak_capture(container,
				      delete_button)]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 auto got=f.get();

			 if (!got)
				 return;

			 auto &[container, delete_button]=*got;

			 {
				 x::w::gridlayoutmanager glm=
					 container->get_layoutmanager();

				 // Two colors, header + footer
				 if (glm->rows() <= 4)
				 {
					 container->stop_message
						 (_("Gradients must have at"
						    " least two colors"));
					 return;
				 }
				 auto rowcol=glm->lookup_row_col
					 (delete_button);

				 if (!rowcol)
					 return;

				 auto &[row, col]=*rowcol;

				 glm->remove_row(row);
			 }

			 appinvoke(&appObj::color_updated, IN_THREAD);
		 });

	// Adjust the new widgets' tabbing order.

	x::w::button add_button=glm->get(glm->rows()-1, 0);

	add_button->get_focus_before_me( {value, combo, delete_button} );

	return {value, delete_button};
}

void appObj::color_updated(ONLY IN_THREAD)
{
	colors_info_t::lock lock{colors_info};

	color_updated_locked(IN_THREAD, lock);
}

void appObj::color_updated_locked(ONLY IN_THREAD,
				  colors_info_t::lock &lock)
{
	lock->save_params.reset();
	lock->save_params.emplace();

	if (!color_updated_locked(IN_THREAD, lock, *lock->save_params))
		lock->save_params.reset();

	if (lock->current_selection)
	{
		using x::exception;

		try {
			auto generator=
				x::w::uigenerators::create(theme.get());

			auto preview_color=
				generator->lookup_color
				(lock->ids.at(lock->current_selection->index),
				 false, "current_color");
			color_preview_cell_canvas
				->set_background_color(IN_THREAD,
						       preview_color);
		} REPORT_EXCEPTIONS(main_window);
	}

	color_enable_disable_buttons(IN_THREAD, lock);
}

void appObj::color_enable_disable_buttons(ONLY IN_THREAD,
					  colors_info_t::lock &lock)
{
	enable_disable_urd(lock,
			   color_update_button,
			   color_delete_button,
			   color_reset_button);

	if (lock->current_selection && lock->save_params &&
	    *lock->save_params == *lock->current_selection)
	{
		color_preview_cell_border_container->show(IN_THREAD);
	}
	else
	{
		color_preview_cell_border_container->hide(IN_THREAD);
	}
}

namespace {
#if 0
}
#endif

// Enumerate list of gradient color/values
//
// Common code that enumerates over the input fields for linear and gradient
// colors, and retrieves the widgets for the gradient value and color.

struct parse_gradient_color_grid {

	const x::w::gridlayoutmanager glm;

	parse_gradient_color_grid(const x::w::container &list_container)
		: glm{list_container->get_layoutmanager()}
	{
	}

	typedef void callback_t(const x::w::input_field &,
				const x::w::focusable_container &c,
				const x::w::validated_input_field<size_t>);

	template<typename callback>
	void operator()(callback &&cb)
	{
		extract(x::make_function<callback_t>(std::forward<callback>
						     (cb)));
	}

	void extract(const x::function<callback_t> &cb)
	{
		size_t n=glm->rows();

		// Skip header, delete button row
		for (size_t i=1; i+1<n; ++i)
		{
			x::w::focusable_container value_field=glm->get(i, 0);

			cb(value_field,
			   glm->get(i, 1),
			   value_field->appdata);
		}
	}
};

#if 0
{
#endif
}

// Create a new loaded_color_gradient_t. Read the gradent value and color
// fields. Return true if all gradient values and colors have been validated.

static bool update_gradient_color_values(const x::w::container &list_container,
					 appObj::loaded_color_gradient_t &g)
{
	bool flag=true;

	parse_gradient_color_grid parser{list_container};

	parser([&]
	       (const auto &value_field,
		const auto &color_field,
		const auto &value_validator)
	       {
		       if (!flag)
			       return;

		       auto value=value_validator->validated_value.get();

		       if (!value)
		       {
			       flag=false;
			       return;
		       }

		       auto color=color_field->editable_combobox_get();

		       if (color.empty())
		       {
			       flag=false;
			       return;
		       }

		       g.emplace(*value, color);
	       });

	return flag;
}

// For a scaled color, enumerate:
//
// 1) The validator for the optional scaled value.
// 2) The corresponding input field.
// 3) The corresponding value in the parsed_scaled_color

static const struct {
	const x::w::validated_input_field<std::optional<double>>
	appObj::*validated_value;
	x::w::input_field appObj::*input_field;

	std::optional<double>
	x::w::ui::parsed_scaled_color::*field;
} scaled_fields[]={
	    {&appObj::color_scaled_r_validated,
	     &appObj::color_scaled_page_r,
	     &x::w::ui::parsed_scaled_color::r},
	    {&appObj::color_scaled_g_validated,
	     &appObj::color_scaled_page_g,
	     &x::w::ui::parsed_scaled_color::g},
	    {&appObj::color_scaled_b_validated,
	     &appObj::color_scaled_page_b,
	     &x::w::ui::parsed_scaled_color::b},
	    {&appObj::color_scaled_a_validated,
	     &appObj::color_scaled_page_a,
	     &x::w::ui::parsed_scaled_color::a}
};

bool appObj::color_updated_locked(ONLY IN_THREAD,
				  colors_info_t::lock &lock,
				  colors_save_params &params)
{
	// If there's no current selection we will populate color_new_name

	params.color_new_name.clear();

	if (!lock->current_selection)
	{
		x::w::input_lock lock{color_new_name};

		params.color_new_name=lock.get();

		// It better be specified.
		if (params.color_new_name.empty())
			return false;
	}

	// Scaled color

	if (color_scaled_option_radio->get_value())
	{
		// There better be a from_name

		auto &scaled_color=
			params.color_new_value
			.emplace<x::w::ui::parsed_scaled_color>();

		scaled_color.from_name=
			color_scaled_page_from->editable_combobox_get();

		if (scaled_color.from_name.empty())
			return false;

		// Pick up each scaled value.
		for (const auto &f:scaled_fields)
		{
			auto v=(this->*(f.validated_value))
				->validated_value.get();
			if (!v)
				return false;

			scaled_color.*(f.field)=*v;
		}

		return true;
	}

	// Linear gradient

	if (color_linear_gradient_option_radio->get_value())
	{
		auto &gradient_color=
			params.color_new_value
			.emplace<loaded_linear_gradient>();

		for (const auto &f:linear_gradient_color_fields)
		{
			auto v=(this->*(f.validator))
				->validated_value.get();
			if (!v)
				return false;

			gradient_color.*(f.loaded_field)=*v;
		}

		return update_gradient_color_values
			(color_linear_page_values_grid,
			 gradient_color.gradient);
	}

	// Radial gradient

	if (color_radial_gradient_option_radio->get_value())
	{
		auto &gradient_color=
			params.color_new_value
			.emplace<loaded_radial_gradient>();

		for (const auto &f:radial_gradient_color_fields)
		{
			auto v=(this->*(f.validator))
				->validated_value.get();
			if (!v)
				return false;

			gradient_color.*(f.loaded_field)=*v;
		}

		for (const auto &f:radial_gradient_color_axises)
		{
			x::w::standard_comboboxlayoutmanager lm=
				(this->*(f.combobox))->get_layoutmanager();

			auto s=lm->selected();

			if (s)
				gradient_color.*(f.field)=
					static_cast<x::w::radial_gradient_values
						    ::radius_axis>(*s);
		}
		if (!update_gradient_color_values
		    (color_radial_page_values_grid,
		     gradient_color.gradient))
			return false;
		return true;
	}

	// Must be basic color

	params.color_new_value=color_basic->current_color();
	return true;
}

// Validate all fields for the linear or radial gradient values and colors

static bool validate_gradient_color_values(ONLY IN_THREAD,
					   const x::w::container
					   &list_container)
{
	bool flag=true;

	parse_gradient_color_grid parser{list_container};

	parser([&]
	       (const auto &value_field,
		const auto &color_field,
		const auto &value_validator)
	       {
		       if (!flag)
			       return;

		       if (!value_field->validate_modified(IN_THREAD))
		       {
			       flag=false;
			       return;
		       }

		       if (color_field->editable_combobox_get().empty())
		       {
			       color_field->request_focus();
			       color_field->stop_message
				       (_("Select or enter a name"));
			       flag=false;
			       return;
		       }
	       });

	return flag;
}

// TODO: when gcc implements to_chars for doubles.

static std::string double_color(double v)
{
	std::ostringstream o;

	x::imbue o_format{x::locale::base::c(), o};

	o << std::setprecision(std::numeric_limits<x::w::rgb_component_t>
			       ::digits10+1) << v;

	return o.str();
}

static std::string fractional_color(x::w::rgb_component_t c)
{
	return double_color(c / (double)x::w::rgb::maximum);
}

namespace {
#if 0
}
#endif

// Helper visitor for writing out a new color.
//
// Factored out from color_update, for readability.
//
// This implements a visitor for loaded_color_t, that converts it to XML.
//
// Constructed with a writelock on the new <color> element where we'll
// create the XML.

struct color_update_impl {

	x::xml::writelock &doc_lock;

	void operator()(const x::w::rgb &c) const;
	void operator()(const x::w::ui::parsed_scaled_color &c) const;
	void operator()(const appObj::loaded_linear_gradient &c) const;
	void operator()(const appObj::loaded_radial_gradient &c) const;

	void gradient(const appObj::loaded_color_gradient_t &g) const;
};

void color_update_impl::operator()(const x::w::rgb &c) const
{
	doc_lock->create_child()->element({"r"})->text(fractional_color(c.r));
	doc_lock->get_parent();
	doc_lock->get_parent();
	doc_lock->create_child()->element({"g"})->text(fractional_color(c.g));
	doc_lock->get_parent();
	doc_lock->get_parent();
	doc_lock->create_child()->element({"b"})->text(fractional_color(c.b));
	doc_lock->get_parent();
	doc_lock->get_parent();
	doc_lock->create_child()->element({"a"})->text(fractional_color(c.a));
}

void color_update_impl::operator()(const x::w::ui::parsed_scaled_color &c)
	const
{
	doc_lock->create_child()->attribute({"scale",c.from_name});

	for (const auto &field:scaled_color_fields)
	{
		auto &v=c.*(field.field);

		if (!v)
			continue;

		doc_lock->create_child()->element({field.name})
			->text(double_color(*v));
		doc_lock->get_parent();
		doc_lock->get_parent();
	}
}

void color_update_impl::operator()(const appObj::loaded_linear_gradient &c)
	const
{
	doc_lock->create_child()->attribute({"type", "linear_gradient"});

	x::w::linear_gradient_values default_values;

	for (const auto &field:linear_gradient_color_fields)
	{
		auto &v=c.*(field.field);

		if (v == default_values.*(field.field))
			continue;

		doc_lock->create_child()->element({field.name})
			->text(double_color(v));
		doc_lock->get_parent();
		doc_lock->get_parent();
	}
	gradient(c.gradient);
}

void color_update_impl::operator()(const appObj::loaded_radial_gradient &c)
	const
{
	doc_lock->create_child()->attribute({"type", "radial_gradient"});

	x::w::radial_gradient_values default_values;

	for (const auto &field:radial_gradient_color_fields)
	{
		auto &v=c.*(field.field);

		if (v == default_values.*(field.field))
			continue;

		doc_lock->create_child()->element({field.name})
			->text(double_color(v));
		doc_lock->get_parent();
		doc_lock->get_parent();
	}

	for (const auto &field:radial_gradient_color_axises)
	{
		if (c.*(field.field) == default_values.*(field.field))
			continue;

		auto xml=
			doc_lock->create_child()->element({field.name});

		switch (c.*(field.field)) {
		case x::w::radial_gradient_values::horizontal:
			xml->text("horizontal");
			break;
		case x::w::radial_gradient_values::vertical:
			xml->text("vertical");
			break;
		case x::w::radial_gradient_values::shortest:
			xml->text("shortest");
			break;
		case x::w::radial_gradient_values::longest:
			xml->text("longest");
			break;
		}

		doc_lock->get_parent();
		doc_lock->get_parent();
	}
	gradient(c.gradient);
}

// Shared logic to write out the linear or color gradient
void color_update_impl::gradient(const appObj::loaded_color_gradient_t &g) const
{
	for (const auto &v:g)
	{
		doc_lock->create_child()->element({"gradient"})
			->element({"value"})
			->text(v.first);
		doc_lock->get_parent();
		doc_lock->get_parent();
		doc_lock->create_child()->element({"color"})
			->text(v.second);
		doc_lock->get_parent();
		doc_lock->get_parent();
		doc_lock->get_parent();
	}
}

#if 0
{
#endif
}

appObj::get_updatecallbackptr appObj::color_update(ONLY IN_THREAD)
{
	// Figure out which color we have currently selected, and validate
	// just those fields

	if (color_scaled_option_radio->get_value())
	{
		if (color_scaled_page_from->editable_combobox_get().empty())
		{
			color_scaled_page_from->request_focus();
			color_scaled_page_from->stop_message
				(_("Select or enter a name"));
			return nullptr;
		}

		for (const auto &scaled_field_info:scaled_fields)
		{
			if (! (this->*(scaled_field_info.input_field))
			    ->validate_modified(IN_THREAD))
				return nullptr;
		}
	}
	if (color_linear_gradient_option_radio->get_value())
	{
		for (const auto &linear_field_info
			     : linear_gradient_color_fields)
		{
			if (!(this->*(linear_field_info.input_field))
			    ->validate_modified(IN_THREAD))
				return nullptr;
		}
		if (!validate_gradient_color_values
		    (IN_THREAD, color_linear_page_values_grid))
			return nullptr;
	}
	if (color_radial_gradient_option_radio->get_value())
	{
		for (const auto &radial_field_info
			     : radial_gradient_color_fields)
		{
			if (!(this->*(radial_field_info.input_field))
			    ->validate_modified(IN_THREAD))
				return nullptr;
		}
		if (!validate_gradient_color_values
		    (IN_THREAD, color_radial_page_values_grid))
			return nullptr;
	}

	colors_info_t::lock lock{colors_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       colors_info_t::lock lock{saved_lock};

		       return me->color_update2(lock);
	       };
}

appObj::update_callback_t appObj::color_update2(colors_info_t::lock &lock)
{
	update_callback_t ret;

	if (!lock->save_params)
		return ret;

	auto &save_params=*lock->save_params;

	std::string id=save_params.color_new_name;
	bool is_new=true;

	if (lock->current_selection)
	{
		id=lock->ids.at(lock->current_selection->index);
		is_new=false;
	}

	if (std::find(x::w::rgb_color_names,
		      x::w::rgb_color_names+x::w::n_rgb_colors, id)
	    != x::w::rgb_color_names+x::w::n_rgb_colors)
	{
		main_window->stop_message(_("This is a predefined color name"));
		return ret;
	}

	// Create a new <color> node.
	auto created_update=create_update("color", id, is_new);

	if (!created_update)
		return ret;

	auto &[doc_lock, new_color]=*created_update;

	std::visit(color_update_impl{doc_lock},
		   save_params.color_new_value);

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    colors_info_t::lock lock{saved_lock};

			    me->color_update2(lock, id, is_new,
					      busy_mcguffin);
		    });

	return ret;
}

void appObj::color_update2(colors_info_t::lock &lock,
			   const std::string &id,
			   bool is_new,
			   const x::ref<x::obj> &busy_mcguffin)
{
	if (is_new)
	{
		auto n=update_new_element(id, lock->ids, color_name);

		std::vector<x::w::list_item_param>
			new_item{ {lock->ids.at(n-1)}};

		for (const auto &other:other_color_widgets)
		{
			x::w::editable_comboboxlayoutmanager lm=
				(this->*(other.widget))->get_layoutmanager();

			// n-1 is the index. There's n_rgb_colors at the
			// beginning, plus a separator line.
			lm->insert_items(n + x::w::n_rgb_colors, new_item );
		}

		status->update(_("Created new color"));
		main_window->in_thread_idle([busy_mcguffin]
					    (ONLY IN_THREAD)
					    {
					    });
		return;
	}

	main_window->in_thread
		([busy_mcguffin]
		 (ONLY IN_THREAD)
		 {
			 appinvoke(&appObj::color_update3,
				   IN_THREAD,
				   busy_mcguffin);
		 });
}

void appObj::color_update3(ONLY IN_THREAD, const x::ref<x::obj> &busy_mcguffin)
{
	colors_info_t::lock lock{colors_info};

	color_selected_locked(IN_THREAD, lock);
	status->update(_("Color updated"));

	main_window->in_thread_idle([busy_mcguffin]
				    (ONLY IN_THREAD)
				    {
				    });
}

appObj::get_updatecallbackptr appObj::color_delete(ONLY IN_THREAD)
{
	colors_info_t::lock lock{colors_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       colors_info_t::lock lock{saved_lock};

		       return me->color_delete2(lock);
	       };
}

appObj::update_callback_t appObj::color_delete2(colors_info_t::lock &lock)
{
	update_callback_t ret;

	if (!lock->current_selection)
		return ret;

	// Locate what we need to delete.
	auto index=lock->current_selection->index;
	auto id=lock->ids.at(index);

	auto new_doc=theme.get()->readlock()->clone_document();

	auto doc_lock=new_doc->writelock();

	while (1)
	{
		doc_lock->get_root();

		auto xpath=get_xpath_for(doc_lock, "color", id);

		if (xpath->count() <= 0)
			break;

		xpath->to_node(1);
		doc_lock->remove();
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    colors_info_t::lock lock{saved_lock};

			    me->color_delete2(lock, index,
					      busy_mcguffin);
		    });

	return ret;
}

void appObj::color_delete2(colors_info_t::lock &lock,
			   size_t index,
			   const x::ref<x::obj> &busy_mcguffin)
{
	// Update the loaded list of colors we store here,
	// and set the current color combo-box dropdown to "New Color".

	lock->ids.erase(lock->ids.begin()+index);

	x::w::standard_comboboxlayoutmanager name_lm=
		color_name->get_layoutmanager();

	name_lm->remove_item(index+1);

	lock->current_selection.reset();

	name_lm->autoselect(0);

	for (const auto &other:other_color_widgets)
	{
		name_lm=(this->*(other.widget))->get_layoutmanager();

		// There's n_rgb_colors standard colors at the beginning,
		// then a separator
		name_lm->remove_item(index + x::w::n_rgb_colors+1);
	}

	// autoselect(0) queues up a request.
	// When it gets processed, the color new name field gets show()n.
	// We want to request focus for that after all that processing also
	// gets done:

	main_window->in_thread_idle
		([busy_mcguffin]
		 (ONLY IN_THREAD)
		 {
			 appinvoke([&]
				   (appObj *me)
				   {
					   colors_info_t::lock
						   lock{me->colors_info};
					   me->color_reset_values(IN_THREAD,
								  lock);

					   me->color_new_name
						   ->request_focus();
					   me->status->update(_("Deleted"));

					   me->main_window->in_thread_idle
						   ([busy_mcguffin]
						    (ONLY IN_THREAD)
						    {
						    });
				   });
		 });
}
