/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "creator/uicompiler_generators_impl.H"
#include "creator/appgenerator_function.H"
#include "creator/appgenerator_functions.H"
#include "creator/appgenerator_save.H"
#include "creator/object_appgenerator_function.H"
#include "creator/uicompiler.H"
#include "creator/app.H"
#include "creator/setting_handler.H"
#include "creator/create_ui_info.H"

#include "x/w/font_literals.H"
#include "x/w/factory.H"
#include "x/w/image_button.H"
#include "x/w/button.H"
#include "x/w/input_field.H"
#include "x/w/input_field_config.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/canvas.H"
#include "x/w/scrollbar.H"
#include "x/w/uigenerators.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/text_param_literals.H"
#include "x/w/alignment.H"
#include "x/w/uielements.H"
#include "x/w/main_window.H"
#include <x/xml/xpath.H>
#include <x/xml/readlock.H>
#include <x/exception.H>
#include <x/functionalrefptr.H>
#include <x/vector.H>
#include <x/chrcasecmp.H>
#include <x/visitor.H>
#include <x/locale.H>
#include <x/refptr_hash.H>
#include <x/weakcapture.H>
#include <x/messages.H>
#include <x/strtok.H>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <optional>
#include <courier-unicode.h>

using namespace std::literals::string_literals;

// Setting-specific behavior.

// Each possible type of a setting implements create_ui() which receives
// the setting's current value.
//
// create_ui() should use the given factory to create a single widget for
// the setting's value.
//
// It returns a closure that gets called to retrieve the setting's (possibly)
// updated value. It should return std::nullopt if the setting's value failed
// validation. The closure receives a bool parameter, which is normally true,
// and the closure should report an error if the validation fails. A false
// parameter indicates that the closure should only return std::nullopt
// and without reporting an error. This is used when the closure gets used
// to retrieve the current value for secondary purposes. Normally the closure
// gets called when attempting to save the generator's new value, and full
// validation should occur, and an error should be reported; but sometimes UI
// work needs to know the current value, and it's ok if it's not currently
// set.
//
// define_additional_parameters() gets called after parsing all generator
// parameters, and it receives a map of all XML elements that have not been
// recognized as a valid parameter. The handler can look through this for
// anything of interest, if found it gets removed. The layoutmanager_type
// setting looks to see if there's a <config> value for the layoutmanager,
// and loads one if it exists.
//
// load() loads the parameter's value. The default implementation calls
// get_text() and stores it in the string_value.
//
// load_from_parent_element() gets called with the parent element's XML node.
// This is used to read the existing @id attribute.
//
// saved_element() gets called after the element, and its value, are saved
// The write lock is positioned at the saved element's XML node. This is used
// to write additional content. For example, the layoutmanager_type handler,
// used by a <container> will checkif any <config> values were specified, then
// proceed and write the <config> section.
//
// This is also used by the text_param setting to add type="theme_text"
// attribute.


// <element> existence, nothing more.
// Checkbox.

struct checkbox_handler : setting_handler {

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		auto field=info.value_factory->create_checkbox(
			[&]
			(const auto &f)
			{
			});

		if (!info.value.string_value.empty())
		{
			field->set_value(1);
		}

		return [field, name=info.parameter_name](bool alert)
		{
			return parameter_value{field->get_value()
				? U"1":U""};
		};
	}

	bool flag_value() const override
	{
		return true;
	}

	void load(const x::xml::readlock &lock,
		  parameter_value &value) const override
	{
		// This element exists, so it has a value.

		value.string_value=std::u32string{U"1"}; // TODO: gcc warning
	}

	void save(const setting_save_info &save_info,
		  appgenerator_save &info) const override
	{
		if (save_info.value.string_value.empty())
			return;

		save_info.lock->create_child()
			->element({save_info.parameter_name});
		save_info.lock->get_parent();
	}
};

static const checkbox_handler checkbox_handler_inst;

static std::optional<parameter_value> get_inputfield(
	const x::w::input_field &field,
	bool alert,
	bool required
)
{
	auto str=field->get_unicode();

	if (required && str.empty())
	{
		if (alert)
		{
			field->stop_message(_("Value required"));
			field->request_focus();
		}
		return std::nullopt;
	}
	return parameter_value{str};
}

// <element> contains an opaque value, not interpreted any further.
//
// Input field

struct single_value_handler : setting_handler {

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		auto field=info.value_factory
			->create_input_field(
				info.value.string_value,
				get_config()
			);

		return [field, required=info.required](bool alert)
		{
			return get_inputfield(field, alert, required);
		};
	}

	virtual x::w::input_field_config get_config() const
	{
		return {};
	}
};

static const single_value_handler single_value_handler_inst;

// Use an input field for an element or container's "id" attribute.

struct id_handler : single_value_handler {

	// Implement load_from_parent_element(), read from the "id" attribute.

	bool load_from_parent_element(const x::xml::readlock &lock,
				      const parse_parameterObj &parameter,
				      parameter_value &value) const override
	{
		value.string_value=lock->get_u32any_attribute("id");
		return false;
	}

	// Override save() by setting the "id" attribute.

	void save(const setting_save_info &save_info,
		  appgenerator_save &info) const override
	{
		if (save_info.value.string_value.empty())
			return;

		save_info.lock->attribute({"id", save_info.value.string_value});
	}
};

static const id_handler id_handler_inst;

// Implement a setting based on a standard combo-box

// The specific setting subclasses and implement combobox_values() that
// returns a list of values for the standard combo-box.

struct standard_combobox_handler : setting_handler {

	// Helper for searching list item values for a specific u32string.

	// Given the list of values for this combo-box, find the one that
	// matches the current value.

	static std::vector<x::w::list_item_param>
	::const_iterator find_string(const std::vector<x::w::list_item_param>
				     &values,
				     const std::u32string &value)
	{
		// Convert the current value to lowercase, for a case-
		// insensitive search.
		auto lower_value=unicode::tolower(value);

		return std::find_if(
			values.begin(),
			values.end(),
			[&]
			(const auto &list_item)
			{
				const x::w::list_item_param::variant_t
					&v=list_item;

				return std::visit(
					x::visitor{
						[&](const x::w::text_param &s)
						{
							return unicode::tolower(
								s.string
							) == lower_value;
						}, [&](const auto &v)
						{
							return false;
						}
					}, v);
			});
	}

	// Implement create_ui()...

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		// ...by calling create_standard_combobox_ui(), with a do-
		// nothing callback.

		return create_standard_combobox_ui(
			IN_THREAD, info,
			[](const auto &, const auto &) {}
		);
	}

	// create_standard_combobox_ui()'s callback's type.

	typedef void create_cb_t(const x::w::focusable_container &,
				 const x::w::standard_comboboxlayoutmanager &);

	// Create a standard combo-box UI

	// The callback gets invoked after the combo-box gets created,
	// and receives the combo-box container and the layout manager as
	// the parameters.

	template<typename F>
	setting_create_ui_ret_t
	create_standard_combobox_ui(ONLY IN_THREAD, const create_ui_info &info,
				    F &&f) const
	{
		return do_create_standard_combobox_ui
			(IN_THREAD, info,
			 x::make_function<create_cb_t>(std::forward<F>(f)));
	}

	setting_create_ui_ret_t
	do_create_standard_combobox_ui(ONLY IN_THREAD,
				       const create_ui_info &info,
				       const x::function<create_cb_t> &cb)
		const LIBCXX_HIDDEN
	{
		auto values=combobox_values(info);

		// Always prepend an empty string, to the list of values,
		// that represents an unset value.

		values.insert(values.begin(), "");

		auto combobox=info.value_factory->create_focusable_container(
			[&]
			(const auto &container)
			{
				auto layout=
					container->standard_comboboxlayout();

				layout->replace_all_items(IN_THREAD, values);

				// Find the current value and make it selected
				// by default.
				//
				// We inserted an empty string, so it should
				// pick it up for an unset value. Loading
				// the theme file should've validated all
				// values, but failsafe to autoselecting
				// the empty string, if we don't find this
				// value.

				auto p=find_string(values,
						   info.value.string_value);
				size_t i=0;

				if (p != values.end())
					i=p-values.begin();
				layout->autoselect(IN_THREAD, i, {});
				cb(container, layout);
			},
			x::w::new_standard_comboboxlayoutmanager{});

		// The validator reads the combo-box's selected value
		// and uses it.

		return [combobox=x::make_weak_capture(combobox),
			values=std::move(values)]
			(bool alert) -> std::optional<parameter_value>
			{
				auto got=combobox.get();

				if (!got)
					return std::nullopt;

				auto &[combobox]=*got;

				auto selected=combobox
				->standard_comboboxlayout()
				->selected();

				// Fetch out the list item that corresponds
				// to the currently-selected() list item,
				// and it better be a text_param.

				if (selected && *selected < values.size())
				{
					const x::w::list_item_param::variant_t
						&v=values[*selected];

					if (std::holds_alternative<
					    x::w::text_param>(v))
						return parameter_value{
							std::get<
							x::w::text_param>(v)
							.string
						};
				}
				if (alert)
				{
					combobox->stop_message(
						_("Selection required")
					);
					combobox->request_focus();
				}
				return std::nullopt;
			};
	}

	virtual std::vector<x::w::list_item_param>
	combobox_values(const create_ui_info &info) const=0;
};

