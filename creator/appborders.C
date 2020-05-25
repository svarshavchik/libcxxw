#include "libcxxw_config.h"
#include "creator/app.H"
#include "x/w/uielements.H"
#include "x/w/impl/uixmlparser.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "messages.H"
#include "catch_exceptions.H"
#include <x/messages.H>
#include <x/strtok.H>
#include <x/weakcapture.H>
#include <x/mpthreadlock.H>
#include <sstream>

namespace {
#if 0
}
#endif

// Values in loaded_border_t and the corresponding widgets.

static const struct {
	char name[8];
	std::string appObj::loaded_border_t::*value;
	x::w::focusable_container appObj::*field;
	const x::w::validated_input_field<std::variant<std::string, double>
					  > appObj::*validated_field;
} all_border_sizes[]=
	{
	 { "width",
	   &appObj::loaded_border_t::width, &appObj::border_width,
	   &appObj::border_width_validated,
	 },
	 { "height",
	   &appObj::loaded_border_t::height,  &appObj::border_height,
	   &appObj::border_height_validated,
	 },
	 { "hradius",
	   &appObj::loaded_border_t::hradius, &appObj::border_hradius,
	   &appObj::border_hradius_validated,
	 },
	 { "vradius",
	   &appObj::loaded_border_t::vradius, &appObj::border_vradius,
	   &appObj::border_vradius_validated,
	 },
	};

static const struct {
	char name[14];
	std::optional<unsigned> appObj::loaded_border_t::*value;
	x::w::input_field appObj::*field;
	const x::w::validated_input_field<std::optional<unsigned>
				    > appObj::*validated_field;
} all_border_size_scales[]=
	{
	 { "width_scale",
	   &appObj::loaded_border_t::width_scale,
	   &appObj::border_width_scale,
	   &appObj::border_width_scale_validated,
	 },
	 { "height_scale",
	   &appObj::loaded_border_t::height_scale,
	   &appObj::border_height_scale,
	   &appObj::border_height_scale_validated,
	 },
	 { "hradius_scale",
	   &appObj::loaded_border_t::hradius_scale,
	   &appObj::border_hradius_scale,
	   &appObj::border_hradius_scale_validated,
	 },
	 { "vradius_scale",
	   &appObj::loaded_border_t::vradius_scale,
	   &appObj::border_vradius_scale,
	   &appObj::border_vradius_scale_validated,
	 },
};

#if 0
{
#endif
}

// UI callback to invoke border_enable_disable_buttons(), in the app.

void appObj::border_enable_disable(ONLY IN_THREAD)
{
	appinvoke([&]
		  (auto *app)
		  {
			  appObj::border_info_t::lock
				  lock{app->border_info};

			  app->border_enable_disable_buttons(IN_THREAD,
							     lock);
		  });
}

// Invoke enable_disable() later
//
//
// Return immediately, releasing the layout manager
// lock, and reenter the app with a fresh environment.

void appObj::border_enable_disable_later()
{
	appinvoke([]
		  (auto *app)
		  {
			  app->main_window->in_thread
				  ([]
				   (ONLY IN_THREAD)
				   {
					   border_enable_disable(IN_THREAD);
				   });
		  });
}

// An image button was clicked here.

static void border_image_button(const x::w::image_button &button)
{
	button->on_activate
		([]
		 (ONLY IN_THREAD,
		  size_t,
		  const auto &trigger,
		  const auto &busy)
		 {
			 if (trigger.index() == x::w::callback_trigger_initial)
				 return;

			 appObj::border_enable_disable(IN_THREAD);
		 });
}

// Install callbacks for editable combo-boxes on the border page.

static void border_editable_combobox(const x::w::focusable_container &container)
{
	x::w::editable_comboboxlayoutmanager lm=
		container->get_layoutmanager();

	lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appObj::border_enable_disable(IN_THREAD);
		 });

	lm->on_validate
		([]
		 (ONLY IN_THREAD,
		  const auto &)
		 {
			 appObj::border_enable_disable_later();

			 return true;
		 });
}

