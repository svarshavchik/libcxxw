#include "libcxxw_config.h"
#include "creator/app.H"
#include "x/w/uielements.H"
#include "x/w/impl/uixmlparser.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "messages.H"
#include "catch_exceptions.H"
#include <x/messages.H>
#include <x/strtok.H>
#include <x/to_string.H>
#include <x/weakcapture.H>
#include <x/mpthreadlock.H>
#include <sstream>

#include "fonts/fontconfig.H"
#include "fonts/fontlist.H"
#include "fonts/fontpattern.H"
#include <fontconfig/fontconfig.h>

// Same order as the corresponding font_size_type dropdown

static const struct {
	x::w::font &(x::w::font::*method)(double);
	char xml_element[12];
	const char *label;
} size_type_options[]=
	{
	 { &x::w::font::set_point_size, "point_size", "Point Size" },
	 { &x::w::font::set_scaled_size, "scaled_size", "Scaled Size (SXG)" },
	 { &x::w::font::scale, "scale", "Scale" }
	};

// All our editable combo-boxes

// ... except point size, which has a double validator attached to it.

static const struct {
	x::w::focusable_container appObj::*app_field;
	x::w::focusable_containerptr app_elements_tptr::*init_field;

	x::w::font::values_t standard_font_values_t::*preset_values;

	std::string appObj::loaded_font_t::*loaded_font_field;
} font_editable_comboboxes[]=
	{
	 {
	  &appObj::font_from_name,
	  &app_elements_tptr::font_from_name,
	  nullptr,
	  &appObj::loaded_font_t::from,
	 },
	 {
	  &appObj::font_family,
	  &app_elements_tptr::font_family,
	  nullptr,
	  &appObj::loaded_font_t::family,
	 },
	 {
	  &appObj::font_weight,
	  &app_elements_tptr::font_weight,
	  &standard_font_values_t::standard_weights,
	  &appObj::loaded_font_t::weight,
	 },
	 {
	  &appObj::font_spacing,
	  &app_elements_tptr::font_spacing,
	  &standard_font_values_t::standard_spacings,
	  &appObj::loaded_font_t::spacing,
	 },
	 {
	  &appObj::font_slant,
	  &app_elements_tptr::font_slant,
	  &standard_font_values_t::standard_slants,
	  &appObj::loaded_font_t::slant,
	 },
	 {
	  &appObj::font_width,
	  &app_elements_tptr::font_width,
	  &standard_font_values_t::standard_widths,
	  &appObj::loaded_font_t::width,
	 },
	};

// Our input fields.
static const struct {
	x::w::input_field appObj::*app_field;
	x::w::input_fieldptr app_elements_tptr::*init_field;
	std::string appObj::loaded_font_t::*loaded_font_field;
	char xml_element[8];
} font_freeform_input_fields[]=
	{
	 {
	  &appObj::font_foundry,
	  &app_elements_tptr::font_foundry,
	  &appObj::loaded_font_t::foundry,
	  "foundry",
	 },
	 {
	  &appObj::font_style,
	  &app_elements_tptr::font_style,
	  &appObj::loaded_font_t::style,
	  "style",
	 },
	};

// Fields except for the "from".

static const struct {

	x::w::focusable_container appObj::*editable_combobox;
	std::string appObj::loaded_font_t::*field;
	x::w::font::values_t standard_font_values_t::*standard_values;
	char xml_element[8];

} loaded_string_fields[] =
	{
	 {&appObj::font_family,  &appObj::loaded_font_t::family, nullptr,
	  "family"},
	 {&appObj::font_weight,  &appObj::loaded_font_t::weight,
	  &standard_font_values_t::standard_weights, "weight"},
	 {&appObj::font_spacing, &appObj::loaded_font_t::spacing,
	  &standard_font_values_t::standard_spacings, "spacing"},
	 {&appObj::font_slant,   &appObj::loaded_font_t::slant,
	  &standard_font_values_t::standard_slants, "slant"},
	 {&appObj::font_width,   &appObj::loaded_font_t::width,
	  &standard_font_values_t::standard_widths, "width"},
	};

