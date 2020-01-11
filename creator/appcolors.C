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
#include "messages.H"
#include <x/messages.H>
#include <x/visitor.H>
#include <x/xml/escape.H>
#include <x/weakcapture.H>
#include <functional>

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

	x::w::editable_comboboxlayoutmanager color_scaled_page_from_lm=
		color_scaled_page_from->get_layoutmanager();

	color_scaled_page_from_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 if (info.list_item_status_info.selected)
				 appinvoke(&appObj::color_updated, IN_THREAD);
		 });
}

// Create an xpath for a particular color.
static auto get_xpath_for(const x::xml::doc::base::readlock &lock,
			  const std::string &id)
{
	return lock->get_xpath("/theme/color[@id='" +
			       x::xml::escapestr(id, true) +
			       "']");
}

void appObj::colors_initialize()
{
	colors_info_t::lock lock{colors_info};

	x::w::standard_comboboxlayoutmanager lm=
		color_name->get_layoutmanager();

	auto existing_dims=theme.get()->readlock();

	existing_dims->get_root();
	auto xpath=existing_dims->get_xpath("/theme/color");
	auto n=xpath->count();

	lock->ids.clear();
	lock->ids.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		lock->ids.push_back(existing_dims->get_any_attribute("id"));
	}

	std::sort(lock->ids.begin(), lock->ids.end());

	// Duplicate IDS: TODO - report this.

	lock->ids.erase(std::unique(lock->ids.begin(), lock->ids.end()),
			lock->ids.end());

	std::vector<x::w::list_item_param> combobox_items;

	combobox_items.reserve(lock->ids.size()+1);

	combobox_items.push_back(_("-- New Color --"));
	combobox_items.insert(combobox_items.end(),
			      lock->ids.begin(),
			      lock->ids.end());
	lm->replace_all_items(combobox_items);

	lm->autoselect(0);

	lm=color_scaled_page_from->get_layoutmanager();
	combobox_items.erase(combobox_items.begin());
	lm->replace_all_items(combobox_items);
}