/////////////////////////////////////////////////////////////////////////////
//
// Additional generator functions
//
// Several generators use additional set of generator functions. A generator
// for a new <container> produces generators for the generator's <config>.
// Picking each layout manager loads the <config> generators list
// with the appropriate generators for the layout manager. This way
// the existing layout manager's <config>s don't get permanently lost
// if a different layout manager gets selected by accident.
//
// For this unordered_set we are providing the hash function and
// the comparison operator, here.
//
// This is also used to support generators that don't vary.
// "compiler.layout_parseconfig" generator parameter always references
// the generators for a generic factory. In this case the unordered map
// would only have one entry.

// Transparent hash, that hashes the const_uicompiler_generators.

struct existing_appgenerator_functions_hash {

	typedef void is_transparent;

	size_t operator()(const existing_appgenerator_functions &e)
		const
	{
		return operator()(e.compilerbase);
	}

	size_t operator()(const const_uicompiler_generatorsbase &g)
		const
	{
		return std::hash<const_uicompiler_generatorsbase>{}(g);
	}
};

// Transparent comparator, that compares the const_uicompiler_generators

struct existing_appgenerator_functions_equal {
	typedef void is_transparent;

	const const_uicompiler_generatorsbase
	&compiler(const existing_appgenerator_functions &e)
		const
	{
		return e.compilerbase;
	}

	const const_uicompiler_generatorsbase
	&compiler(const const_uicompiler_generatorsbase &compilerbase)
		const
	{
		return compilerbase;
	}

	template<typename A, typename B>
	bool operator()(A && a, B && b) const
	{
		return compiler(std::forward<A>(a))
			== compiler(std::forward<B>(b));
	}
};

// The current state of the generators

// create_ui() allocates a reference-counted object containing the
// current_functions.
//
// This reference-counted object is captured by value by all closures,
// and serves to store the per-generator parsed_functions.
//
// 1) The per-generator parsed_functions
//
// 2) The currently-showed functions, what create_ui() returns.
//    It gets constructed using the appropriate per-generator
//    parsed_functions.
//
// After construction, create_config_container() gets called to create the
// container for the generators.
//
// save_config_container() gets called when the main value, to which these
// generators are attached, get saved. If the main valur is valid, and the
// generators are value, the generators get saved in value->extra. Invalid
// generators clear the value.

typedef x::mpobj<std::tuple<
			 std::unordered_set<
				 existing_appgenerator_functions,
				 existing_appgenerator_functions_hash,
				 existing_appgenerator_functions_equal
				 >,
			 appgenerator_functionsptr
			 >
		 > current_functions_t;

struct extra_generators_uiObj : virtual public x::obj {

public:
	current_functions_t current_functions;

	extra_generators_uiObj(const parameter_value &value);

	// Create the UI for the generator functions.
	// Constructs appgenerator_functions, and uses it to initialize
	// a list widget that shows the functions.
	//
	// The list widget itself is created in another container, a grid
	// container. When a different layout manager gets selected the
	// grid container gets cleared, removing the previous list of
	// generators, and another one gets created together with its
	// appgenerator_functions.
	//
	// Since each new widget in the main window goes at the end of the
	// window's focus order, we pass in in the combo-box that selects
	// the layout manager, and the list widget follows it in the tabbing
	// order.
	//
	// This is also reused to reuse the UI creation for a specific
	// factory's generators. There is no layout manager combo-box, so
	// this parameter is a null pointer in that case.

	static appgenerator_functions
	create_config_ui(
		ONLY IN_THREAD,
		appObj *me,
		const x::w::main_window &parent_window,
		const x::w::focusable_containerptr &focus_after,
		const x::w::container &c,
		const existing_appgenerator_functions &existing_functions
	);

	static appgenerator_functions create_config_ui(
		ONLY IN_THREAD,
		appObj *me,
		const x::w::main_window &parent_window,
		const x::w::focusable_containerptr &focus_after,
		const x::w::container &c,
		const existing_appgenerator_functions &existing_functions,
		const x::w::gridfactory &f
	);

	// Create the container for the list widget.
	x::w::container create_config_container(
		ONLY IN_THREAD,
		appObj *me,
		const x::w::main_window &mw,
		const x::w::gridfactory &f,
		const x::w::focusable_containerptr &focus_after={}
	);

	void save_config_container(std::optional<parameter_value> &value);
};

extra_generators_uiObj::extra_generators_uiObj(const parameter_value &value)
{
	// If the generators are aready specified we'll set the
	// parsed_values and the current_generators.

	if (std::holds_alternative<existing_appgenerator_functions>(
		    value.extra
	    ))
	{
		current_functions_t::lock lock{current_functions};

		auto &[parsed_values, current_generators]=*lock;

		parsed_values.insert(
			std::get<existing_appgenerator_functions>(value.extra));
	}
}

appgenerator_functions extra_generators_uiObj
::create_config_ui(ONLY IN_THREAD,
		   appObj *me,
		   const x::w::main_window &parent_window,
		   const x::w::focusable_containerptr &focus_after,
		   const x::w::container &c,
		   const existing_appgenerator_functions &existing_functions)
{
	// The grid layout manager container with the new generators list.
	//
	// Remove whatever was there before.
	auto glm=c->gridlayout();

	glm->remove();

	return create_config_ui(IN_THREAD, me, parent_window, focus_after, c,
				existing_functions, glm->append_row());
}

appgenerator_functions extra_generators_uiObj
::create_config_ui(ONLY IN_THREAD,
		   appObj *me,
		   const x::w::main_window &parent_window,
		   const x::w::focusable_containerptr &focus_after,
		   const x::w::container &c,
		   const existing_appgenerator_functions &existing_functions,
		   const x::w::gridfactory &f)
{
	x::w::uielements ui;

	// Run the generators_contents_values_grid_values generator, to
	// create the UI.

	f->generate("generator_contents_values_grid_values",
		    me->current_generators->cxxwui_generators, ui);

	// Initialize the new UI.
	app_generator_elements_tptr generator_elements;

	appgenerator_functionsObj::generators_values_elements_initialize
		(generator_elements,
		 ui,
		 me->current_generators->uicompiler_info);

	x::w::focusable_container values_container{
		ui.get_element("generator_contents_values")
	};

	// Get our input focus after this wdget.
	//
	// This is used for layout manager factories. Changing the layout
	// manager type creates a new generator_contents_values widget, for
	// the new layout manager. It should still follow, in focus order,
	// after the layout manager type combo-box.

	if (focus_after)
	{
		values_container->get_focus_after(focus_after);
	}
	auto functions=
		appgenerator_functions::create(generator_elements,
					       parent_window,
					       me->current_generators);

	generator_info_lock lock{functions->generator_info};

	functions->generator_values_initialize
		(IN_THREAD, lock, existing_functions);

	return functions;
}

x::w::container extra_generators_uiObj::create_config_container(
	ONLY IN_THREAD,
	appObj *me,
	const x::w::main_window &mw,
	const x::w::gridfactory &f,
	const x::w::focusable_containerptr &focus_after
)
{
	f->padding(0);

	return f->create_container(
		[&, this]
		(const auto &c)
		{
			// Look at the parsed_values. It's non-empty if
			// they were loaded, above, from the <config> section.
			current_functions_t::lock lock{
				current_functions
			};

			auto &[parsed_values, current_generator]=*lock;

			// An empty parsed_values indicates that there is no
			// initial layout manager. Leave the grid container
			// empty.

			if (parsed_values.empty())
				return;

			// Otherwise create the initial layout manager's
			// container with the <config> generators.

			current_generator=create_config_ui(
				IN_THREAD, me, mw,
				focus_after,
				c,
				*parsed_values.begin()
			);
		},
		x::w::new_gridlayoutmanager()
	);
}

void extra_generators_uiObj::save_config_container(
	std::optional<parameter_value> &value
)
{
	current_functions_t::lock lock{current_functions};

	auto &[parsed_values, current_generators]=*lock;

	// If value is set we also expect current_generators to be set,
	// otherwise there is no value.

	if (!current_generators)
		value.reset();

	if (value)
	{
		// Extract the final set of values, for saved_element()
		// to read.

		generator_info_lock lock{
			current_generators->generator_info
		};

		value->extra=existing_appgenerator_functions{
			lock->compilerbase,
			*lock->functions,
		};
	}
}

typedef x::ref<extra_generators_uiObj> extra_generators_ui;

/////////////////////////////////////////////////////////////////////////////


// This element is a type that specifies a layout manager for a new
// <container>.

struct layoutmanager_type_handler : standard_combobox_handler {

	// The list of allowed combo-box values is obtained by
	// compiling a list of all appuigenerator_type::layoutmanagers.

	std::vector<x::w::list_item_param
		    > combobox_values(const create_ui_info &info) const override
	{
		std::vector<std::u32string> layout_managers;

		for (const auto &[name, compiler]
			     : info.app->current_generators
			     ->uicompiler_info->uigenerators)
		{
			if (compiler->type_category.type !=
			    appuigenerator_type::layoutmanager)
				continue;

			auto &s=compiler->type_category.category;

			if (s.empty())
				continue; // Skips singleton layout manager

			layout_managers.push_back(x::locale::base::global()
						  ->tou32(s));
		}

		std::sort(layout_managers.begin(), layout_managers.end());

		return {layout_managers.begin(), layout_managers.end()};
	}

	// And now, when we have the selected layout manager's name,
	// find the const_uicompiler_generators this refers to.