// A combo-box for a font values that has several standard defined values.
//
// Initialize it.
static void initialize_standard_font_values(const x::w::focusable_container &c,
					    const x::w::font::values_t &v)
{
	std::vector<x::w::list_item_param> params;

	params.reserve(v.size());

	for (const auto &vv:v)
		params.push_back(vv.description);

	c->editable_comboboxlayout()->replace_all_items(params);
}

// Invoke enable_disable() later
//
//
// Return immediately, releasing the layout manager
// lock, and reenter the app with a fresh environment.

void appObj::font_enable_disable_later(ONLY IN_THREAD)
{
	appinvoke([]
		  (auto *me)
		  {
			  font_info_t::lock lock{me->font_info};

			  if (lock->enable_disable_scheduled)
				  return;

			  lock->enable_disable_scheduled=true;

			  me->main_window->in_thread
				  ([]
				   (ONLY IN_THREAD)
				   {
					   font_enable_disable_now(IN_THREAD);
				   });
		  });
}

void appObj::font_enable_disable_now(ONLY IN_THREAD)
{
	appinvoke([&]
		  (appObj *me)
		  {
			  font_info_t::lock lock{me->font_info};

			  lock->enable_disable_scheduled=false;

			  me->font_enable_disable_buttons(IN_THREAD, lock);

		  });
}

void appObj::font_enable_disable(ONLY IN_THREAD)
{
	font_info_t::lock lock{font_info};

	font_enable_disable_buttons(IN_THREAD, lock);
}

// Install callbacks for editable combo-boxes on the font page.

static void font_editable_combobox(const x::w::focusable_container &container)
{
	auto lm=container->editable_comboboxlayout();

	lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appObj::font_enable_disable_later(IN_THREAD);
		 });

	lm->on_validate
		([]
		 (ONLY IN_THREAD,
		  const auto &)
		 {
			 appObj::font_enable_disable_later(IN_THREAD);

			 return true;
		 });
}