void appObj::color_selected(ONLY IN_THREAD,
			    const
			    x::w::standard_combobox_selection_changed_info_t
			    &info)
{
	colors_info_t::lock lock{colors_info};

	size_t n=info.list_item_status_info.item_number;

	if (!info.list_item_status_info.selected)
	{
		// When a selection is changed, the old value is always
		// deselected first. So, hook this to clear and reset things.

		color_new_name->set("");
		color_new_name_label->hide(IN_THREAD);
		color_new_name->hide(IN_THREAD);
		return;
	}

	if (n == 0) // New color entry
	{
		lock->current_selection.reset();
		color_new_name_label->show(IN_THREAD);
		color_new_name->show(IN_THREAD);
		color_basic_option_radio->set_value(IN_THREAD, 1);
		color_reset_values(IN_THREAD, lock);
		return;
	}

	lock->current_selection.emplace();
	auto &orig_params=*lock->current_selection;
	orig_params.index=--n;

	auto current_value=theme.get()->readlock();
	current_value->get_root();

	auto xpath=get_xpath_for(current_value,
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

	color_new_name_label->hide(IN_THREAD);
	color_new_name->hide(IN_THREAD);
	color_reset_values(IN_THREAD, lock);
}

static const struct {
	std::optional<double> x::w::ui::parsed_scaled_color::*field;
	const x::w::validated_input_field<std::string> appObj::*validator;
} scaled_color_fields[]={
			 {&x::w::ui::parsed_scaled_color::r,
			  &appObj::color_scaled_r_validated},
			 {&x::w::ui::parsed_scaled_color::g,
			  &appObj::color_scaled_g_validated},
			 {&x::w::ui::parsed_scaled_color::g,
			  &appObj::color_scaled_g_validated},
			 {&x::w::ui::parsed_scaled_color::a,
			  &appObj::color_scaled_a_validated},
};

static const struct {
	double x::w::linear_gradient_values::*field;
	const x::w::validated_input_field<double> appObj::*validator;
} linear_gradient_color_fields[]={
				  {&x::w::linear_gradient_values::x1,
				   &appObj::color_linear_x1_validated},
				  {&x::w::linear_gradient_values::y1,
				   &appObj::color_linear_y1_validated},
				  {&x::w::linear_gradient_values::x2,
				   &appObj::color_linear_x2_validated},
				  {&x::w::linear_gradient_values::y2,
				   &appObj::color_linear_y2_validated},
				  {&x::w::linear_gradient_values::fixed_width,
				   &appObj::color_linear_width_validated},
				  {&x::w::linear_gradient_values::fixed_height,
				   &appObj::color_linear_height_validated}};

static const struct {
	double x::w::radial_gradient_values::*field;
	const x::w::validated_input_field<double> appObj::*validator;
} radial_gradient_color_fields[]={
				  {&x::w::radial_gradient_values::inner_center_x,
				   &appObj::color_radial_inner_x_validated},
				  {&x::w::radial_gradient_values::inner_center_y,
				   &appObj::color_radial_inner_y_validated},
				  {&x::w::radial_gradient_values::inner_radius,
				   &appObj::color_radial_inner_radius_validated},
				  {&x::w::radial_gradient_values::outer_center_x,
				   &appObj::color_radial_outer_x_validated},
				  {&x::w::radial_gradient_values::outer_center_y,
				   &appObj::color_radial_outer_y_validated},
				  {&x::w::radial_gradient_values::outer_radius,
				   &appObj::color_radial_outer_radius_validated},
				  {&x::w::radial_gradient_values::fixed_width,
				   &appObj::color_radial_fixed_width_validated},
				  {&x::w::radial_gradient_values::fixed_height,
				   &appObj::color_radial_fixed_height_validated},
};

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
		if (lock->current_selection &&
		    lock->current_selection->index < ids.size())
			ids.erase(ids.begin() + lock->current_selection->index);

		existing_colors={ ids.begin(), ids.end() };
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
	add(const x::w::container &container,
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

	reset_linear_gradient.gradient=
		reset_radial_gradient.gradient={{0, "50%"},
						{100, "100%"}};

	// Check the currently selected color type, and have it update one
	// of the above defaults.

	auto selected_option=color_basic_option_radio;

	if (lock->current_selection)
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

	// We can now use the defaults to populate each color type's page.
	//
	// First, the basic color.
	color_basic->current_color(IN_THREAD, reset_rgb_color);

	// Now the scaled color.

	{
		x::w::editable_comboboxlayoutmanager lm=
			color_scaled_page_from->get_layoutmanager();

		x::w::input_lock scaled_color_lock{lm};

		scaled_color_lock.locked_input_field
			->set(IN_THREAD, reset_scaled_color.from_name);

		if (reset_scaled_color.from_name.empty())
		{
			lm->unselect(IN_THREAD);
		}
		else
		{
			auto iter=std::lower_bound
				(lock->ids.begin(),
				 lock->ids.end(),
				 reset_scaled_color.from_name);

			if (iter != lock->ids.end() &&
			    *iter == reset_scaled_color.from_name)
			{
				lm->autoselect(IN_THREAD,
					       iter-lock->ids.begin(), {});
			}
		}

		for (const auto &field:scaled_color_fields)
		{
			std::string s;

			if (reset_scaled_color.*(field.field))
				s=fmtdblval(*(reset_scaled_color.*
					      (field.field)));

			(this->*(field.validator))->set(s);
		}
	}

	// The linear gradient

	for (const auto &f:linear_gradient_color_fields)
		(this->*(f.validator))->set(reset_linear_gradient.*(f.field));

	// The radial gradient
	for (const auto &f:radial_gradient_color_fields)
		(this->*(f.validator))->set(reset_radial_gradient.*(f.field));

	{
		x::w::standard_comboboxlayoutmanager lm=
			color_radial_inner_axis->get_layoutmanager();
		lm->autoselect(IN_THREAD, static_cast<size_t>
			       (reset_radial_gradient.inner_radius_axis), {});
	}

	{
		x::w::standard_comboboxlayoutmanager lm=
			color_radial_outer_axis->get_layoutmanager();
		lm->autoselect(IN_THREAD, static_cast<size_t>
			       (reset_radial_gradient.outer_radius_axis), {});
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
				create_gradient_row.add(values.list_container,
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
	selected_option->set_value(IN_THREAD, 1);
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
		create_gradient_row.add(container, glm,
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

		x::w::validated_input_field<size_t> validator=f->appdata;

		auto v=validator->validated_value.get();

		if (!v)
			return false;

		parser(*v,
		       // Cell 1 is the combo-box with the color's value.

		       x::w::input_lock{
			       x::w::editable_comboboxlayoutmanager{
				       x::w::focusable_container{glm->get(i, 1)}
				       ->get_layoutmanager()
						 }}.get());
	}

	return true;
}

void appObj::color_add_enable_disable(ONLY IN_THREAD,
				      const x::w::container &container,
				      const x::w::gridlayoutmanager &glm,
				      bool enable_disable)
{
	x::w::button b=glm->get(glm->rows()-1, 0);

	b->set_enabled(IN_THREAD, enable_disable);
	color_updated(IN_THREAD);
}

std::tuple<x::w::input_field, x::w::button>
appObj::color_create_gradient_row::add(const x::w::container &container,
				       const x::w::gridlayoutmanager &glm,
				       const x::w::gridfactory &f,
				       std::optional<size_t> initial_value,
				       const std::string &initial_color)
{
	x::w::input_field_config if_config;

	if_config.columns=6;
	if_config.alignment=x::w::halign::right;
	auto value=f->valign(x::w::valign::middle)
		.create_input_field("", if_config);

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

			 // skip this row
			 auto rowcol=glm->lookup_row_col(field);

			 if (!rowcol)
				 return *parsed_value;

			 auto &[row, col]=*rowcol;

			 size_t real_row=1;

			 for (size_t i=1, n=glm->rows()-1; i<n; ++i)
			 {
				 if (i == row)
					 continue;

				 x::w::input_field f=glm->get(i, 0);

				 x::w::validated_input_field<size_t>
					 validator=f->appdata;

				 auto v=validator->validated_value.get();

				 if (!v)
					 continue;
				 if (*v == *parsed_value)
				 {
					 field->stop_message
						 (_("All gradient values must"
						    " be unique"));
					 return std::nullopt;
				 }

				 if (*v < *parsed_value)
					 real_row=i+1;
			 }

			 std::cout << "Insert " << real_row << std::endl;
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

			 l->append_items(existing_colors);

			 auto b=ids.begin();
			 auto e=ids.end();
			 auto iter=std::lower_bound(b, e, initial_color);

			 if (iter != ids.end() && *iter == initial_color)
			 {
				 l->autoselect(iter-b);
			 }
			 else if (!initial_color.empty())
			 {
				 x::w::input_lock i_lock{l};
				 i_lock.locked_input_field
					 ->set(initial_color);
			 }

			 x::w::input_field f=l->current_selection();
			 f->on_validate
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
	static int counter=0;

	std::cout << "Color updated " << ++counter << std::endl;
}