void appObj::borders_elements_initialize(app_elements_tptr &elements,
					 x::w::uielements &ui,
					 init_args &args)
{
	x::w::focusable_container border_name=
		ui.get_element("border_name_field");
	x::w::label border_new_name_label=
		ui.get_element("border_new_name_label");
	x::w::input_field border_new_name=
		ui.get_element("border_new_name_field");
	x::w::focusable_container border_from_name=
		ui.get_element("border_from_name");
	x::w::focusable_container border_color=
		ui.get_element("border_color");
	x::w::focusable_container border_color2=
		ui.get_element("border_color2");
	x::w::image_button border_rounded_corner_default=
		ui.get_element("border_rounded_corner_default");
	x::w::image_button border_rounded_corner_yes=
		ui.get_element("border_rounded_corner_yes");
	x::w::image_button border_rounded_corner_no=
		ui.get_element("border_rounded_corner_no");
	x::w::standard_comboboxlayoutmanager border_name_lm=
		border_name->get_layoutmanager();

	x::w::focusable_container border_width=
		ui.get_element("border_width");
	x::w::input_field border_width_scale=
		ui.get_element("border_width_scale");
	x::w::focusable_container border_height=
		ui.get_element("border_height");
	x::w::input_field border_height_scale=
		ui.get_element("border_height_scale");
	x::w::focusable_container border_hradius=
		ui.get_element("border_hradius");
	x::w::input_field border_hradius_scale=
		ui.get_element("border_hradius_scale");
	x::w::focusable_container border_vradius=
		ui.get_element("border_vradius");
	x::w::input_field border_vradius_scale=
		ui.get_element("border_vradius_scale");

	x::w::image_button border_dashes_option=
		ui.get_element("border_dashes_option");
	x::w::input_field border_dashes_field=
		ui.get_element("border_dashes_field");

	x::w::container border_preview_cell_border_container=
		ui.get_element("border_preview_cell_border_container");
	x::w::container border_preview=
		ui.get_element("border_preview");
	x::w::button border_update_button=
		ui.get_element("border_update_button");
	x::w::button border_reset_button=
		ui.get_element("border_reset_button");
	x::w::button border_delete_button=
		ui.get_element("border_delete_button");

	// Install callbacks.

	border_new_name->on_filter(args.label_filter);

	border_new_name->on_validate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger)
		 {
			 appObj::border_enable_disable(IN_THREAD);

			 return true;
		 });

	border_dashes_option->on_activate
		([]
		 (ONLY IN_THREAD,
		  size_t i,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appObj::border_enable_disable(IN_THREAD);
		 });

	border_image_button(border_rounded_corner_default);
	border_image_button(border_rounded_corner_yes);
	border_image_button(border_rounded_corner_no);
	border_image_button(border_dashes_option);

	border_name_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::border_selected, IN_THREAD,
				   info);
		 });

	border_editable_combobox(border_from_name);
	border_editable_combobox(border_color);
	border_editable_combobox(border_color2);

	// Save our elements
	elements.border_name=border_name;
	elements.border_new_name_label=border_new_name_label;
	elements.border_new_name=border_new_name;
	elements.border_from_name=border_from_name;
	elements.border_color=border_color;
	elements.border_color2=border_color2;
	elements.border_rounded_corner_default=border_rounded_corner_default;
	elements.border_rounded_corner_yes=border_rounded_corner_yes;
	elements.border_rounded_corner_no=border_rounded_corner_no;

	elements.border_width=border_width;
	elements.border_width_scale=border_width_scale;
	elements.border_height=border_height;
	elements.border_height_scale=border_height_scale;
	elements.border_hradius=border_hradius;
	elements.border_hradius_scale=border_hradius_scale;
	elements.border_vradius=border_vradius;
	elements.border_vradius_scale=border_vradius_scale;
	elements.border_dashes_option=border_dashes_option;
	elements.border_dashes_field=border_dashes_field;
	elements.border_preview_cell_border_container=
		border_preview_cell_border_container;
	elements.border_preview=border_preview;
	elements.border_update_button=border_update_button;
	elements.border_reset_button=border_reset_button;
	elements.border_delete_button=border_delete_button;

	border_update_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::border_update);
		 });

	border_reset_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 appinvoke(&appObj::border_reset, IN_THREAD);
		 });

	border_delete_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::border_delete);
		 });

}