	static const_uicompiler_generatorsptr
	find_newlayout_generator(const std::u32string &value)
	{
		const_uicompiler_generatorsptr new_layout_generator;

		appinvoke([&]
			  (appObj *me)
		{
			auto &uicompiler_info=
				me->current_generators->uicompiler_info;
			auto &new_layouts=
				uicompiler_info->new_layouts;

			auto new_layout_iter=
				new_layouts.find(x::locale::base::global()
						 ->fromu32(value));

			if (new_layout_iter == new_layouts.end())
				return;

			new_layout_generator=new_layout_iter->second;
		});

		return new_layout_generator;
	}

	// Override define_additional_parameters

	// If there's a <config> parameter this gets run through the appropriate
	// new layout manager parser.

	void define_additional_parameters(parameter_value &value,
					  std::unordered_map<std::string,
					  x::xml::readlock>
					  &unparsed_additional_parameters)
		const override
	{
		auto new_layout=find_newlayout_generator(value.string_value);

		if (!new_layout) return;

		// Is there a <config> here?

		auto config_iter=unparsed_additional_parameters
			.find("config");

		appinvoke([&]
			  (appObj *me)
		{
			// Whether or not there was a <config> we'll
			// define value.extra.
			//
			// A new <container> won't get here, new_layout
			// will be empty. Otherwise we'll always set value.extra
			// and that's how create_ui() knows whether to create
			// the initial container, or not.

			auto &extra=value.extra
				.emplace<existing_appgenerator_functions>
				(new_layout);

			if (config_iter != unparsed_additional_parameters.end())
			{
				extra.parsed_functions=new_layout->parse(
					config_iter->second->clone(),
					me->current_generators
					->uicompiler_info->uigenerators
				);
			}
		});

		if (config_iter != unparsed_additional_parameters.end())
			unparsed_additional_parameters.erase(config_iter);
	}

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override;

	void saved_element(const setting_save_info &save_info,
			   appgenerator_save &info) const override;

	virtual const char *container_name() const=0;
};

struct layoutmanager_type_handler_name : layoutmanager_type_handler {

	const char *container_name() const override
	{
		return "name";
	}
};

struct layoutmanager_type_handler_progressbar : layoutmanager_type_handler {

	const char *container_name() const override
	{
		return "progressbar";
	}
};


// Override create_ui

// Calls create_standard_combobox_ui to create the main combo-box that
// selects the layout manager type.
//
// The combo-box gets captures and used to create a container for the
// layout manager's <config>, on the nxet row.

setting_create_ui_ret_t
layoutmanager_type_handler::create_ui(ONLY IN_THREAD,
				      const create_ui_info &info) const
{
	x::w::focusable_containerptr type_combo;
	x::w::standard_comboboxlayoutmanagerptr type_lm;

	auto ret=create_standard_combobox_ui(IN_THREAD, info,
					     [&]
					     (const auto &c,
					      const auto &lm)
					     {
						     type_combo=c;
						     type_lm=lm;
					     });

	// Create the <config> generator list on the next row.

	auto next_row=info.name_value_lm->append_row();

	// First, create a <canvas> in the first column.

	next_row->create_canvas();

	// define_additional_parameters() checks whether the layout manager
	// was specified, for this setting, and always initializes values.extra
	// even if there was no <config>.

	auto current_config_info=
		extra_generators_ui::create(info.value);

	auto config_container=
		current_config_info->create_config_container(
			IN_THREAD, info.app, info.mw, next_row, type_combo
		);

	// Update the <config> container whenever a different layout manager
	// gets selected.

	type_lm->on_selection_changed
		([ret, config_container, current_config_info,
		  wmw=info.mw.weaken(),
		  type_combo=x::weakptr<x::w::focusable_containerptr>{type_combo}]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		{
			// What layout manager is selected now?

			auto type_comboptr=type_combo.getptr();

			if (!type_comboptr)
				return;

			auto mw=wmw.getptr();

			if (!mw)
				return;

			if (status_info.list_item_status_info.trigger.index() ==
			    x::w::callback_trigger_initial)
				return;

			if (!status_info.list_item_status_info.selected)
				return;

			auto value=ret(false);

			// Before figuring out what to do with the new
			// layout manager selection: if an existing
			// layout manager's <config> generators were shown...
			current_functions_t::lock lock{
				current_config_info->current_functions
			};

			auto &[parsed_values, current_generators]=*lock;

			if (current_generators)
			{
				// We take them and put save them in the
				// parsed_values.

				generator_info_lock lock{
					current_generators->generator_info
				};

				auto iter=parsed_values.find(lock->compilerbase);

				if (iter != parsed_values.end())
					parsed_values.erase(iter);

				parsed_values.emplace(lock->compilerbase,
						      *lock->functions);
			}

			// In all cases the current_generators are no more.

			current_generators=appgenerator_functionsptr{};

			// And if a new layout manager was selected we'll
			// set them up.
			if (value)
			{
				auto compiler=find_newlayout_generator
					(value->string_value);

				if (compiler)
				{
					// Ok, we know the compiler, if
					// we do not have any parsed_values
					// ( <config> values) for this one
					// we'll create an empty list.

					auto iter=
						parsed_values.find(compiler);

					if (iter == parsed_values.end())
						iter=parsed_values.emplace
							(compiler).first;

					appinvoke([&]
						  (appObj *me)
					{
						current_generators=
							extra_generators_uiObj::
							create_config_ui(
								IN_THREAD, me,
								mw,
								type_comboptr,
								config_container
								,
								*iter);
						config_container->show_all(
							IN_THREAD
						);
					});
					return;
				}
			}

			// No layout manager is now selected, clear the
			// wiedges.
			config_container->gridlayout()->remove();
		});

	return [current_config_info, ret]
		(bool alert)
	{
		auto value=ret(alert);

		current_config_info->save_config_container(value);

		return value;
	};
}

void layoutmanager_type_handler::saved_element(
	const setting_save_info &save_info,
	appgenerator_save &info) const
{
	// We expect to have a previous setting that gives the name of the
	// new container.
	//
	// A <container> has a <name> and a <type>.

	if (!save_info.lock->get_previous_element_sibling() ||
	    save_info.lock->name() != container_name())
	{
		throw EXCEPTION("Internal error: cannot locate new container's "
				"<name>");
	}

	auto container_name=save_info.lock->get_text();

	// Back to me:
	save_info.lock->get_next_element_sibling();

	// Set up an extra_saves callback that automatically creates the
	// new container's layout manager, unless it exists already.

	auto value_string=
		x::locale::base::global()->fromu32(
			save_info.value.string_value
		);

	info.extra_saves.emplace_back
		([value_string, container_name]
		 (const x::xml::writelock &lock,
		  appgenerator_save &save)
		{
			appinvoke([&]
				  (appObj *me)
			{
				me->generator_autocreate_layout_or_factory
					(lock, save,
					 "layout",
					 value_string,
					 container_name);
			});
		});

	// We expect to have the list of functions here, if not something's
	// wrong, so we take the safe route and just bail out.

	if (!std::holds_alternative<existing_appgenerator_functions>(
		    save_info.value.extra
	    ))
		return;

	auto &parsed_functions=
		std::get<existing_appgenerator_functions>(save_info.value.extra)
		.parsed_functions;

	// A non-empty list ends up creating a new <config>

	if (parsed_functions.empty())
		return;

	auto config=save_info.lock->create_next_sibling()->element({"config"});

	for (const auto &f:parsed_functions)
		f->save(config, info);
}

static const layoutmanager_type_handler_name
layoutmanager_type_handler_name_inst;

static const layoutmanager_type_handler_progressbar
layoutmanager_type_handler_progressbar_inst;


// Implement a setting based on an editable combo-box

// The specific setting subclasses and implement combobox_values() that
// returns a list of values for the standard combo-box.

struct editable_combobox_handler : setting_handler {

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		auto values=combobox_values(info);

		auto combobox=info.value_factory->create_focusable_container
			([&]
			 (const auto &container)
			{
				auto layout=
					container->editable_comboboxlayout();

				layout->replace_all_items(values);

				// Find the current value and make it selected
				// by default.
				auto p=standard_combobox_handler
					::find_string(values,
						      info.value.string_value);

				if (p != values.end())
				{
					layout->autoselect(p-values.begin());
				}
				else
				{
					layout->set(info.value.string_value);
				}
			},
			 x::w::new_editable_comboboxlayoutmanager{});

		// The validator reads the combo-box's selected value
		// and uses it.

		return [combobox](bool alert)
		{
			return parameter_value{
				combobox->editable_combobox_input_field()
				->get_unicode()
			};
		};
	}

	virtual std::vector<x::w::list_item_param
			    > combobox_values(const create_ui_info &info)
		const=0;
};

// A single_value referencing an appearance object.

// Use an editable combo-box for this.

struct appearance_or_base_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Predefined appearances (if any), a separator, then currently
		// defined appearances.

		std::vector<x::w::list_item_param> appearances;

		appinvoke([&]
			  (appObj *me)
		{
			appObj::appearance_info_t::lock lock{
				me->appearance_info
			};

			auto [app_defaults, app_defined]=
				me->appearance_get_by_type(
					lock,
					me->appearance_types.find(
						info.handler_name
					));

			adjust_defaults(app_defaults);

			appearances.reserve(app_defaults.size() +
					    app_defined.size() + 1);

			for (const auto &n:app_defaults)
				appearances.emplace_back(n);

			if (app_defaults.size() && app_defined.size())
				appearances.emplace_back(x::w::separator{});

			for (const auto &n:app_defined)
				appearances.emplace_back(n);

		});
		return appearances;
	}

	virtual void adjust_defaults(std::vector<std::string> &) const
	{
	}
};