void appObj::fonts_elements_initialize(app_elements_tptr &elements,
					 x::w::uielements &ui,
					 init_args &args)
{
	x::w::focusable_container font_name=
		ui.get_element("font_name_field");
	x::w::label font_new_name_label=
		ui.get_element("font_new_name_label");
	x::w::input_field font_new_name=
		ui.get_element("font_new_name_field");
	x::w::focusable_container font_from_name=
		ui.get_element("font_from_name");
	x::w::standard_comboboxlayoutmanager font_name_lm=
		font_name->get_layoutmanager();

	x::w::focusable_container font_family=
		ui.get_element("font_family");

	// Use fontconfig to get a list of available font families
	{
		std::set<std::string> families;

		auto list=x::w::fontconfig::config::create()->create_list();

		for (auto b=list->begin(), e=list->end(); b != e; ++b)
		{
			std::string family;

			if (!(*b)->get_string(FC_FAMILY, family))
				continue;
			families.insert(family);
		}

		std::vector<x::w::list_item_param> items{ families.begin(),
				families.end() };

		font_family->listlayout()->replace_all_items(items);
	}

	// How the font size gets specified.
	x::w::focusable_container font_size_type=
		ui.get_element("font_size_type");

	auto font_size_type_lm=font_size_type->standard_comboboxlayout();

	{
		std::vector<x::w::list_item_param> items;

		items.reserve(sizeof(size_type_options)
			      /sizeof(size_type_options[0]));

		for (const auto &o:size_type_options)
			items.push_back(o.label);

		font_size_type_lm->replace_all_items(items);
	}

	// Font size combo-box gets a default list of standard font sizes.
	x::w::focusable_container font_size=
		ui.get_element("font_size");

	{
		std::vector<x::w::list_item_param> items;

		items.reserve(args.standard_point_sizes.size());

		for (auto ps:args.standard_point_sizes)
			items.push_back(x::to_string(ps, x::locale::base::c()));

		font_size->editable_comboboxlayout()->replace_all_items(items);
	}
	x::w::input_field font_foundry=
		ui.get_element("font_foundry");

	x::w::input_field font_style=
		ui.get_element("font_style");

	x::w::focusable_container font_weight=
		ui.get_element("font_weight");

	x::w::focusable_container font_spacing=
		ui.get_element("font_spacing");

	x::w::focusable_container font_slant=
		ui.get_element("font_slant");

	x::w::focusable_container font_width=
		ui.get_element("font_width");

	x::w::container font_preview_container=
		ui.get_element("font_preview_container");
	x::w::font_picker_preview font_preview=
		ui.get_element("font_preview");
	x::w::button font_update_button=
		ui.get_element("font_update_button");
	x::w::button font_reset_button=
		ui.get_element("font_reset_button");
	x::w::button font_delete_button=
		ui.get_element("font_delete_button");

	// Install callbacks.

	font_new_name->on_filter(get_label_filter());

	font_new_name->on_validate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger)
		 {
			 appObj::font_enable_disable_later(IN_THREAD);
			 return true;
		 });

	font_name_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::font_selected, IN_THREAD,
				   info);
		 });

	// Save our elements
	elements.font_name=font_name;
	elements.font_new_name_label=font_new_name_label;
	elements.font_new_name=font_new_name;
	elements.font_from_name=font_from_name;

	elements.font_family=font_family;
	elements.font_size_type=font_size_type;
	elements.font_size=font_size;
	elements.font_foundry=font_foundry;
	elements.font_style=font_style;
	elements.font_weight=font_weight;
	elements.font_spacing=font_spacing;
	elements.font_slant=font_slant;
	elements.font_width=font_width;

	// Use the convenient arrays to attach callbacks.

	for (const auto &combo:font_editable_comboboxes)
		font_editable_combobox(elements.*(combo.init_field));

	initialize_standard_font_values(font_weight, args.standard_weights);
	initialize_standard_font_values(font_spacing, args.standard_spacings);
	initialize_standard_font_values(font_slant, args.standard_slants);
	initialize_standard_font_values(font_width, args.standard_widths);

	for (const auto &info:font_freeform_input_fields)
	{
		(elements.*(info.init_field))->on_validate
			([]
			 (ONLY IN_THREAD,
			  const auto &trigger)
			 {
				 appObj::font_enable_disable_later(IN_THREAD);
				 return true;
			 });
	}
	font_size_type_lm->on_selection_changed
		([&]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 if (!info.list_item_status_info.selected)
				 return;

			 font_enable_disable_later(IN_THREAD);
		 });

	elements.font_preview_container=
		font_preview_container;
	elements.font_preview=font_preview;
	elements.font_update_button=font_update_button;
	elements.font_reset_button=font_reset_button;
	elements.font_delete_button=font_delete_button;

	font_update_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::font_update);
		 });

	font_reset_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 appinvoke(&appObj::font_reset, IN_THREAD);
		 });

	font_delete_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::font_delete);
		 });

}

void appObj::fonts_initialize(ONLY IN_THREAD)
{
	font_info_t::lock lock{font_info};

	fonts_initialize(IN_THREAD, lock);
	font_reset_values(IN_THREAD, lock);
}