void appObj::borders_initialize(ONLY IN_THREAD)
{
	border_info_t::lock lock{border_info};

	borders_initialize(IN_THREAD, lock);
	border_reset_values(IN_THREAD, lock);
}

void appObj::borders_initialize(ONLY IN_THREAD, border_info_t::lock &lock)
{
	x::w::standard_comboboxlayoutmanager lm=
		border_name->get_layoutmanager();

	auto existing_borders=theme.get()->readlock();

	existing_borders->get_root();
	auto xpath=existing_borders->get_xpath("/theme/border");
	auto n=xpath->count();

	lock->ids.clear();
	lock->ids.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		lock->ids.push_back(existing_borders->get_any_attribute("id"));
	}

	std::sort(lock->ids.begin(), lock->ids.end());

	// Duplicate IDS: TODO - report this.

	lock->ids.erase(std::unique(lock->ids.begin(), lock->ids.end()),
			lock->ids.end());

	std::vector<x::w::list_item_param> combobox_items;

	combobox_items.reserve(lock->ids.size()+1);

	combobox_items.push_back(_("-- New Border --"));
	combobox_items.insert(combobox_items.end(),
			      lock->ids.begin(),
			      lock->ids.end());
	lm->replace_all_items(IN_THREAD, combobox_items);
	lm->autoselect(IN_THREAD, 0, {});

	// The same list goes into the from name dropdown.

	combobox_items.erase(combobox_items.begin());

	{
		x::w::editable_comboboxlayoutmanager from_lm=
			border_from_name->get_layoutmanager();
		from_lm->replace_all_items(IN_THREAD, combobox_items);
	}
	border_enable_disable_buttons(IN_THREAD, lock);
}

namespace {
#if 0
}
#endif

// Implement UI border parse to store the loaded data in loaded_border_t

struct parse_border : x::w::ui::parse_border {

	appObj::loaded_border_t &loaded_border;

	parse_border(appObj::loaded_border_t &loaded_border)
		: loaded_border{loaded_border}
	{
	}

	void from(std::string &arg) override
	{
		loaded_border.from=std::move(arg);
	}

	void color(std::string &color,
		   std::string &color2) override
	{
		loaded_border.color=std::move(color);
		loaded_border.color2=std::move(color2);
	}

	void width(std::string &arg) override
	{
		loaded_border.width=std::move(arg);
	}

	void width_scale(unsigned arg) override
	{
		loaded_border.width_scale=arg;
	}

	void height(std::string &arg) override
	{
		loaded_border.height=std::move(arg);
	}

	void height_scale(unsigned arg) override
	{
		loaded_border.height_scale=arg;
	}

	void hradius(std::string &arg) override
	{
		loaded_border.hradius=std::move(arg);
	}

	void hradius_scale(unsigned arg) override
	{
		loaded_border.hradius_scale=arg;
	}

	void vradius(std::string &arg) override
	{
		loaded_border.vradius=std::move(arg);
	}

	void vradius_scale(unsigned arg) override
	{
		loaded_border.vradius_scale=arg;
	}

	void rounded(bool flag) override
	{
		loaded_border.rounded=flag;
	}

	void dashes(std::vector<double> &arg) override
	{
		loaded_border.dashes.emplace(std::move(arg));
	}
};

#if 0
{
#endif
}