static const appearance_or_base_handler appearance_or_base_handler_inst;

// A single_value referencing a defined appearance object

// Override adjust_defaults() to clear the list of default appearance objects.

struct appearance_handler : appearance_or_base_handler {

	void adjust_defaults(std::vector<std::string> &app_defaults)
		const override
	{
		app_defaults.clear();
	}
};


static const appearance_handler appearance_handler_inst;

// A single_value with a color

// Use an editable combo-box.

struct color_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Predefined colors, a separator, then theme colors.

		std::vector<x::w::list_item_param> colors;

		appObj::colors_info_t::lock lock{info.app->colors_info};

		colors.reserve(lock->ids.size() +
			       x::w::n_rgb_colors+1);

		for (size_t i=0; i<x::w::n_rgb_colors; ++i)
			colors.push_back(x::w::rgb_color_names[i]);

		colors.push_back(x::w::separator{});

		for (const auto &c:lock->ids)
			colors.push_back(c);

		return colors;
	}

};

static const color_handler color_handler_inst;

// A single_value containing a dimension

struct dim_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Load the theme dimensions

		std::vector<x::w::list_item_param> dims;

		appObj::dimension_info_t::lock lock{info.app->dimension_info};

		dims.reserve(lock->ids.size());

		for (const auto &c:lock->ids)
			dims.push_back(c);

		return dims;
	}
};

static const dim_handler dim_handler_inst;

// A single_value containing a border

struct border_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Load the theme borders.

		std::vector<x::w::list_item_param> borders;

		appObj::border_info_t::lock lock{info.app->border_info};

		borders.reserve(lock->ids.size());

		for (const auto &c:lock->ids)
			borders.push_back(c);

		return borders;
	}
};

static const border_handler border_handler_inst;

// <element> contains an unsigned value
//
// Input field

struct to_size_t_handler : setting_handler {

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		x::w::input_field_config config{10};

		config.alignment=x::w::halign::right;

		std::optional<size_t> initial_value;

		std::istringstream i{x::locale::base::global()->fromu32(
				info.value.string_value
			)};

		initial_value=0;
		i >> *initial_value;

		if (!i)
			initial_value.reset();

		// Create a size_t validator.

		const auto &[field, validated_input]=
			info.value_factory->create_input_field(
				x::w::create_string_validated_input_field_contents<size_t>(
					[]
					(ONLY IN_THREAD,
					 const std::string &value,
					 std::optional<size_t> &parsed_value,
					 const auto &field,
					 const auto &trigger)
					{
						if (value.empty())
							return;

						if (!parsed_value)
							field.stop_message(
								_("Invalid "
								  "value")
							);
					},
					[]
					(size_t n)
					{
						return std::to_string(n);
					},
					initial_value),
				config);

		return 	[field, validated_input, required=info.required]
			(bool alert)
			{
				auto ret=get_inputfield(field, alert, required);

				if (!ret)
					return ret;

				if (validated_input->value())
					return ret;

				if (alert)
				{
					field->stop_message(_("Invalid input"));
					field->request_focus();
				}

				ret.reset();
				return ret;
			};
	}
};

static const to_size_t_handler to_size_t_handler_inst;

// <element> contains an floating point value
//
// Input field

struct to_mm_handler : setting_handler {
	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		x::w::input_field_config config{10};

		config.alignment=x::w::halign::right;

		auto [field, validator]=info.value_factory->create_input_field(
			x::w::create_validated_input_field_contents(
				[]
				(ONLY IN_THREAD,
				 const std::string &value,
				 const auto &lock,
				 const auto &trigger)
				-> std::optional<std::string>
				{
					if (value.empty())
						// Empty string, valid value.
						return std::string{};

					double v;

					const char *c=value.c_str();

					auto res=std::from_chars(c,
								 c+value.size(),
								 v);

					if (res.ec != std::errc{} ||
					    *res.ptr)
					{
						lock.stop_message
							(_("Enter a numeric"
							   " value"));
						return {};
					}

					if (v < 0)
					{
						lock.stop_message(
							_("Value cannot be "
							  "negative"));
						return {};
					}
					return value;
				},
				[]
				(const std::optional<std::string> &n)
				-> std::string
				{
					if (!n)
						return std::string{};

					double v;

					const char *c=n->c_str();

					auto fres=std::from_chars(c,
								  c+n->size(),
								  v);

					char buffer[40];

					std::to_chars_result res;
					if (fres.ec == std::errc{} &&
					    (res=std::to_chars(
						    buffer,
						    buffer+sizeof(buffer)-1,
						    v,
						    std::chars_format::fixed,
						    1)).ec == std::errc{})
					{
						*res.ptr=0;

						auto p=std::find(buffer,
								 res.ptr,
								 '.');

						if (*p == '.')
						{
							// Remove ".0"/

							auto q=p;

							while (*++q)
							{
								if (*q != '0')
									break;
							}

							if (!*q)
								*p=0;
						}
						return buffer;
					}
					return "";
				},
				x::locale::base::global()->fromu32(
					info.value.string_value
				)
			),
			config);

		return 	[field, validator, required=info.required]
			(bool alert) -> std::optional<parameter_value>
			{
				if (validator->value())
				{
					return get_inputfield(
						field, alert, required
					);
				}
				if (alert)
				{
					field->stop_message(_("Invalid input"));
					field->request_focus();
				}
				return std::nullopt;
			};
	}
};

static const to_mm_handler to_mm_handler_inst;

// <element> contains an integer value 0-100.
//
// Input field

struct to_percentage_t_handler : setting_handler {

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		x::w::input_field_config config{4};

		config.alignment=x::w::halign::right;

		std::optional<size_t> initial_value;

		std::istringstream i{x::locale::base::global()->fromu32(
				info.value.string_value
			)};

		initial_value=0;
		i >> *initial_value;

		if (!i)
			initial_value.reset();

		// Create a size_t validator (for the percentage value).

		// The value for this validator is std::optional<size_t>,
		// and the value is a nullopt if the entered value is
		// an empty string.

		auto contents=x::w::create_validated_input_field_contents(
			[]
			(ONLY IN_THREAD,
			 const std::string &value,
			 const auto &lock,
			 const auto &trigger)
			-> std::optional<std::optional<size_t>>
			{
				if (value.empty())
					// Empty string, valid value.
					return std::optional<size_t>{};

				size_t v;

				const char *c=value.c_str();

				auto res=std::from_chars(c,
							 c+value.size(),
							 v);

				if (res.ec != std::errc{} || v > 100)
				{
					lock.stop_message
						(_("Enter a percentage"
						   " value between"
						   " 0-100."));
					return std::nullopt;
				}
				return std::optional<size_t>{v};

			},
			[]
			(const std::optional<size_t> &n)
			{
				if (!n)
					return std::string{};

				char buffer[40];

				*std::to_chars(buffer,
					       buffer+sizeof(buffer)-1,
					       *n).ptr=0;
				return std::string{buffer};
			},
			initial_value);

		auto [field, validated_input]=
			info.value_factory->create_input_field(
				contents, config
			);

		// Accept only digits.

		field->on_filter([]
				 (ONLY IN_THREAD, const auto &info)
		{
			for (const auto &c:info.new_contents)
				if (c < '0' || c > '9')
					return;
			info.update();
		});

		return 	[field, required=info.required]
			(bool alert)
			{
				return get_inputfield(field, alert, required);
			};
	}
};


static const to_percentage_t_handler to_percentage_t_handler_inst;

// <element> contains a list_selection_type_cb_t
//
// Combo-box

struct to_selection_type_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		return {
			std::begin(x::w::selection_type_str),
			std::end(x::w::selection_type_str),
		};
	}
};

static const to_selection_type_handler to_selection_type_handler_inst;

// <element> specifies an halign
//
// Combo box.

struct to_halign_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		return {
			std::begin(x::w::halign_names),
			std::end(x::w::halign_names),
		};
	}
};

static const to_halign_handler to_halign_handler_inst;

// <element> specifies a valign
//
// Combo box.

struct to_valign_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		return {
			std::begin(x::w::valign_names),
			std::end(x::w::valign_names),
		};
	}
};

static const to_valign_handler to_valign_handler_inst;

// <element> specifies a scrollbar visibility
//
// Combo box.

struct to_scrollbar_visibility_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		return {
			std::begin(x::w::scrollbar_visibility_names),
			std::end(x::w::scrollbar_visibility_names),
		};
	}
};

static const to_scrollbar_visibility_handler to_scrollbar_visibility_handler_inst;

// <element> specifies a shortcut
//
// Input field.

struct shortcut_handler : single_value_handler {

	void saved_element(const setting_save_info &save_info,
			   appgenerator_save &info) const override
	{
		// Set the type parameter on the new saved element.

		save_info.lock->attribute({"type", "theme_text"});
	}
};

static const shortcut_handler shortcut_handler_inst;

// <element> specifies a bidi value
//
// Combo box.

struct to_bidi_direction_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		return {
			std::begin(x::w::bidi_names),
			std::end(x::w::bidi_names),
		};
	}
};

static const to_bidi_direction_handler to_bidi_direction_handler_inst;

// <element> specifies a bidi value
//
// Combo box.