void appObj::fonts_initialize(ONLY IN_THREAD, font_info_t::lock &lock)
{
	x::w::standard_comboboxlayoutmanager lm=
		font_name->get_layoutmanager();

	auto existing_fonts=theme.get()->readlock();

	existing_fonts->get_root();
	auto xpath=existing_fonts->get_xpath("/theme/font");
	auto n=xpath->count();

	lock->ids.clear();
	lock->ids.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		lock->ids.push_back(existing_fonts->get_any_attribute("id"));
	}

	std::sort(lock->ids.begin(), lock->ids.end());

	// Duplicate IDS: TODO - report this.

	lock->ids.erase(std::unique(lock->ids.begin(), lock->ids.end()),
			lock->ids.end());

	std::vector<x::w::list_item_param> combobox_items;

	combobox_items.reserve(lock->ids.size()+1);

	combobox_items.push_back(_("-- New Font --"));
	combobox_items.insert(combobox_items.end(),
			      lock->ids.begin(),
			      lock->ids.end());
	lm->replace_all_items(IN_THREAD, combobox_items);
	lm->autoselect(IN_THREAD, 0, {});

	// The same list goes into the from name dropdown.

	combobox_items.erase(combobox_items.begin());

	{
		x::w::editable_comboboxlayoutmanager from_lm=
			font_from_name->get_layoutmanager();
		from_lm->replace_all_items(IN_THREAD, combobox_items);
	}
	font_enable_disable_buttons(IN_THREAD, lock);
}

namespace {
#if 0
}
#endif

// Implement UI font parse to store the loaded data in loaded_font_t

struct parse_font : x::w::ui::parse_font {

	appObj * const me;
	appObj::font_info_t::lock &lock;

	parse_font(appObj * me,
		   appObj::font_info_t::lock &lock)
		: me{me}, lock{lock}
	{
	}

	inline auto &loaded_font()
	{
		return lock->current_selection.value().loaded_font;
	}

	void set_point_size(double v) override
	{
		auto &loaded_font=this->loaded_font();

		loaded_font.size=v;
		loaded_font.size_type=appObj::font_size_type_t::point_size;
	}

	void set_scaled_size(double v) override
	{
		auto &loaded_font=this->loaded_font();

		loaded_font.size=v;
		loaded_font.size_type=appObj::font_size_type_t::scaled_size;
	}

	void scale(double v) override
	{
		auto &loaded_font=this->loaded_font();

		loaded_font.size=v;
		loaded_font.size_type=appObj::font_size_type_t::scale;
	}

	void set_from(const std::string &v) override
	{
		loaded_font().from=v;
	}

	void set_family(const std::string &v) override
	{
		loaded_font().family=v;
	}

	void set_foundry(const std::string &v) override
	{
		loaded_font().foundry=v;
	}

	void set_style(const std::string &v) override
	{
		loaded_font().style=v;
	}

	void set_weight(const std::string &v) override
	{
		loaded_font().weight=v;
	}

	void set_spacing(const std::string &v) override
	{
		loaded_font().spacing=v;
	}

	void set_slant(const std::string &v) override
	{
		loaded_font().slant=v;
	}

	void set_width(const std::string &v) override
	{
		loaded_font().width=v;
	}

};

#if 0
{
#endif
}

void appObj::font_selected(ONLY IN_THREAD,
			     const
			     x::w::standard_combobox_selection_changed_info_t
			     &info)
{
	font_info_t::lock lock{font_info};

	size_t n=info.list_item_status_info.item_number;

	if (!info.list_item_status_info.selected)
	{
		// When a selection is changed, the old value is always
		// deselected first. So, hook this to clear and reset all
		// fields.

		font_new_name->set("");

		auto lm=font_from_name->editable_comboboxlayout();

		// Reenable the old font in the "from" name dropdown,
		// since it can now be used as "from".

		if (n > 0)
			lm->enabled(IN_THREAD, --n, true);

		return;
	}

	lock->current_selection.reset();

	if (n == 0) // New font entry
	{
		font_selected_locked(IN_THREAD, lock);
		return;
	}

	--n;

	auto lm=font_from_name->standard_comboboxlayout();

	// A new font is selected.
	//
	// Disable the corresponding item in the font_from_name
	// combo-box.

	lm->enabled(IN_THREAD, n, false);

	lock->current_selection.emplace();
	lock->current_selection->index=n;
	font_selected_locked(IN_THREAD, lock);
}