void appObj::border_selected(ONLY IN_THREAD,
			     const
			     x::w::standard_combobox_selection_changed_info_t
			     &info)
{
	border_info_t::lock lock{border_info};

	size_t n=info.list_item_status_info.item_number;

	if (!info.list_item_status_info.selected)
	{
		// When a selection is changed, the old value is always
		// deselected first. So, hook this to clear and reset all
		// fields.

		border_new_name->set("");

		x::w::editable_comboboxlayoutmanager
			lm=border_from_name->get_layoutmanager();

		// Reenable the old border in the "from" name dropdown,
		// since it can now be used as "from".

		if (n > 0)
			lm->enabled(IN_THREAD, --n, true);

		return;
	}

	lock->current_selection.reset();

	if (n == 0) // New border entry
	{
		border_selected_locked(IN_THREAD, lock);
		return;
	}

	--n;

	x::w::standard_comboboxlayoutmanager
		lm=border_from_name->get_layoutmanager();

	// A new border is selected.
	//
	// Disable the corresponding item in the border_from_name
	// combo-box.

	lm->enabled(IN_THREAD, n, false);

	lock->current_selection.emplace();
	lock->current_selection->index=n;
	border_selected_locked(IN_THREAD, lock);
}

void appObj::border_reset(ONLY IN_THREAD)
{
	border_info_t::lock lock{border_info};

	border_selected_locked(IN_THREAD, lock);

	if (lock->current_selection)
		border_name->request_focus();
	else
		border_new_name->request_focus();
}

void appObj::border_selected_locked(ONLY IN_THREAD,
				    border_info_t::lock &lock)
{
	if (!lock->current_selection)
	{
		border_new_name_label->show(IN_THREAD);
		border_new_name->show(IN_THREAD);
		border_new_name->set(IN_THREAD, "");
		border_reset_values(IN_THREAD, lock);
		return;
	}

	auto &orig_params=*lock->current_selection;
	auto n=orig_params.index;

	auto current_value=theme.get()->readlock();
	current_value->get_root();

	auto xpath=get_xpath_for(current_value,
				 "border",
				 lock->ids.at(n));

	// We expect one to be there, of course.
	xpath->to_node(1);

	typedef x::exception exception;
	try {
		parse_border parser{orig_params.loaded_border};

		parser.parse({current_value});
	} REPORT_EXCEPTIONS(main_window);

	border_new_name_label->hide(IN_THREAD);
	border_new_name->hide(IN_THREAD);

	border_reset_values(IN_THREAD, lock);
}