struct to_bidi_directional_format_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		return {
			std::begin(x::w::bidi_format_names),
			std::end(x::w::bidi_format_names),
		};
	}
};

static const to_bidi_directional_format_handler
to_bidi_directional_format_handler_inst;

// <element> specifies a theme text parameter
//
// Input field

struct text_param_value_handler : single_value_handler {

	void saved_element(const setting_save_info &save_info,
			   appgenerator_save &info) const override
	{
		// Set the type parameter on the new saved element.

		save_info.lock->attribute({"type", "theme_text"});
	}

	x::w::input_field_config get_config() const override
	{
		x::w::input_field_config config;

		config.rows=4;

		return config;
	}
};

static const text_param_value_handler text_param_value_handler_inst;

// <element> specifies a font name
//
// Combo box.

struct font_value_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Load the theme borders.

		std::vector<x::w::list_item_param> fonts;

		appObj::font_info_t::lock lock{info.app->font_info};

		fonts.reserve(lock->ids.size());

		for (const auto &c:lock->ids)
			fonts.push_back(c);

		return fonts;
	}
};

static const font_value_handler font_value_handler_inst;

// <element> specifies a rgb name
//
// Combo box.

struct rgb_value_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Load the theme colors.

		std::vector<x::w::list_item_param> colors;

		appObj::colors_info_t::lock lock{info.app->colors_info};

		colors.reserve(x::w::n_rgb_colors+1+lock->ids.size());

		colors.insert(colors.end(),
			      x::w::rgb_color_names,
			      x::w::rgb_color_names + x::w::n_rgb_colors);

		if (!lock->ids.empty())
			colors.emplace_back(x::w::separator{});

		for (const auto &c:lock->ids)
			colors.push_back(c);

		return colors;
	}
};

static const rgb_value_handler rgb_value_handler_inst;

// <element> specifies a factory generator
//
// The name of this handler specifies what type of a factory this is.

struct lookup_factory_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		std::vector<x::w::list_item_param> factories;

		auto all_factories=info.app->current_generators
			->all_generators_for("factory",
					     info.handler_name);

		factories.insert(factories.end(),
				 all_factories.begin(),
				 all_factories.end());

		return factories;
	}

	// Add an extra_save action to automatically create the factory.

	// If the named factory does not exist it will get created.

	void saved_element(const setting_save_info &save_info,
			   appgenerator_save &info) const override;
};

void lookup_factory_handler::saved_element(const setting_save_info &save_info,
					   appgenerator_save &info) const
{
	auto value_string=x::locale::base::global()->fromu32(
		save_info.value.string_value
	);

	info.extra_saves.emplace_back
		([handler_name=save_info.handler_name, value_string]
		 (const x::xml::writelock &lock,
		  appgenerator_save &save)
		{
			appinvoke([&]
				  (appObj *me)
			{
				me->generator_autocreate_layout_or_factory
					(lock, save,
					 "factory",
					 handler_name,
					 value_string);
			});
		});
}

static const lookup_factory_handler lookup_factory_handler_inst;

// <elements> contains 0 or more factory elements.

// Dialog

struct factory_parseconfig_handler : public setting_handler {

	// Common logic to locate the generic factory parser, and use
	// it to initialize an empty parameter_value.
	//
	// The parameter value gets initialized as
	// existing_appgenerator_functions with the generic factory compiler
	// set as its compiler.
	//
	// Returns the generic factory compiler and a std::reference_wrapper
	// to the list of parsed functions, initially empty.

	auto initialize_value(appObj *me,
			      parameter_value &value) const
	{
		auto type_category=get_type_category();

		auto &uicompiler_info=me->current_generators->uicompiler_info;

		auto iter=uicompiler_info->uigenerators_lookup
			.find(type_category);

		if (iter == uicompiler_info->uigenerators_lookup.end())
			throw EXCEPTION("Internal error: "
					"cannot find generic "
					"factory");

		auto &functions =
			value.extra.emplace<existing_appgenerator_functions>(
				*iter
			);

		return std::tuple{*iter,
			std::ref(functions.parsed_functions)};
	}

	// Find the generic factory compiler.

	// Overriden in listlayout_parseconfig_handler and
	// list_items_params_value_handler

	virtual type_category_t get_type_category() const
	{
		return {
			appuigenerator_type::factory,
			"factory"
		};
	}

	// Exactly one widget must be specified

	virtual bool one_only() const
	{
		return true;
	}

	// The contents of this generator are a factory generator
	void load(const x::xml::readlock &lock,
		  parameter_value &value) const override
	{
		appinvoke(
			[&]
			(appObj *me)
			{
				// Use the generic factory to parse the
				// generator and save it in the value.

				const auto &[compiler, functions]=
					initialize_value(me, value);

				auto &uicompiler_info=me->current_generators
					->uicompiler_info;

				functions.get()=compiler->parse(
					lock,
					uicompiler_info->uigenerators
				);
			});
	}

	setting_create_ui_ret_t create_ui(ONLY IN_THREAD,
					  const create_ui_info &info)
		const override
	{
		// instantiate the extra_generators_ui object that holds
		// the generator data.
		//
		// If we already loaded the parsed generator, in
		// info.parameter_value, we'll go with it, otherwise
		// we need to create an empty parameter value, but with
		// the same uicompiler.

		parameter_value default_parameter_value;

		initialize_value(info.app, default_parameter_value);

		auto current_config_info=extra_generators_ui::create(
			std::holds_alternative<existing_appgenerator_functions>(
				info.value.extra
			) ? info.value : default_parameter_value
		);

		auto config_container=
			current_config_info->create_config_container(
				IN_THREAD, info.app, info.mw, info.value_factory
			);

		return [current_config_info, config_container,
			one_only=this->one_only()]
			(bool alert)
		{
			std::optional<parameter_value> ret;

			// Now check what was entered.

			current_functions_t::lock lock{
				current_config_info->current_functions
			};

			auto &[parsed_values, current_generators]=*lock;

			if (current_generators)
			{
				generator_info_lock lock{
					current_generators->generator_info
				};

				auto &value=ret.emplace();

				if (one_only && lock->functions->size() != 1)
				{
					auto glm=config_container->gridlayout();

					x::w::focusable f=glm->get(0, 0);

					if (alert)
					{
						f->request_focus();
						x::w::element{f}->stop_message(
							_("Exactly one widget"
							  " must be specified")
						);
					}
					ret.reset();
					return ret;
				}

				// As tempting as it is to move
				// *lock_functions, a subsequent validator
				// can fail, so we should not do this.
				value.extra.emplace<
					existing_appgenerator_functions
					>(lock->compilerbase,
					  *lock->functions);
			}
			return ret;
		};
	}

	// Override save()
	//
	// If same_xpath only saved_element() gets called, so we jump over
	// there.
	void save(const setting_save_info &save_info,
		  appgenerator_save &info) const override
	{
		save_info.lock->create_child()
			->element({save_info.parameter_name});

		saved_element(save_info, info);

		save_info.lock->get_parent();
	}

	// Override saved_element()
	void saved_element(const setting_save_info &save_info,
			   appgenerator_save &info) const override
	{
		if (std::holds_alternative<existing_appgenerator_functions>
		    (save_info.value.extra))
		{
			auto &appgenerator_functions=
				std::get<existing_appgenerator_functions>(
					save_info.value.extra
				);

			for (auto &f:appgenerator_functions.parsed_functions)
			{
				f->save(save_info.lock, info);
			}
		}
	}
};


static const factory_parseconfig_handler factory_parseconfig_handler_inst;

// <elements> contains 0 or more listlayout elements.

// Use the factory UI for this, specifying the list layoutmanager
// type_category_t.

struct listlayout_parseconfig_handler : factory_parseconfig_handler {

	// Override get_type_category()

	type_category_t get_type_category() const override
	{
		return {
			appuigenerator_type::layoutmanager,
			"list"
		};
	}

	// Can be any number of generators

	bool one_only() const override
	{
		return false;
	}
};

static const listlayout_parseconfig_handler listlayout_parseconfig_handler_inst;

// <elements> contains initializations of list_item_params.

// Use the factory UI for this, specifying the list_items type_category_t.

struct list_items_params_value_handler : factory_parseconfig_handler {

	// Override get_type_category()

	type_category_t get_type_category() const override
	{
		return {
			appuigenerator_type::list_items,
			"list_items"
		};
	}

	// Can be any number of generators

	bool one_only() const override
	{
		return false;
	}
};

static const list_items_params_value_handler
list_items_params_value_handler_inst;

// <elements> occurs multiple times, each occurence contains a string.

// Use a multi-line input field, for now.

struct multiple_values_handler : single_value_handler {

	// Override load_from_parent_element, and do our own thing.

	bool load_from_parent_element(const x::xml::readlock &lock,
				      const parse_parameterObj &parameter,
				      parameter_value &value) const override
	{
		// Parse the multiplie values ourselves, one value per line
		// in the string value.

		auto copy=lock->clone();

		auto xpath=copy->get_xpath(parameter.parameter_name);

		size_t n=xpath->count();

		std::u32string str;

		for (size_t i=1; i<=n; ++i)
		{
			xpath->to_node(1);
			str += copy->get_u32text();
			str += U"\n";
		}

		value.string_value=std::move(str);
		return true;
	}

	x::w::input_field_config get_config() const override
	{
		x::w::input_field_config c;

		c.rows=4;
		return c;
	}