void appObj::font_reset(ONLY IN_THREAD)
{
	font_info_t::lock lock{font_info};

	font_selected_locked(IN_THREAD, lock);

	if (lock->current_selection)
		font_name->request_focus();
	else
		font_new_name->request_focus();
}

void appObj::font_selected_locked(ONLY IN_THREAD,
				  font_info_t::lock &lock)
{
	if (!lock->current_selection)
	{
		font_new_name_label->show(IN_THREAD);
		font_new_name->show(IN_THREAD);
		font_new_name->set(IN_THREAD, "");
		font_reset_values(IN_THREAD, lock);
		return;
	}

	auto &orig_params=*lock->current_selection;
	auto n=orig_params.index;

	auto current_value=theme.get()->readlock();
	current_value->get_root();

	auto xpath=get_xpath_for(current_value,
				 "font",
				 lock->ids.at(n));

	// We expect one to be there, of course.
	xpath->to_node(1);

	typedef x::exception exception;
	try {
		orig_params.loaded_font=loaded_font_t{};

		parse_font parser{this, lock};

		parser.parse(current_value, lock->ids.at(n) );
	} REPORT_EXCEPTIONS(main_window);

	font_new_name_label->hide(IN_THREAD);
	font_new_name->hide(IN_THREAD);

	font_reset_values(IN_THREAD, lock);
}

void appObj::font_reset_values(ONLY IN_THREAD,
			       font_info_t::lock &lock)
{
	busy();

	// Clear everything.

	auto font_from_name_lm=font_from_name->editable_comboboxlayout();

	font_from_name_lm->unselect(IN_THREAD);

	font_from_name_lm->set(IN_THREAD, "");

	// Reset the "From" font

	// Note: clearing the input field is not enough, we need to unselect
	// too, because if we don't, and the previous selection gets selected
	// again, the editable combo-box does not fire.

	if (!lock->current_selection)
	{
		for (const auto &combo_boxes:font_editable_comboboxes)
		{
			(this->*(combo_boxes.app_field))
				->editable_comboboxlayout()
				->set(IN_THREAD, "", true);
		}

		font_size_type->standard_comboboxlayout()
			->autoselect(IN_THREAD, 0, {});

		font_point_size_validated->set(IN_THREAD, std::nullopt);

		for (const auto &ifield:font_freeform_input_fields)
		{
			(this->*(ifield.app_field))->set(IN_THREAD, "", true);
		}
	}
	else
	{
		auto &loaded_font=lock->current_selection->loaded_font;

		if (!loaded_font.from.empty())
		{
			auto b=lock->ids.begin();
			auto e=lock->ids.end();

			auto iter=std::find(b, e, loaded_font.from);

			if (iter == e) // ?
			{
				font_from_name_lm
					->set(IN_THREAD, loaded_font.from,
					      true);
			}
			else
			{
				font_from_name_lm->autoselect(IN_THREAD,
							      iter-b,
							      {});
			}
		}
		else
		{
			font_from_name_lm->set(IN_THREAD, "", true);
		}

		// For the combo-boxes with preset values, if the
		// font's value matches one of the preset values we will
		// select the corresponding item in the combo-box, otherwise
		// we'll just set the input field, in the editable combo-box,
		// to whatever it is.

		for (const auto &f:loaded_string_fields)
		{
			auto lm=(this->*(f.editable_combobox)
				 )->editable_comboboxlayout();

			auto value=loaded_font.*(f.field);

			bool selected=false;
			if (f.standard_values)
			{
				auto b=(standard_font_values
					.*(f.standard_values)).begin();
				auto e=(standard_font_values
					.*(f.standard_values)).end();

				auto iter=std::find_if
					(b, e,
					 [&]
					 (const auto &v)
					 {
						 return value == v.label;
					 });

				if (iter != e)
				{
					lm->autoselect(IN_THREAD, iter-b,
						       {});
					selected=true;
				}
			}

			if (!selected)
				lm->set(IN_THREAD, value, true);
		}

		// We have a validator insalled for the font size field, use it.
		//
		// Then set the pair of free-form input fields.

		font_point_size_validated->set(IN_THREAD,
					       loaded_font.size);
		font_size_type->standard_comboboxlayout()
			->autoselect(IN_THREAD,
				     static_cast<size_t>(loaded_font.size_type),
				     {});

		for (const auto &f:font_freeform_input_fields)
		{
			(this->*(f.app_field))
				->set(IN_THREAD,
				      loaded_font.*(f.loaded_font_field), true);
		}
	}

	font_enable_disable_buttons(IN_THREAD, lock);
}