void appObj::border_reset_values(ONLY IN_THREAD,
				 border_info_t::lock &lock)
{
	busy();

	// Clear everything.

	x::w::editable_comboboxlayoutmanager
		border_from_name_lm=border_from_name->get_layoutmanager(),
		border_color_lm=border_color->get_layoutmanager(),
		border_color2_lm=border_color2->get_layoutmanager(),
		border_width_lm=border_width->get_layoutmanager(),
		border_height_lm=border_height->get_layoutmanager(),
		border_hradius_lm=border_hradius->get_layoutmanager(),
		border_vradius_lm=border_vradius->get_layoutmanager();

	border_from_name_lm->unselect(IN_THREAD);
	border_color_lm->unselect(IN_THREAD);
	border_color2_lm->unselect(IN_THREAD);
	border_width_lm->unselect(IN_THREAD);
	border_height_lm->unselect(IN_THREAD);
	border_hradius_lm->unselect(IN_THREAD);
	border_vradius_lm->unselect(IN_THREAD);

	border_from_name_lm->set(IN_THREAD, "");
	border_color_lm->set(IN_THREAD, "");
	border_color2_lm->set(IN_THREAD, "");

	border_width_validated->set(IN_THREAD, "");
	border_height_validated->set(IN_THREAD, "");
	border_hradius_validated->set(IN_THREAD, "");
	border_vradius_validated->set(IN_THREAD, "");
	border_dashes_option->set_value(IN_THREAD, 0);
	border_rounded_corner_default->set_value(IN_THREAD, 1);

	constexpr std::optional<unsigned> no_scale_value;

	border_width_scale_validated->set(IN_THREAD, no_scale_value);
	border_height_scale_validated->set(IN_THREAD, no_scale_value);
	border_hradius_scale_validated->set(IN_THREAD, no_scale_value);
	border_vradius_scale_validated->set(IN_THREAD, no_scale_value);
	border_dashes_field_validated
		->set(IN_THREAD, std::vector<double>{});

	// Reset the "From" border

	// Note: clearing the input field is not enough, we need to unselect
	// too, because if we don't, and the previous selection gets selected
	// again, the editable combo-box does not fire.

	if (!lock->current_selection)
	{
		static const x::w::focusable_container
			appObj::*clear_editable_comboboxes_on_reset[]=
			{
			 &appObj::border_from_name,
			 &appObj::border_color,
			};

		for (const auto &combo_boxes:clear_editable_comboboxes_on_reset)
		{
			x::w::editable_comboboxlayoutmanager
				lm=(this->*combo_boxes)->get_layoutmanager();

			lm->set(IN_THREAD, "", true);
		}
		border_rounded_corner_default->set_value(1);
	}
	else
	{
		auto &loaded_border=lock->current_selection->loaded_border;

		// Border colors
		border_color_lm->set(IN_THREAD, loaded_border.color);
		border_color2_lm->set(IN_THREAD, loaded_border.color2);

		if (!loaded_border.from.empty())
		{
			auto b=lock->ids.begin();
			auto e=lock->ids.end();

			auto iter=std::find(b, e, loaded_border.from);

			if (iter == e) // ?
			{
				border_from_name_lm
					->set(IN_THREAD, loaded_border.from,
					      true);
			}
			else
			{
				border_from_name_lm->autoselect(iter-b);
			}
		}
		else
		{
			border_from_name_lm->set("", true);
		}

		// Dashes

		if (loaded_border.dashes)
		{
			border_dashes_option->set_value(IN_THREAD, 1);

			border_dashes_field_validated
				->set(IN_THREAD, *loaded_border.dashes);
		}
		else
		{
			border_dashes_option->set_value(IN_THREAD, 0);
			border_dashes_field_validated
				->set(IN_THREAD, std::vector<double>{});
		}

		// Rounded

		if (loaded_border.rounded)
		{
			(*loaded_border.rounded ?
			 border_rounded_corner_yes:border_rounded_corner_no)
				->set_value(1);
		}
		else
		{
			 border_rounded_corner_default->set_value(1);
		}

		// Border sizes

		for (const auto &border_size:all_border_sizes)
		{
			(this->*(border_size.validated_field))
				->set(IN_THREAD,
				      loaded_border.*(border_size.value));
		}

		for (const auto &border_size_scale:all_border_size_scales)
		{
			(this->*(border_size_scale.validated_field))
				->set(IN_THREAD,
				      loaded_border.*(border_size_scale.value));
		}
	}

	border_enable_disable_buttons(IN_THREAD, lock);
}