	void save(const setting_save_info &save_info, appgenerator_save &info)
		const override
	{
		std::vector<std::u32string> v;

		x::strtok_str(save_info.value.string_value, U"\n", v);

		for (auto w:v)
		{
			w=x::trim(w);

			if (w.empty())
				continue;

			save_info.lock->create_child()
				->element({save_info.parameter_name})
				->text(w)->parent()->parent();
		}

		saved_element(save_info, info);
		save_info.lock->get_parent();
	}
};

static const multiple_values_handler multiple_values_handler_inst;

// List layout style

// Use a multi-line input field, for now.

struct listlayoutstyle_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		return {
			"highlight",
			"bullet",
		};
	};
};

static const listlayoutstyle_handler listlayoutstyle_handler_inst;

const std::unordered_map<std::string_view,
			 const setting_handler *> setting_handler::member_types{
	{ "optional_constant<true>", &checkbox_handler_inst },
	{ "optional_constant<false>", &checkbox_handler_inst },
	{ "single_value", &single_value_handler_inst },
	{ "compiler.generators->lookup_color", &color_handler_inst },
	{ "compiler.generators->lookup_dim", &dim_handler_inst },
	{ "to_size_t", &to_size_t_handler_inst },
	{ "to_mm", &to_mm_handler_inst },
	{ "to_halign", &to_halign_handler_inst },
	{ "to_valign", &to_valign_handler_inst },
	{ "to_scrollbar_visibility", &to_scrollbar_visibility_handler_inst },
	{ "to_bidi_direction", &to_bidi_direction_handler_inst },
	{ "to_bidi_directional_format",
	  &to_bidi_directional_format_handler_inst },
	{ "compiler.text_param_value", &text_param_value_handler_inst },
	{ "compiler.font_value", &font_value_handler_inst },
	{ "compiler.rgb_value", &rgb_value_handler_inst },
	{ "method_call", &checkbox_handler_inst },
	{ "lookup_appearance", &appearance_handler_inst },
	{ "lookup_appearance_base", &appearance_or_base_handler_inst },
};

const std::unordered_map<std::string_view,
			 const setting_handler *
			 > setting_handler::parameter_types{

	{ "optional_constant<true>", &checkbox_handler_inst },
	{ "optional_constant<false>", &checkbox_handler_inst },
	{ "single_value", &single_value_handler_inst },

	// Aliases for single value.
	{ "compiler.lookup_scrollbar_type", &single_value_handler_inst },
	{ "compiler.lookup_container_generators(name)",
	  &layoutmanager_type_handler_name_inst },
	{ "compiler.lookup_container_generators(progressbar)",
	  &layoutmanager_type_handler_progressbar_inst },
	{ "restore_panelayoutmanager_position", &single_value_handler_inst },
	{ "restore_tablelayoutmanager_position", &single_value_handler_inst },

	{ "compiler.generators->lookup_color", &color_handler_inst },
	{ "compiler.generators->lookup_dim", &dim_handler_inst },
	{ "compiler.generators->lookup_border", &border_handler_inst },

	{ "single_value_exists", &checkbox_handler_inst},
	{ "to_size_t", &to_size_t_handler_inst },
	{ "to_mm", &to_mm_handler_inst },
	{ "to_halign", &to_halign_handler_inst },
	{ "to_valign", &to_valign_handler_inst },
	{ "to_percentage_t", &to_percentage_t_handler_inst },
	{ "to_selection_type", &to_selection_type_handler_inst },
	{ "to_scrollbar_visibility", &to_scrollbar_visibility_handler_inst },

	{ "multiple_values", &multiple_values_handler_inst },

	{ "compiler.list_items_param_value",
	  &list_items_params_value_handler_inst },
	{ "lookup_factory", &lookup_factory_handler_inst },
	{ "compiler.shortcut_value", &shortcut_handler_inst },
	{ "compiler.text_param_value", &text_param_value_handler_inst },

	{ "compiler.factory_parseconfig",
	  &factory_parseconfig_handler_inst },

	{ "compiler.listlayout_parseconfig",
	  &listlayout_parseconfig_handler_inst },

	{ "lookup_appearance", &appearance_handler_inst },

	{ "listlayoutstyle", &listlayoutstyle_handler_inst },
};

////////////////////////////////////////////////////////////////////////////
//
// Parse a <parameter> specification. Most of the work happens in the
// uicompiler_generator_collection superclass. No work happens in
// the constructor. parse_function instantiates each parameter object then
// calls define_parameter().

parse_parameterObj::parse_parameterObj(const std::string &extra_single_value)
	: parameter_name{extra_single_value},
	  handler{&single_value_handler_inst}
{
}

////////////////////////////////////////////////////////////////////////////
//
// Parse a <function> specification.

parse_function::parse_function(const x::xml::readlock &root)
{
	parsed_parameters.reserve(root->get_xpath("parameter")
				  ->count() +
				  root->get_xpath("parameter/lookup/parameter")
				  ->count() + 3);

	std::vector<parse_parameter> extra_parameters;

	std::unordered_set<std::string> all_parsed_parameters;

	size_t same_xpath=0;

	bool new_element_or_container=false;

	std::string condition_name, condition_value, condition_exists;

	// Go through what's in a <function>.

	for (bool flag=root->get_first_element_child(); flag;
	     flag=root->get_next_element_sibling())
	{
		auto parameter=root->clone();

		auto name=root->name();

		if (name == "name")
		{
			this->name=parameter->get_text();
			continue;
		}

		if (name == "invoke" || name == "object" ||
		    name == "before_invocation" ||
		    name == "after_invocation" ||
		    name == "parameter_parser_name")
			continue;

		if (name == "condition")
		{
			// Figure out what's here and initialize the
			// condition_value.

			if (!condition_name.empty() ||
			    !condition_exists.empty())
				throw EXCEPTION("Multiple <condition>"
						" specifications");

			for (bool flag=parameter
				     ->get_first_element_child(); flag;
			     flag=parameter->get_next_element_sibling())
			{
				auto condition=parameter->clone();

				name = condition->name();

				if (name == "name")
				{
					condition_name=
						condition->get_text();
					continue;
				}
				if (name == "value")
				{
					condition_value=
						condition->get_text();
					continue;
				}
				if (name == "exists")
				{
					condition_exists=
						condition->get_text();
					continue;
				}
				throw EXCEPTION("Unknown <condition> "
						"node: "
						<< name);
			}

			if (!condition_name.empty() &&
			    !condition_exists.empty())
			{
				throw EXCEPTION("Cannot have both "
						"<condition> <name> "
						"and <exists>");
			}

			if (!condition_name.empty() &&
			    condition_value.empty())
			{
				throw EXCEPTION("Did not find a "
						"<condition> <value>");
			}

			continue;
		}

		if (name == "new_element" ||
		    name == "new_container")
		{
			// If <tooltip> exists, it names
			// a <tooltip> element with a <label>.

			// If <elements> exists, it names a
			// <factory type="elements"

			// If <context> exists, it contains
			//
			// - An optional <shortcut>
			// - A <menu> containing parsed
			//!   name=listlayout layoutmanager parser.

			new_element_or_container=true;
			continue;
		}

		if (name != "parameter")
		{
			throw EXCEPTION("Unknown <function> node: "
					<< name);
		}

		auto parsed_parameter=parse_parameter::create();

		parsed_parameter->define_parameter(parameter, extra_parameters);

		if (parsed_parameter->scalar_parameter)
			continue;

		if (!all_parsed_parameters.insert(
			    parsed_parameter->parameter_name
		    ).second)
		{
			throw EXCEPTION("Duplicate parameter: "
					<< parsed_parameter->parameter_name);
		}

		if (parsed_parameter->same_xpath)
			++same_xpath;
		parsed_parameters.push_back(parsed_parameter);
	}

	// Review the extra parameters we collected

	// Nothing more needs to be done if a regular parameter with that name
	// was specified, this directive will piggy-back on top of it.
	//
	// Otherwise we add it as an additional parameter.

	for (auto &p: extra_parameters)
	{
		if (all_parsed_parameters.find(p->parameter_name) !=
		    all_parsed_parameters.end())
		{
			// container specifies a lookup for a <name>,
			// but also formally defines it as a parameter.
			continue;
		}
		// create_progressbar looks for a <progressbar> value, add it
		// as a parameter.

		parsed_parameters.push_back(p);
	}

	if (new_element_or_container)
	{
		if (same_xpath)
			throw EXCEPTION("Cannot specify xpath for a widget");

		// Add an "id" field as the first parameter, sets the "id"
		// attribute.
		auto id_parameter=parse_parameter::create();

		id_parameter->parameter_name="id";
		id_parameter->handler_name="id";
		id_parameter->handler=&id_handler_inst;
		id_parameter->is_optional=true;

		parsed_parameters.insert(parsed_parameters.begin(),
					 id_parameter);

		// Append the "elements" parameter, optional elements factory
		// and an optonal tooptip string.
		auto elements_parameter=parse_parameter::create();
		auto tooltip_parameter=parse_parameter::create();

		elements_parameter->parameter_name="elements";
		elements_parameter->handler_name="elements";
		elements_parameter->handler=&lookup_factory_handler_inst;
		elements_parameter->is_optional=true;

		parsed_parameters.push_back(elements_parameter);

		tooltip_parameter->parameter_name="tooltip";
		tooltip_parameter->handler_name="tooltip";
		tooltip_parameter->handler=&text_param_value_handler_inst;
		tooltip_parameter->is_optional=true;
		parsed_parameters.push_back(tooltip_parameter);
	}

	parsed_parameters.shrink_to_fit();
	if (same_xpath > 1 ||
	    (same_xpath && (!condition_name.empty()
			    || !condition_exists.empty())))
		throw EXCEPTION("Cannot have multiple <xpath>s or both "
				"<xpath> and a condition");

	if (!condition_name.empty())
		condition=std::tuple{condition_name, condition_value};

	if (!condition_exists.empty())
		condition=std::tuple{
			condition_exists,
			all_parsed_parameters.find(condition_exists)
			!= all_parsed_parameters.end()};

	if (name.empty())
		throw EXCEPTION("Did not parse <function> <name>");
}