void appObj::font_enable_disable_buttons(ONLY IN_THREAD,
					 font_info_t::lock &lock)
{
	// Now, read the state of all widgets and create new save_params

	lock->save_params.reset();
	lock->save_params.emplace();

	auto &save_params=*lock->save_params;

	bool good_save_params=true;

	// Parse the From name.

	if (!lock->current_selection)
	{
		save_params.font_new_name=
			x::trim(x::w::input_lock{font_new_name}.get());

		if (save_params.font_new_name.empty())
		{
			good_save_params=false;
		}
	}

	save_params.font_new_value.from=
		x::trim(font_from_name->editable_combobox_get());

	// Read what's in the editable combo-boxes.

	for (const auto &combo:font_editable_comboboxes)
	{
		auto value=(this->*(combo.app_field))->editable_combobox_get();

		// If this one has preset values and the entered field matches
		// one of the preset values we will use the preset value's
		// label. I.e. "Monospace" => "monospace"

		if (combo.preset_values)
		{
			auto b=(standard_font_values
				.*(combo.preset_values)).begin();
			auto e=(standard_font_values
				.*(combo.preset_values)).end();

			auto iter=std::find_if
				(b, e,
				 [&]
				 (const auto &v)
				 {
					 return value == v.description;
				 });

			if (iter != e)
				value=iter->label;
		}

		save_params.font_new_value.*(combo.loaded_font_field)=value;
	}

	for (const auto &ifield:font_freeform_input_fields)
	{
		auto value=x::w::input_lock{(this->*(ifield.app_field))}.get();

		save_params.font_new_value.*(ifield.loaded_font_field)=
			x::trim(value);
	}

	auto st=font_size_type->listlayout()->selected();
	if (!st)
	{
		// Shouldn't happen, this standard combo-box should always
		// have something selected.
		good_save_params=false;
	}
	else
	{
		save_params.font_new_value.size_type=
			static_cast<font_size_type_t>(*st);
	}

	auto validated_size=
		font_point_size_validated->validated_value.get();

	if (!validated_size)
	{
		// Validation failure.
		good_save_params=false;
	}
	else
	{
		// Good validation, including nothing entered.
		save_params.font_new_value.size=*validated_size;
	}

	if (!good_save_params)
	{
		lock->save_params.reset();
	}
	enable_disable_urd(lock,
			   font_update_button,
			   font_delete_button,
			   font_reset_button);

	// Try to preview the font.

	if (lock->current_selection && lock->save_params &&
	    *lock->save_params == *lock->current_selection)
	{
		using x::exception;

		font_preview_container->hide(IN_THREAD);

		try {
			auto generator=
				x::w::uigenerators::create(theme.get());

			auto preview_font=
				generator->lookup_font
				(lock->ids.at(lock->current_selection->index),
				 true, "current_font");

			font_preview->update_preview(IN_THREAD, preview_font);
			font_preview_container->show_all(IN_THREAD);
		} REPORT_EXCEPTIONS(main_window);
	}
	else
	{
		font_preview_container->hide(IN_THREAD);
	}
}

appObj::get_updatecallbackptr appObj::font_update(ONLY IN_THREAD)
{
	font_info_t::lock lock{font_info};

	if (!lock->save_params)
		return nullptr;

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       font_info_t::lock lock{saved_lock};

		       return me->font_update2(lock);
	       };
}