void appObj::border_enable_disable_buttons(ONLY IN_THREAD,
					   border_info_t::lock &lock)
{
	// Dashes option
	bool dashes=border_dashes_option->get_value();

	if (dashes)
	{
		border_dashes_field->set_enabled(IN_THREAD, true);
	}
	else
	{
		// Empty list.
		border_dashes_field->set_enabled(IN_THREAD, false);
	}

	// Now, read the state of all widgets and create new save_params

	lock->save_params.reset();
	lock->save_params.emplace();

	auto &save_params=*lock->save_params;

	save_params.border_new_value.color=
		x::trim(border_color->editable_combobox_get());

	// Disable color2 if there is no color or no dashes.

	if (save_params.border_new_value.color.empty() || !dashes)
	{
		border_color2->set_enabled(IN_THREAD, false);
	}
	else
	{
		border_color2->set_enabled(IN_THREAD, true);
	}

	bool good_save_params=true;

	if (!lock->current_selection)
	{
		save_params.border_new_name=
			x::trim(x::w::input_lock{border_new_name}.get());

		if (save_params.border_new_name.empty())
		{
			good_save_params=false;
		}
	}

	save_params.border_new_value.from=
		x::trim(border_from_name->editable_combobox_get());

	if (!save_params.border_new_value.color.empty())
		save_params.border_new_value.color2=
			x::trim(border_color2->editable_combobox_get());

	for (const auto &border_size:all_border_sizes)
	{
		if (!good_save_params)
			break;

		auto v=(this->*(border_size.validated_field))->validated_value
			.get();

		if (!v)
		{
			good_save_params=false;
			break;
		}

		save_params.border_new_value.*(border_size.value)=
			border_format_size(*v);
	}

	for (const auto &border_size_scale:all_border_size_scales)
	{
		if (!good_save_params)
			break;

		auto v=(this->*(border_size_scale.validated_field))
			->validated_value.get();

		if (!v)
		{
			good_save_params=false;
			break;
		}

		save_params.border_new_value.*(border_size_scale.value)=*v;
	}

	if (good_save_params)
	{
		if (border_rounded_corner_no->get_value())
		{
			save_params.border_new_value.rounded=false;
		}
		if (border_rounded_corner_yes->get_value())
		{
			save_params.border_new_value.rounded=true;
		}
	}

	if (good_save_params && border_dashes_option->get_value())
	{
		save_params.border_new_value.dashes=
			border_dashes_field_validated->validated_value.get();
		if (!save_params.border_new_value.dashes)
		{
			good_save_params=false;
		}
	}

	if (!good_save_params)
	{
		lock->save_params.reset();
	}

	enable_disable_urd(lock,
			   border_update_button,
			   border_delete_button,
			   border_reset_button);

	if (lock->current_selection && lock->save_params &&
	    *lock->save_params == *lock->current_selection)
	{
		using x::exception;

		try {
			auto generator=
				x::w::uigenerators::create(theme.get());

			auto preview_border=
				generator->lookup_border
				(lock->ids.at(lock->current_selection->index),
				 true, "current_border");
			x::w::borderlayoutmanager blm=
				border_preview->get_layoutmanager();

			blm->update_border(IN_THREAD, preview_border);
		} REPORT_EXCEPTIONS(main_window);

		border_preview_cell_border_container->show(IN_THREAD);
	}
	else
	{
		border_preview_cell_border_container->hide(IN_THREAD);
	}
}

appObj::get_updatecallbackptr appObj::border_update(ONLY IN_THREAD)
{
	border_info_t::lock lock{border_info};

	if (!lock->save_params)
		return nullptr;

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       border_info_t::lock lock{saved_lock};

		       return me->border_update2(lock);
	       };
}