bool parse_function::is_parameter() const
{
	return false;
}

std::string parse_function::get_name() const
{
	return name;
}

void parse_function::save(const x::xml::writelock &lock,
			  const std::vector<parameter_value> &parameter_values,
			  appgenerator_save &info)
	const
{
	auto n=lock->create_child();

	n->element({name});

	std::visit(x::visitor{
			[](no_condition)
			{
			},
			[&](const std::tuple<std::string, std::string>
			    &name_and_value)
			{
				// Since this function specifies a conditional
				// element, with a particular value, we add
				// it here.

				const auto &[name, value]=name_and_value;

				n->element({name});
				n->text(value);

				lock->get_parent();
				lock->get_parent();
			},
			[&](const std::tuple<std::string, bool>
			    &condition_exists)
			{
				// We'll check later, after generating all
				// the object parameters.
			}}, condition);

	if (parsed_parameters.size() != parameter_values.size())
	{
		throw EXCEPTION("Internal error, wrong parameter value count "
				"for " << name << "\nParameters:\n"
				<< ({
						std::ostringstream o;

						for (const auto &p:
							     parsed_parameters)
						{
							o << "    "
							  << p->parameter_name
							  << " ("
							  << p->handler_name
							  << ")\n";
						}
						o.str();
					})
				<< "Values:\n"
				<< ({
						std::ostringstream o;

						for (const auto &v:
							     parameter_values)
						{
							o << "    ["
							  << x::locale
								::base::global()
								->fromu32
								(v.string_value)
							  << "]\n";
						}

						o.str();
					})
		);
	}

	auto v=parameter_values.begin();

	for (const auto &p:parsed_parameters)
	{
		p->save(lock, *v++, false, info);
	}

	std::visit(x::visitor{
			[](no_condition)
			{
			},
			[&](const std::tuple<std::string, std::string>
			    &name_and_value)
			{
			},
			[&](const std::tuple<std::string, bool>
			    &condition_exists)
			{
				const auto &[name, value]=condition_exists;

				// Now that everything is saved, we can verify
				// that we have the condition included, if
				// not create an empty one.

				auto check_xpath=lock->get_xpath(name);

				if (check_xpath->count() == 0)
				{
					auto n=lock->create_child();

					n->element({name});
					lock->get_parent();
				}
			}}, condition);

	lock->get_parent();
}

x::w::text_param parse_function::description(description_format fmt) const
{
	x::w::text_param t;

	auto parameter_font = "list; weight=bold"_font;

	switch (fmt) {
	case description_format::list:
		break;
	case description_format::title:
		t("label; scale=2"_theme_font);
		parameter_font = "label; weight=bold; scale=2"_font;
		break;
	}

	t(name);

	std::visit(x::visitor{
			[](const no_condition &)
			{
			},[&](const condition_name_and_value &name_value)
			{
				auto &[name, value]=name_value;
				t(parameter_font);
				t(" <");
				t(name);
				t(">");
				t(value);
				t("</");
				t(name);
				t(">");
			},[&](const condition_exists exists_node)
			{
				auto &[name, has_value]=exists_node;
				t(parameter_font);
				t(" <");
				t(name);
				if (has_value)
				{
					t(">...<");
					t(name);
				}
				t(">");
			}
		}, condition);

	return t;
}

namespace {
#if 0
}
#endif

// Parsed object members.

// The dialog for editing object members uses a discrete object to store
// the generators for the object. This object stores the list of the parsed
// appgenerator_function-s, as well as the appgenerators_functions object that
// handles their UI.

typedef x::mpobj<std::tuple<std::vector<const_appgenerator_function>,
			    appgenerator_functionsptr>
		 > object_latest_functions_t;

struct object_latest_functionsObj : virtual public x::obj {

	object_latest_functions_t functions;
};

typedef x::ref<object_latest_functionsObj> object_latest_functions;

// Implement appgenerator_functionObj virtual methods

// Implements appgenerator_functionObj virtual methods for a generator
// for a UI function call.

struct appgenerator_function_implObj : public appgenerator_functionObj {

	// The function being implemented here
	const parse_function *function;

	// The values for the function's parameters.

	// Same size as the function's parsed_parameters vector
	x::vector<parameter_value> parameter_values;

	appgenerator_function_implObj(
		const parse_function *function,
		std::vector<parameter_value> &&parameter_values
	) : function{function},
	    parameter_values{x::vector<parameter_value>::create(
			    std::move(parameter_values)
		    )
	    }
	{
		if (this->parameter_values->size() !=
		    function->parsed_parameters.size())
			throw EXCEPTION("Interal error: wrong number of values "
					"for a new function");
	}

	~appgenerator_function_implObj()=default;

	appgenerator_function clone() const override
	{
		std::vector<parameter_value> cloned_parameter_values{
			*parameter_values
		};

		for (auto &c:cloned_parameter_values)
			c.cloned();

		return x::ref<appgenerator_function_implObj>::create
			(function, std::move(cloned_parameter_values));
	}

	x::w::text_param description(description_format fmt) const override
	{
		return function->description(fmt);
	}

	generator_create_ui_ret_t create_ui(ONLY IN_THREAD,
					    const x::w::main_window &mw,
					    const x::w::gridlayoutmanager &glm)
		const override
	{
		auto ret=appinvoke([&]
				   (appObj *me)
		{
			return create_ui(IN_THREAD, me, mw, glm);
		});

		if (!ret)
		{
			// Fail gracefully, as if everything failed
			// validation, so let the cheaps fall as they may.
			ret.emplace([]
				    {
					    return false;
				    });
		}

		return *ret;
	}

	generator_create_ui_ret_t create_ui(ONLY IN_THREAD, appObj *app,
					    const x::w::main_window &mw,
					    const x::w::gridlayoutmanager &glm)
		const;

	static appgenerator_functions create_object_ui(
		appObj *me,
		ONLY IN_THREAD,
		const x::w::main_window &mw,
		const parse_parameter &p,
		const object_latest_functions &latest_functions,
		const x::w::gridfactory &f);

	void save(const x::xml::writelock &lock,
		  appgenerator_save &info) const override
	{
		function->save(lock, *parameter_values, info);
	}
};

generator_create_ui_ret_t appgenerator_function_implObj::create_ui(
	ONLY IN_THREAD, appObj *app,
	const x::w::main_window &mw,
	const x::w::gridlayoutmanager &glm)
	const
{
	if (parameter_values->size()
	    != function->parsed_parameters.size())
	{
		throw EXCEPTION("Internal error: generator function"
				" parameter count mismatch");
	}

	// Collect all parameter values here.

	auto get_values = x::vector<setting_create_ui_ret_t>::create();

	// Create the same number of get_values as there are
	// parsed_parameters.

	get_values->reserve(parameter_values->size());

	// Create the UI for a single parameter
	//
	// An object parameter's <member>s generate the same UI for
	// each object member. This is a common closure that, given
	// a parameter and its value, generates the row in the grid
	// for this parameter (actual one, or an object member).

	auto create_ui_for=[&IN_THREAD, &mw, app]
		(const auto &get_values,
		 const auto &glm,
		 const auto &p,
		 auto &v)
	{
		auto f=glm->append_row();

		f->halign(x::w::halign::right);
		f->valign(x::w::valign::middle);

		// Create a meaningful label
		auto s=p->parameter_name + ":";

		s[0] = x::chrcasecmp::toupper(s[0]);

		f->create_label(s);

		f->valign(x::w::valign::middle);

		if (!p->handler)
			throw EXCEPTION("Internal error: handler not "
					"set for " << p->handler_name);

		// Call each parameters create_ui() and capture
		// all parameters' validators in get_values.
		get_values->push_back(p->handler->create_ui(IN_THREAD, {
					app,
					mw,
					f,
					glm,
					v,
					!p->is_optional,
					p->parameter_name,
					p->handler_name,
				}));
	};

	// Handles processing of the UI elements.
	//
	// Gets called with get_values, what create_ui_for() populates,
	// and parameter_values, the values of the corresponding parameters.
	//
	// If all UI handlers pass validation the values get replaced by the
	// new values.
	//
	// Also is used to process object member parameters.

	auto process_ui_results=[]
		(const auto &get_values,
		 const auto &parameter_values)
	{
		std::vector<parameter_value> new_values;

		new_values.reserve(parameter_values->size());

		// Ok, we will now call each parameter's validator,
		// and then we'll update the parameter_values with
		// what's been edited.

		for (auto &get_field:*get_values)
		{
			auto new_value=get_field(true);

			if (!new_value)
				return false;
			new_values.push_back(*new_value);
		}

		(*parameter_values)=std::move(new_values);

		return true;
	};

	size_t i=0;

	for (const auto &p:function->parsed_parameters)
	{
		if (p->is_object_parameter)
		{
			// For an object parameter with <member>s we'll
			// create a separate container members' generators.

			auto f=glm->append_row();

			f->halign(x::w::halign::right);
			f->valign(x::w::valign::middle);

			x::w::text_param title;

			title("mono"_theme_font);
			title("<");
			title(p->parameter_name);
			title(">:");
			f->create_label(title);

			// Capture the most recent set of generator functions
			// for the members. The popup dialog updates the
			// generator functions.
			//
			// Initialize with the current list of generators for
			// this object, if any.

			auto latest_functions=object_latest_functions::create();

			{
				object_latest_functions_t::lock lock{
					latest_functions->functions
				};

				std::get<0>(*lock) =
					std::get<std::vector<
						const_appgenerator_function>>(
							parameter_values
							->at(i).extra
						);
			}

			f->valign(x::w::valign::middle);

			auto functions=create_object_ui(
				app, IN_THREAD,  mw, p, latest_functions,
				f
			);

			// Collect the final list of generator functions for
			// this object parameter. This gets fetched out of
			// the latest_function object.

			get_values->push_back(
				[functions]
				(bool alert) -> std::optional<parameter_value>
				{
					std::optional<parameter_value> ret;

					ret.emplace();

					generator_info_lock gen_lock{
						functions->generator_info
					};

					ret->extra.emplace<
						std::vector<
							const_appgenerator_function>
						>(*gen_lock->functions);
					return ret;
				});
		}
		else
		{
			create_ui_for(get_values, glm, p,
				      parameter_values->at(i));

		}
		++i;
	}

	return [get_values, parameter_values=this->parameter_values,
		process_ui_results]
	{
		return process_ui_results(get_values, parameter_values);
	};
}