appObj::update_callback_t appObj::font_update2(font_info_t::lock &lock)
{
	appObj::update_callback_t ret;

	if (!lock->save_params)
		return ret; // Shouldn't happen.

	auto &save_params=*lock->save_params;

	std::string id=save_params.font_new_name;
	bool is_new=true;

	if (lock->current_selection)
	{
		id=lock->ids.at(lock->current_selection->index);
		is_new=false;
	}

	auto created_update=create_update("font", id, is_new);

	if (!created_update)
		return ret;

	auto &[doc_lock, new_font]=*created_update;

	auto &new_value=save_params.font_new_value;

	if (!new_value.from.empty())
	{
		doc_lock->create_child()->element({"from"})
			->text(new_value.from)
			->parent()->parent();
	}

	if (new_value.size && static_cast<size_t>(new_value.size_type)
	    < sizeof(size_type_options)/sizeof(size_type_options[0]))
	{
		auto element=
			size_type_options[static_cast<size_t>
					  (new_value.size_type)]
			.xml_element;

		doc_lock->create_child()->element({element})
			->text(fmtdblval(*new_value.size))
			->parent()->parent();
	}

	for (const auto &f:loaded_string_fields)
	{
		auto &s=new_value.*(f.field);

		if (s.empty())
			continue;
		doc_lock->create_child()->element({f.xml_element})
			->text(s)->parent()->parent();
	}

	for (const auto &f:font_freeform_input_fields)
	{
		auto &s=new_value.*(f.loaded_font_field);

		if (s.empty())
			continue;
		doc_lock->create_child()->element({f.xml_element})
			->text(s)->parent()->parent();
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    font_info_t::lock lock{saved_lock};

			    me->font_update3(IN_THREAD, lock, id, is_new,
					     busy_mcguffin);
		    });

	return ret;
}

void appObj::font_update3(ONLY IN_THREAD,
			  font_info_t::lock &lock,
			  const std::string &id,
			  bool is_new,
			  const x::ref<x::obj> &busy_mcguffin)
{
	if (is_new)
	{
		auto from_name_lm=font_from_name->standard_comboboxlayout();

		update_new_element(IN_THREAD,
				   {id}, lock->ids, font_name,
				   [&]
				   (size_t i)
				   {
					   from_name_lm->insert_items(
						   IN_THREAD,
						   i-1,
						   {id}
					   );
				   });

		status->update(_("Created new font"));
		return;
	}

	font_selected_locked(IN_THREAD, lock);
	status->update(_("Font updated"));
}

appObj::get_updatecallbackptr appObj::font_delete(ONLY IN_THREAD)
{
	font_info_t::lock lock{font_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       font_info_t::lock lock{saved_lock};

		       return me->font_delete2(lock);
	       };
}

appObj::update_callback_t appObj::font_delete2(font_info_t::lock &lock)
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

		auto xpath=get_xpath_for(doc_lock, "font", id);

		if (xpath->count() <= 0)
			break;

		xpath->to_node(1);
		doc_lock->remove();
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    font_info_t::lock lock{saved_lock};

			    me->font_delete3(IN_THREAD, lock, index,
					     busy_mcguffin);
		    });

	return ret;
}

void appObj::font_delete3(ONLY IN_THREAD, font_info_t::lock &lock,
			  size_t index,
			  const x::ref<x::obj> &busy_mcguffin)
{
	// Update the loaded list of fonts we store here,
	// and set the current font combo-box dropdown to "New Font".

	lock->ids.erase(lock->ids.begin()+index);

	auto name_lm=font_name->standard_comboboxlayout(),
		from_name_lm=font_from_name->standard_comboboxlayout();

	name_lm->remove_item(IN_THREAD, index+1);
	from_name_lm->remove_item(IN_THREAD, index);

	lock->current_selection.reset();

	name_lm->autoselect(IN_THREAD, 0, {});

	font_reset_values(IN_THREAD, lock);

	font_new_name->request_focus(IN_THREAD);
	status->update(_("Deleted"));
}