appObj::update_callback_t appObj::border_update2(border_info_t::lock &lock)
{
	appObj::update_callback_t ret;

	if (!lock->save_params)
		return ret; // Shouldn't happen.

	auto &save_params=*lock->save_params;

	std::string id=save_params.border_new_name;
	bool is_new=true;

	if (lock->current_selection)
	{
		id=lock->ids.at(lock->current_selection->index);
		is_new=false;
	}

	auto created_update=create_update("border", id, is_new);

	if (!created_update)
		return ret;

	auto &[doc_lock, new_border]=*created_update;

	auto &new_value=save_params.border_new_value;

	if (!new_value.from.empty())
		doc_lock->attribute({"from", new_value.from});

	if (!new_value.color.empty())
	{
		doc_lock->create_child()->element({"color"})
			->text(new_value.color)->parent()->parent();

		if (!new_value.color2.empty())
			doc_lock->create_child()->element({"color2"})
				->text(new_value.color2)->parent()->parent();
	}

	if (new_value.rounded)
	{
		doc_lock->create_child()->element({"rounded"})
			->text(*new_value.rounded ? 1:0)->parent()->parent();
	}

	for (const auto &border_size : all_border_sizes)
	{
		auto &v=new_value.*(border_size.value);

		if (v.empty())
			continue;

		doc_lock->create_child()->element({border_size.name})
			->text(v)->parent()->parent();
	}

	for (const auto &border_size_scale : all_border_size_scales)
	{
		auto &v=new_value.*(border_size_scale.value);

		if (!v)
			continue;

		doc_lock->create_child()->element({border_size_scale.name})
			->text(*v)->parent()->parent();
	}

	if (new_value.dashes)
	{
		if (new_value.dashes->size() == 0)
			doc_lock->create_child()->element({"dash"});
		else for (auto v:*new_value.dashes)
		     {
			     doc_lock->create_child()->element({"dash"})
				     ->text(fmtdblval(v))
				     ->parent()->parent();
		     }
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    border_info_t::lock lock{saved_lock};

			    me->border_update2(lock, id, is_new,
					       busy_mcguffin);
		    });

	return ret;
}

void appObj::border_update2(border_info_t::lock &lock,
			    const std::string &id,
			    bool is_new,
			    const x::ref<x::obj> &busy_mcguffin)
{
	if (is_new)
	{
		x::w::standard_comboboxlayoutmanager
			from_name_lm=border_from_name->get_layoutmanager();

		update_new_element({id}, lock->ids, border_name,
				   [&]
				   (size_t i)
				   {
					   from_name_lm->insert_items(i-1,
								      {id});
				   });

		status->update(_("Created new border"));
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
			 appinvoke(&appObj::border_update3,
				   IN_THREAD,
				   busy_mcguffin);
		 });
}

void appObj::border_update3(ONLY IN_THREAD, const x::ref<x::obj> &busy_mcguffin)
{
	border_info_t::lock lock{border_info};

	border_selected_locked(IN_THREAD, lock);
	status->update(_("Border updated"));

	main_window->in_thread_idle([busy_mcguffin]
				    (ONLY IN_THREAD)
				    {
				    });
}

appObj::get_updatecallbackptr appObj::border_delete(ONLY IN_THREAD)
{
	border_info_t::lock lock{border_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       border_info_t::lock lock{saved_lock};

		       return me->border_delete2(lock);
	       };
}

appObj::update_callback_t appObj::border_delete2(border_info_t::lock &lock)
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

		auto xpath=get_xpath_for(doc_lock, "border", id);

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
			    border_info_t::lock lock{saved_lock};

			    me->border_delete2(lock, index,
					       busy_mcguffin);
		    });

	return ret;
}

void appObj::border_delete2(border_info_t::lock &lock,
			    size_t index,
			    const x::ref<x::obj> &busy_mcguffin)
{
	// Update the loaded list of borders we store here,
	// and set the current border combo-box dropdown to "New Border".

	lock->ids.erase(lock->ids.begin()+index);

	x::w::standard_comboboxlayoutmanager name_lm=
		border_name->get_layoutmanager();

	x::w::standard_comboboxlayoutmanager from_name_lm=
		border_from_name->get_layoutmanager();

	name_lm->remove_item(index+1);
	from_name_lm->remove_item(index);

	lock->current_selection.reset();

	name_lm->autoselect(0);

	// autoselect(0) queues up a request.
	// When it gets processed, the border new name field gets show()n.
	// We want to request focus for that after all that processing also
	// gets done:

	main_window->in_thread_idle
		([busy_mcguffin]
		 (ONLY IN_THREAD)
		 {
			 appinvoke([&]
				   (appObj *me)
				   {
					   border_info_t::lock
						   lock{me->border_info};
					   me->border_reset_values(IN_THREAD,
								  lock);

					   me->border_new_name
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