// Implement the uicompiler_generators interface for object members.

// Implement available_functions() by returning a sorted list of all
// appgenerator_functions, which is already stored in the parameter's
// parsed_parameter_functions, we just give them in the same order as
// the member parsed_parameters.

struct object_config_generatorsObj : public uicompiler_generatorsbaseObj {

	// My object parameter.

	const parse_parameter p;

	object_config_generatorsObj(const parse_parameter &p) : p{p}
	{
	}

	compiler_available_functions_t available_functions() const override
	{
		compiler_available_functions_t functions;

		functions.reserve(p->parsed_parameters.size());

		for (const auto &pp:p->parsed_parameters)
		{
			auto iter=p->parsed_parameter_functions.find(
				pp->get_name()
			);

			if (iter == p->parsed_parameter_functions.end())
				throw EXCEPTION("Internal error: cannot find \""
						<< pp->get_name()
						<< "\" parameter");

			functions.emplace_back(iter->second, iter->first);
		}

		return functions;
	}
};

// Create a popup dialog for an object parameter, and show it.

appgenerator_functions appgenerator_function_implObj::create_object_ui(
	appObj *me,
	ONLY IN_THREAD,
	const x::w::main_window &mw,
	const parse_parameter &p,
	const object_latest_functions &latest_functions,
	const x::w::gridfactory &f)
{
	object_latest_functions_t::lock lock{
		latest_functions->functions
	};

	auto &[existing_functions, functions] = *lock;

	// Use create_config_ui to set up the UI for the
	// generators, and save the returned
	// appgenerator_functions in the latest_functions
	// object.

	return extra_generators_uiObj::create_config_ui(
		IN_THREAD,
		me,
		mw,
		{},
		mw,
		{
			x::ref<object_config_generatorsObj>::create(p),
			{ existing_functions.begin(),
			  existing_functions.end()}
		},
		f
	);
}

#if 0
{
#endif
};

bool parse_function::try_parse_function(
	const x::xml::readlock &root,
	std::vector<const_appgenerator_function> &parsed
) const
{
	std::vector<parameter_value> parameter_values;

	bool ret=parse_function_or_object_generator(root, parameter_values);

	if (ret)
		parsed.push_back(create(std::move(parameter_values)));

	return ret;
}

appgenerator_function parse_function::create() const
{
	std::vector<parameter_value> values{parsed_parameters.size()};

	prepare_object_member_values(values);

	return create(std::move(values));
}

appgenerator_function parse_function::create(std::vector<parameter_value>
					     &&args) const
{
	return x::ref<appgenerator_function_implObj>::create
		(this, std::move(args));
}

uicompiler_generatorsObj::uicompiler_generatorsObj()
	: impl{x::ref<implObj>::create()}
{
}

uicompiler_generatorsObj::~uicompiler_generatorsObj()=default;

size_t generator_type_spec_hash::operator()(const const_uicompiler_generators
					    &a)
	const noexcept
{
	return operator()(a->type_category);
}

size_t generator_type_spec_hash::operator()(const type_category_t &a)
	const noexcept
{

	return std::hash<std::string>{}(a.category)
					       + static_cast<size_t>(a.type);
}

generator_type_spec_equ::arg::arg(const const_uicompiler_generators &a)
	: c{a->type_category}
{
}

generator_type_spec_equ::arg::arg(const type_category_t &a)
	: c{a}
{
}

bool generator_type_spec_equ::operator()(arg a, arg b)
	const noexcept
{
	return a.c.type == b.c.type && a.c.category == b.c.category;
}

void uicompiler_generatorsObj::initialize(const x::xml::readlock &root,
					  const uigenerators_t &generators)
{
	// Figure out my <type>

	auto type_str=root->get_any_attribute("type");

	if (type_str == "elements")
		; // Default
	else if (type_str == "list_items")
	{
		type_category.type=appuigenerator_type::list_items;
	}
	else if (type_str == "factory")
	{
		type_category.type=appuigenerator_type::factory;
	}
	else if (type_str == "layoutmanager")
	{
		type_category.type=appuigenerator_type::layoutmanager;
	}
	else if (type_str == "new_layoutmanager")
	{
		type_category.type=appuigenerator_type::new_layoutmanager;
	}
	else
		throw EXCEPTION("Internal error, unknown parser type: "
				<< type_str);

	// Go through all the elements in this <parser> and deal with them.

	for (bool flag=root->get_first_element_child(); flag;
	     flag=root->get_next_element_sibling())
	{
		auto node=root->clone();

		auto name=node->name();

		if (name == "name")
		{
			// The name of this function

			this->name=node->get_text();
			continue;
		}

		if (name == "parameter")
			// Some internal parameter, not specified in the
			// theme file
			continue;

		if (name == "category")
		{
			// The name of this function
			type_category.category=node->get_text();
			continue;
		}

		if (name == "config")
		{
			// The generator for its <config>

			auto config=node->get_text();

			auto iter=generators.find(config);

			if (iter == generators.end())
				throw EXCEPTION("Cannot find <config> "
						<< node->get_text());

			this->config=&*iter->second;
			continue;
		}

		if (name == "use_common")
		{
			// Effectively this generator's "superclass".

			auto parent=node->get_text();

			auto iter=generators.find(parent);

			if (iter == generators.end())
				throw EXCEPTION("Cannot find <use_common> "
						<< node->get_text());

			this->parent=&*iter->second;
			continue;
		}
		if (name != "function")
			throw EXCEPTION("Unknown parser node: " << name);

		parse_function parsed_function{node};

		std::string function_name=parsed_function.name;

		impl->parsed_functions.emplace(function_name,
					       std::move(parsed_function));
	}
}

std::vector<const_appgenerator_function>
uicompiler_generatorsObj::parse(const x::xml::readlock &root,
				const uigenerators_t &all_generators) const
{
	std::vector<const_appgenerator_function> parsed;

	// Parse each element.

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		// We try to parse it using our parsed_function. If
		// we don't succeed, we'll then switch to our parent,
		// and try again.

		auto source=this;

		while (source)
		{
			auto [b, e] = source->impl->parsed_functions
				.equal_range(root->name());

			// Try a each parser for this XML element.

			while (b != e)
			{
				if (b->second.try_parse_function(
					    root->clone(),
					    parsed
				    ))
				{
					break;	// Parsed succesfully
				}
				++b;
			}

			if (b != e)
				break; // Parsed successfully

			source=source->parent;
		}

		if (!source) // Couldn't parse it.
		{
			throw EXCEPTION("Cannot parse <" << root->name()
					<< ">");
		}
	}
	return parsed;
}

std::vector<std::tuple<const_appgenerator_function, x::w::text_param>
	    > uicompiler_generatorsObj::available_functions() const
{
	std::vector<std::tuple<const_appgenerator_function,
			       x::w::text_param>> list;

	// Grab all parsed_functions from me, and my parent.

	for (auto source=this; source; source=source->parent)
	{
		for (const auto &[label, function] :
			     source->impl->parsed_functions)
		{
			list.emplace_back(function.create(),
					  function.description
					  (description_format::list));
		}
	}

	// Sort the list, by name.
	std::sort(list.begin(),
		  list.end(),
		  []
		  (const auto &a, const auto &b)
		  {
			  const auto &[functiona, descra]=a;
			  const auto &[functionb, descrb]=b;

			  return descra.string < descrb.string;
		  });
	return list;
}

const char *type_category_t::xml_node_name() const
{
	switch (type) {
	case appuigenerator_type::elements:
		return "factory";
	case appuigenerator_type::factory:
		return "factory";
	case appuigenerator_type::layoutmanager:
		return "layout";
	default:
		throw EXCEPTION("xml_node_name() is defined only for "
				"elements, factories and layoutmanagers");
	}
};
