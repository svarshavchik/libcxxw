/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "creator/uicompiler_generators_impl.H"
#include "creator/appgenerator_function.H"
#include "creator/appgenerator_functions.H"
#include "creator/appgenerator_save.H"
#include "creator/uicompiler.H"
#include "creator/app.H"
#include "x/w/font_literals.H"
#include "x/w/factory.H"
#include "x/w/input_field.H"
#include "x/w/input_field_config.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/uigenerators.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/text_param_literals.H"
#include "x/w/alignment.H"
#include <x/xml/xpath.H>
#include <x/xml/readlock.H>
#include <x/exception.H>
#include <x/functionalrefptr.H>
#include <x/vector.H>
#include <x/chrcasecmp.H>
#include <x/visitor.H>
#include <x/locale.H>
#include <cstring>
#include <string>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <optional>
#include <courier-unicode.h>

using namespace std::literals::string_literals;

struct create_ui_info {
	const std::u32string &value;
	const std::string &handler_name;
};

typedef x::functionref< std::optional<std::u32string> ()
			> setting_create_ui_ret_t;

// Setting-specific behavior.

// Each possible type of a setting implements create_ui() which receives
// the setting's current value.
//
// create_ui() should use the given factory to create a single widget for
// the setting's value.
//
// It returns a closure that gets called to retrieve the setting's (possibly)
// updated value. It should return std::nullopt if the setting's value failed
// validation.
//
// saved_element() gets called after the element, and its value, are saved
// The write lock is positioned at the saved element's XML node.
//
// This is used by the text_param setting to add type="theme_text" attribute.

struct setting_handler {

	virtual setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
						  const create_ui_info &info)
		const=0;

	virtual void saved_element(const x::xml::writelock &lock,
				   const std::string &handler_name,
				   const std::u32string &value,
				   appgenerator_save &info) const
	{
	}
};

// <element> existence, nothing more.
// Checkbox.

struct checkbox_handler : setting_handler {

	setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
					  const create_ui_info &info)
		const override
	{
		throw EXCEPTION( "Checkbox handler unimplemented, for "
				 << info.handler_name << " (value "
				 << x::locale::base::global()
				 ->fromu32(info.value)
				 << ")");
	}
};

static const checkbox_handler checkbox_handler_inst;

struct unimplemented_handler : setting_handler {

	setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
					  const create_ui_info &info)
		const override
	{
		throw EXCEPTION( "Handler not implemented for "
				 << info.handler_name << " (value "
				 << x::locale::base::global()
				 ->fromu32(info.value)
				 << ")");
	}
};

// <element> contains an opaque value, not interpreted any further.
//
// Input field

struct single_value_handler : setting_handler {

	setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
					  const create_ui_info &info)
		const override
	{
		auto field=f->create_input_field(info.value);

		return [field]
		{
			return field->get_unicode();
		};
	}
};

static const single_value_handler single_value_handler_inst;

// Implement a setting based on a standard combo-box

// The specific setting subclasses and implement combobox_values() that
// returns a list of values for the standard combo-box.

struct standard_combobox_handler : setting_handler {

	// Helper for searching list item values for a specific u32string.

	static std::vector<x::w::list_item_param>
	::const_iterator find_string(const std::vector<x::w::list_item_param>
				     &values,
				     const std::u32string &value)
	{
		auto lower_value=unicode::tolower(value);

		return std::find_if
			(values.begin(),
			 values.end(),
			 [&]
			 (const auto &list_item)
			 {
				 const x::w::list_item_param::variant_t
					 &v=list_item;

				 return std::visit(x::visitor{
						 [&](const x::w::text_param &s)
						 {
							 return unicode::
								 tolower
								 (s.string)
								 == lower_value;
						 }, [&](const auto &v)
						 {
							 return false;
						 }
					 }, v);
			 });



	}

	setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
					  const create_ui_info &info)
		const override
	{
		auto values=combobox_values(info);

		// Empty string value, combobox value is optional

		values.insert(values.begin(), "");

		auto combobox=f->create_focusable_container
			([&]
			 (const auto &container)
			{
				auto layout=
					container->standard_comboboxlayout();

				layout->replace_all_items(values);

				// Find the current value and make it selected
				// by default.
				//
				// We inserted an empty string, so it should
				// pick it up for an unset value. Loading
				// the theme file should've validated all
				// values, but failsafe to autoselecting
				// the empty string, if we don't find this
				// value.

				auto p=find_string(values, info.value);
				size_t i=0;

				if (p != values.end())
					i=p-values.begin();
				layout->autoselect(i);
			},
			 x::w::new_standard_comboboxlayoutmanager{});

		// The validator reads the combo-box's selected value
		// and uses it.

		return [combobox,
			values=std::move(values)]
			() -> std::optional<std::u32string>
		{
			auto selected=combobox->standard_comboboxlayout()
				->selected();

			if (selected && *selected < values.size())
			{
				const x::w::list_item_param::variant_t &v=
					values[*selected];

				if (std::holds_alternative<x::w::text_param>(v))
					return std::get<x::w::text_param>(v)
						.string;
			}
			combobox->stop_message(_("Selection required"));
			combobox->request_focus();
			return std::nullopt;
		};
	}

	virtual std::vector<x::w::list_item_param>
	combobox_values(const create_ui_info &info) const=0;
};

// This element is a type that specifies a layout manager.

struct layoutmanager_type_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param
		    > combobox_values(const create_ui_info &info) const override
	{
		std::vector<x::w::list_item_param> layout_managers;

		appinvoke([&]
			  (appObj *me)
		{
			for (const auto &[name, compiler]
				     : me->current_generators->uicompiler_info
				     ->uigenerators)
			{
				if (compiler->type_category.type !=
				    appuigenerator_type::layoutmanager)
					continue;

				auto &s=compiler->type_category.category;
				layout_managers.push_back(std::u32string{
						s.begin(),
						s.end()
					});
			}
		});

		return layout_managers;
	}
};

static const layoutmanager_type_handler layoutmanager_type_handler_inst;

// Implement a setting based on an editable combo-box

// The specific setting subclasses and implement combobox_values() that
// returns a list of values for the standard combo-box.

struct editable_combobox_handler : setting_handler {

	setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
					  const create_ui_info &info)
		const override
	{
		auto values=combobox_values(info);

		auto combobox=f->create_focusable_container
			([&]
			 (const auto &container)
			{
				auto layout=
					container->editable_comboboxlayout();

				layout->replace_all_items(values);

				// Find the current value and make it selected
				// by default.
				auto p=standard_combobox_handler
					::find_string(values, info.value);

				if (p != values.end())
				{
					layout->autoselect(p-values.begin());
				}
				else
				{
					layout->set(info.value);
				}
			},
			 x::w::new_editable_comboboxlayoutmanager{});

		// The validator reads the combo-box's selected value
		// and uses it.

		return [combobox]
		{
			return combobox->editable_comboboxlayout()
				->get_unicode();
		};
	}

	virtual std::vector<x::w::list_item_param
			    > combobox_values(const create_ui_info &info)
		const=0;
};

// A single_value containing a color

struct color_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Predefined colors, a separator, then theme colors.

		std::vector<x::w::list_item_param> colors;

		appinvoke([&]
			  (appObj *me)
		{
			appObj::colors_info_t::lock lock{me->colors_info};

			colors.reserve(lock->ids.size() +
				       x::w::n_rgb_colors+1);

			for (size_t i=0; i<x::w::n_rgb_colors; ++i)
				colors.push_back(x::w::rgb_color_names[i]);

			colors.push_back(x::w::separator{});

			for (const auto &c:lock->ids)
				colors.push_back(c);
		});

		return colors;
	}

};

static const color_handler color_handler_inst;

// A single_value containing a dimension

struct dim_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Predefined colors, a separator, then theme colors.

		std::vector<x::w::list_item_param> dims;

		appinvoke([&]
			  (appObj *me)
		{
			appObj::dimension_info_t::lock lock{me->dimension_info};

			dims.reserve(lock->ids.size());

			for (const auto &c:lock->ids)
				dims.push_back(c);
		});

		return dims;
	}
};

static const dim_handler dim_handler_inst;

// A single_value containing a border

struct border_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// Predefined colors, a separator, then theme colors.

		std::vector<x::w::list_item_param> borders;

		appinvoke([&]
			  (appObj *me)
		{
			appObj::border_info_t::lock lock{me->border_info};

			borders.reserve(lock->ids.size());

			for (const auto &c:lock->ids)
				borders.push_back(c);
		});

		return borders;
	}
};

static const border_handler border_handler_inst;

// <element> exists, and is empty.
//
// Checkbox

typedef unimplemented_handler single_value_exists_handler;

static const single_value_exists_handler single_value_exists_handler_inst;

// <element> contains an unsigned value
//
// Input field

struct to_size_t_handler : setting_handler {

	setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
					  const create_ui_info &info)
		const override
	{
		x::w::input_field_config config{10};

		config.alignment=x::w::halign::right;

		auto field=f->create_input_field(info.value, config);

		// Create a size_t validator.

		auto validated_input=field->set_string_validator
			([]
			 (ONLY IN_THREAD,
			  const std::string &value,
			  size_t *parsed_value,
			  const auto &field,
			  const auto &trigger) -> std::optional<size_t>
			 {
				 if (parsed_value)
					 return *parsed_value;

				 return std::nullopt;
			 },
			 []
			 (size_t n)
			 {
				 return std::to_string(n);
			 });


		return 	[field, validated_input]
			() -> std::optional<std::u32string>
			{
				if (validated_input->validated_value.get())
					return field->get_unicode();
				return std::nullopt;
			};
	}
};

static const to_size_t_handler to_size_t_handler_inst;

// <element> contains an floating point value
//
// Input field

typedef unimplemented_handler to_mm_handler;

static const to_mm_handler to_mm_handler_inst;

// <element> contains an integer value 0-100.
//
// Input field

struct to_percentage_t_handler : setting_handler {

	setting_create_ui_ret_t create_ui(const x::w::gridfactory &f,
					  const create_ui_info &info)
		const override
	{
		x::w::input_field_config config{4};

		config.alignment=x::w::halign::right;

		auto field=f->create_input_field(info.value, config);

		// Accept only digits.

		field->on_filter([]
				 (ONLY IN_THREAD, const auto &info)
		{
			for (const auto &c:info.new_contents)
				if (c < '0' || c > '9')
					return;
			info.update();
		});

		// Create a size_t validator.

		// The value for this validator is std::optional<size_t>,
		// and the value is a nullopt if the entered value is
		// an empty string.

		auto validated_input=field->set_validator
			([]
			 (ONLY IN_THREAD,
			  const std::string &value,
			  const auto &field,
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
					 field->stop_message
						 (_("Enter a percentage value"
						    " between 0-100."));
					 return std::nullopt;
				 }
				 return std::optional<size_t>{v};
			 },
			 []
			 (const std::optional<size_t> &n)
			 {
				 if (!n)
					 return std::string{};

				 return std::to_string(*n);
			 });


		return 	[field, validated_input]
			() -> std::optional<std::u32string>
			{
				auto vv=validated_input->validated_value.get();

				if (!vv) return std::nullopt; // Bad input.

				auto &v=*vv;

				if (!v) return U"";

				std::string s=std::to_string(*v);

				return std::u32string{ s.begin(), s.end() };
			};
	}
};


static const to_percentage_t_handler to_percentage_t_handler_inst;

// <element> contains a list_selection_type_cb_t
//
// Combo-box

typedef unimplemented_handler to_selection_type_handler;

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

typedef unimplemented_handler to_scrollbar_visibility_handler;

static const to_scrollbar_visibility_handler to_scrollbar_visibility_handler_inst;

// <element> specifies a shortcut
//
// Input field.

typedef unimplemented_handler shortcut_handler;

static const shortcut_handler shortcut_handler_inst;

// <element> specifies a bidi value
//
// Combo box.

struct to_bidi_direction_handler : standard_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		// TODO
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
		// TODO
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

	void saved_element(const x::xml::writelock &lock,
			   const std::string &handler_name,
			   const std::u32string &value,
			   appgenerator_save &info) const override
	{
		lock->attribute({"type", "theme_text"});
	}

};

static const text_param_value_handler text_param_value_handler_inst;

// <element> specifies a font name
//
// Combo box.

typedef unimplemented_handler font_value_handler;

static const font_value_handler font_value_handler_inst;

// <element> specifies a rgb name
//
// Combo box.

typedef unimplemented_handler rgb_value_handler;

static const rgb_value_handler rgb_value_handler_inst;

// <element> specifies method call.
//
// Checkbox.

typedef unimplemented_handler method_call_handler;

static const method_call_handler method_call_handler_inst;

// <element> specifies a grid factory generator
//
// Combo box.

struct lookup_factory_handler : editable_combobox_handler {

	std::vector<x::w::list_item_param> combobox_values(const create_ui_info
							   &info) const override
	{
		std::vector<x::w::list_item_param> factories;

		appinvoke([&]
			  (appObj *me)
		{
			auto all_factories=me->current_generators
				->all_generators_for("factory",
						     info.handler_name);

			factories.insert(factories.end(),
					 all_factories.begin(),
					 all_factories.end());
		});

		return factories;
	}

	// Add an extra_save action to autocreate this factory.

	void saved_element(const x::xml::writelock &lock,
			   const std::string &handler_name,
			   const std::u32string &value,
			   appgenerator_save &info) const override;
};

void lookup_factory_handler::saved_element(const x::xml::writelock &lock,
					   const std::string &handler_name,
					   const std::u32string &value,
					   appgenerator_save &info) const
{
	auto value_string=x::locale::base::global()->fromu32(value);

	info.extra_saves.emplace_back
		([handler_name, value_string]
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

typedef unimplemented_handler factory_parseconfig_handler;

static const factory_parseconfig_handler factory_parseconfig_handler_inst;

// <elements> contains 0 or more listlayout elements.

// Dialog

typedef unimplemented_handler listlayout_parseconfig_handler;

static const listlayout_parseconfig_handler listlayout_parseconfig_handler_inst;

// <elements> contains initializations of list_item_params.

// Dialog

typedef unimplemented_handler list_items_params_value_handler;

static const list_items_params_value_handler
list_items_params_value_handler_inst;

// <elements> occurs multiple times, each occurence contains a string.

// Custom dialog.

typedef unimplemented_handler multiple_values_handler;

static const multiple_values_handler multiple_values_handler_inst;

///////////////////////////////////////////////////////
//
// Lookup table for <type>s of <member>s.

typedef decltype([](const char *p) noexcept
 {
	 size_t n=0;

	 while (*p)
	 {
		 n = (n << 1) ^ (unsigned char)*p;
		 p++;
	 }
	 return n;
 }) c_string_hash;

typedef decltype([]
		 (const char *a, const char *b) noexcept
 {
	 return strcmp(a, b) == 0;
 }) c_string_equals;

static const std::unordered_map<const char *,
				const setting_handler *,
				c_string_hash,
				c_string_equals> member_types{
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
	{ "method_call", &method_call_handler_inst },
};

//! Extract additional information from a <lookup>.

struct lookup_info {

	//! The lookup function
	std::string function;

	//! An extra single_value parameter
	std::string extra_single_value;

	//! Constructor
	lookup_info(const x::xml::const_readlock &lookup);
};

lookup_info::lookup_info(const x::xml::const_readlock &lookup)
{
	auto root=lookup->clone();

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto name=root->name();

		if (name == "function")
		{
			function=root->get_text();
			continue;
		}
		if (name == "default_params")
			continue;
		if (name == "modify")
			continue;
		if (name == "prepend-parameter")
			continue;

		if (name != "parameter")
			throw EXCEPTION("Unknown node in <lookup>: <"
					<< name << ">");

		auto parameter=root->get_text();

		if (parameter == "lock")
			continue;

		if (parameter.substr(0, 20) == "single_value(lock, \"")
		{
			parameter=parameter.substr(20);

			extra_single_value=std::string{parameter.begin(),
				std::find(parameter.begin(),
					  parameter.end(),
					  '"')};
			continue;
		}

		throw EXCEPTION("Unknown <parameter> in <lookup>: "
				<< parameter);
	}
}


////////////////////////////////////////////////////////////////////////////
//
// Parse a <member> specification.

void parse_parameter::define_member(const x::xml::readlock &root,
				    std::vector<parse_parameter> &extra_members)
{
	std::string member_type_name;

	std::optional<lookup_info> lookup;

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto member_prop=root->clone();

		auto name=member_prop->name();

		if (name == "name")
		{
			parameter_name=member_prop->get_text();
			continue;
		}
		if (name == "type")
		{
			member_type_name=member_prop->get_text();
			continue;
		}

		if (name == "field")
			continue;

		if (name == "lookup")
		{
			lookup.emplace(root);
			continue;
		}
		if (name == "method_call")
		{
			member_type_name="method_call";
			continue;
		}
		throw EXCEPTION("Unknown <member> node: " << name);
	}

	// We know what to do with a <lookup> only for a single_value.
	if (lookup)
	{
		if (member_type_name != "single_value")
		{
			throw EXCEPTION("Internal error: lookup is for"
					" something other than a single_value");
		}
	}

	if (member_type_name == "single_value")
	{
		if (lookup)
		{
			// Replace the type with the lookup function.

			if (lookup->function
			    == "compiler.generators->lookup_color" ||
			    lookup->function
			    == "compiler.generators->lookup_dim")
			{
				member_type_name=lookup->function;

				if (!lookup->extra_single_value.empty())
				{
					extra_members.emplace_back
						(lookup->extra_single_value);
				}
			}
			// TODO: get_appearance_base
			// TODO: lookup_appearance<const_type_appearance>
		}
	}

	auto iter=member_types.find(member_type_name.c_str());

	if (iter == member_types.end())
	{
		throw EXCEPTION("Unknown <member> <type> "
				<< member_type_name
				<< " (of " << parameter_name << ")");
	}

	handler=iter->second;
}

////////////////////////////////////////////////////////////////////////////
//
// Parse a <parameter> specification.

static const std::unordered_map<const char *,
				const setting_handler *,
				c_string_hash,
				c_string_equals> parameter_types{

	{ "optional_constant<true>", &checkbox_handler_inst },
	{ "optional_constant<false>", &checkbox_handler_inst },
	{ "single_value", &single_value_handler_inst },

	// Aliases for single value.
	{ "compiler.lookup_scrollbar_type", &single_value_handler_inst },
	{ "compiler.lookup_container_generators",
	  &layoutmanager_type_handler_inst },
	{ "restore_panelayoutmanager_position", &single_value_handler_inst },
	{ "restore_tablelayoutmanager_position", &single_value_handler_inst },

	{ "compiler.generators->lookup_color", &color_handler_inst },
	{ "compiler.generators->lookup_dim", &dim_handler_inst },
	{ "compiler.generators->lookup_border", &border_handler_inst },

	{ "single_value_exists", &single_value_exists_handler_inst },
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
};

parse_parameter::parse_parameter()=default;

void parse_parameter::define_parameter(const x::xml::readlock &root,
				       std::vector<parse_parameter>
				       &extra_parameters)
{
	std::string parameter_type;
	std::string factory_wrapper;
	std::optional<lookup_info> lookup;

	object_members.reserve(root->get_xpath("member")->count()+
			       root->get_xpath("member/lookup/parameter")
			       ->count());

	std::unordered_set<std::string> all_parsed_members;

	std::vector<parse_parameter> extra_members;

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto parameter_prop=root->clone();

		auto name=parameter_prop->name();

		if (name == "name")
		{
			parameter_name=parameter_prop->get_text();
			continue;
		}
		if (name == "type")
		{
			parameter_type=parameter_prop->get_text();
			continue;
		}

		if (name == "lookup")
		{
			lookup.emplace(parameter_prop);
			continue;
		}

		if (name == "xpath")
		{
			auto xpath=parameter_prop->get_text();
			if (xpath != ".")
			{
				std::cerr << "Unknown <xpath> "
					"value: "
					  << xpath
					  << std::endl;
				continue;
			}
			same_xpath=true;
			continue;
		}

		if (name == "factory_wrapper")
		{
			factory_wrapper=
				parameter_prop->get_text();
			continue;
		}

		if (name == "scalar")
		{
			scalar_parameter=true;
			break;
		}

		if (name == "object")
		{
			is_object_parameter=true;
			continue;
		}

		if (name == "member_name" ||
		    name == "before-passing-parameter" ||
		    name == "after-passing-parameter" ||
		    name == "initialize_self" ||
		    name == "default_constructor_params" ||
		    name == "method_call")
			continue;

		if (name == "member")
		{
			parse_parameter new_member;

			new_member.define_member(parameter_prop, extra_members);

			if (!all_parsed_members
			    .insert(new_member.parameter_name).second)
				throw EXCEPTION("Duplicate member: "
						<< new_member.parameter_name);

			object_members.push_back(std::move(new_member));

			continue;
		}
		throw EXCEPTION("Unknown <parameter> node: "
				<< name);
	}

	// Review the extra members we collected

	// Nothing more needs to be done if a regular member with that name
	// was specified, this directive will piggy-back on top of it.
	//
	// Otherwise we add it as an additional member.

	for (auto &m:extra_members)
	{
		if (all_parsed_members.find(m.parameter_name) !=
		    all_parsed_members.end())
		{
			continue;
		}

		object_members.push_back(m);
	}

	object_members.shrink_to_fit();

	if (scalar_parameter)
	{
		if (is_object_parameter ||
		    !object_members.empty())
			throw EXCEPTION("<scalar> together with "
					"other attributes");
		return;
	}

	if (is_object_parameter)
	{
		return;
	}

	if (!object_members.empty())
		throw EXCEPTION("<member> specified without an "
				"<object>");

	// We know what to do with a <lookup> only for a single_value.

	if (lookup)
	{
		if (parameter_type != "single_value")
		{
			throw EXCEPTION("Internal error: lookup is for"
					" something other than a single_value");
		}
	}

	if (parameter_type == "single_value")
	{
		if (lookup)
		{
			// Replace the type with the lookup function.

			if (lookup->function.substr(0, 27)
			    != "compiler.lookup_appearance<") // TODO
			{
				parameter_type=lookup->function;

				if (!lookup->extra_single_value.empty())
				{
					extra_parameters.emplace_back
						(lookup->extra_single_value);
				}
			}
		}
	}

	handler_name=parameter_type;

	if (parameter_type.substr(0, 16) == "compiler.lookup_" &&
	    parameter_type.size() >= 34 &&
	    parameter_type.substr(parameter_type.size()-18) ==
	    "factory_generators")
	{
		parameter_type="lookup_factory";
		handler_name=handler_name.substr(16, handler_name.size()-34);

		if (handler_name.empty())
			handler_name="factory";
	}

	auto iter=parameter_types.find(parameter_type.c_str());

	if (iter == parameter_types.end())
	{
		throw EXCEPTION("Unknown <parameter> <type> "
				<< parameter_type
				<< " (of " << parameter_name << ")");
	}

	if (parameter_name.empty())
		throw EXCEPTION("Parameter <name> not specified");

	handler=iter->second;
}

parse_parameter::parse_parameter(const std::string &extra_single_value)
	: parameter_name{extra_single_value},
	  handler{&single_value_handler_inst}
{
}

void parse_parameter::save(const x::xml::writelock &lock,
			   const std::u32string &value,
			   appgenerator_save &info) const
{
	if (same_xpath)
	{
		handler->saved_element(lock, handler_name, value, info);
		lock->create_child()->text(value)->parent();
	}
	else
	{
		lock->create_child()->element({parameter_name})->text(value)
			->parent();
		handler->saved_element(lock, handler_name, value, info);
		lock->get_parent();
	}

	if (is_object_parameter)
	{
		throw EXCEPTION("Unimplemented: object");
	}

	if (scalar_parameter)
	{
		throw EXCEPTION("Unimplemented: scalar");
	}

	if (object_members.size())
	{
		throw EXCEPTION("Unimplemented: members");
	}
}

////////////////////////////////////////////////////////////////////////////

LOG_CLASS_INIT(parse_function);

parse_function::parse_function(const x::xml::readlock &root)
{
	parsed_parameters.reserve(root->get_xpath("parameter")
				  ->count() +
				  root->get_xpath("parameter/lookup/parameter")
				  ->count());

	std::vector<parse_parameter> extra_parameters;

	std::unordered_set<std::string> all_parsed_parameters;

	size_t same_xpath=0;

	std::string condition_name, condition_value, condition_exists;

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

		parse_parameter parsed_parameter;

		parsed_parameter.define_parameter(parameter, extra_parameters);

		if (parsed_parameter.scalar_parameter)
			continue;

		if (!all_parsed_parameters.insert(parsed_parameter
						  .parameter_name)
		    .second)
		{
			throw EXCEPTION("Duplicate parameter: "
					<< parsed_parameter.parameter_name);
		}

		if (parsed_parameter.same_xpath)
			++same_xpath;
		parsed_parameters
			.push_back(std::move(parsed_parameter));
	}

	// Review the extra parameters we collected

	// Nothing more needs to be done if a regular parameter with that name
	// was specified, this directive will piggy-back on top of it.
	//
	// Otherwise we add it as an additional parameter.

	for (auto &p: extra_parameters)
	{
		if (all_parsed_parameters.find(p.parameter_name) !=
		    all_parsed_parameters.end())
		{
			continue;
		}

		parsed_parameters.push_back(p);
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

void parse_function::save(const x::xml::writelock &lock,
			  const std::vector<std::u32string> &parameter_values,
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
				const auto &[name, value]=name_and_value;

				n->element({name});
				n->text(value);

				lock->get_parent();
				lock->get_parent();
			},
			[&](const std::tuple<std::string, bool>
			    &condition_exists)
			{
				const auto &[name, value]=condition_exists;

				if (value)
					return; // Will generate this parameter

				throw EXCEPTION("TODO: not tested yet");
				n->element({name});
				lock->get_parent();
			}}, condition);

	if (new_element_or_container)
	{
		std::cout << "NEW ELEMENT OR CONTAINER" << std::endl;
	}

	if (parsed_parameters.size() != parameter_values.size())
	{
		throw EXCEPTION("Internal error, wrong parameter value count");
	}

	auto v=parameter_values.begin();

	for (const auto &p:parsed_parameters)
	{
		p.save(lock, *v++, info);
	}
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

struct plain_appgenerator_functionObj : public appgenerator_functionObj {


	const parse_function *function;

	x::vector<std::u32string> parameter_values;

	plain_appgenerator_functionObj(const parse_function *function,
				       std::vector<std::u32string>
				       &&parameter_values)
		: function{function},
		  parameter_values{x::vector<std::u32string>
		::create(std::move(parameter_values))}
	{
	}

	~plain_appgenerator_functionObj()=default;

	appgenerator_function clone() const override
	{
		return x::ref<plain_appgenerator_functionObj>::create
			(function, std::vector<std::u32string>{
				*parameter_values
			});
	}

	x::w::text_param description(description_format fmt) const override
	{
		return function->description(fmt);
	}

	generator_create_ui_ret_t create_ui(const x::w::gridlayoutmanager &glm)
		const override
	{
		if (parameter_values->size()
		    != function->parsed_parameters.size())
		{
			throw EXCEPTION("Internal error: generator function"
					" parameter count mismatch");
		}

		auto get_values = x::vector<setting_create_ui_ret_t>::create();

		get_values->reserve(parameter_values->size());

		size_t i=0;

		for (const auto &p:function->parsed_parameters)
		{
			auto f=glm->append_row();

			f->halign(x::w::halign::right);
			f->valign(x::w::valign::middle);

			auto s=p.parameter_name + ":";

			s[0] = x::chrcasecmp::toupper(s[0]);

			f->create_label(s);

			f->valign(x::w::valign::middle);
			get_values->push_back(p.handler->create_ui(f, {
						parameter_values->at(i),
						p.handler_name,
					}));

			++i;
		}
		return [get_values, parameter_values=this->parameter_values]
		{
			std::vector<std::u32string> new_values;

			new_values.reserve(parameter_values->size());

			for (auto &get_field:*get_values)
			{
				auto new_value=get_field();

				if (!new_value)
					return false;
				new_values.push_back(*new_value);
			}

			(*parameter_values)=std::move(new_values);

			return true;
		};
	}

	void save(const x::xml::writelock &lock,
		  appgenerator_save &info) const override
	{
		function->save(lock, *parameter_values, info);
	}
};

#if 0
{
#endif
};

bool parse_function::parse_generator(const x::xml::readlock &root,
				     std::vector<const_appgenerator_function>
				     &funcs)
	const
{
	bool condition_found=false;

	// If there are no conditions that was easy: condition_found is
	// true. Otherwise we'll check them below.
	std::visit(x::visitor{
			[&](const no_condition &)
			{
				condition_found=true;
			},[&](const condition_name_and_value &name_value)
			{
				LOG_DEBUG("Checking " << name
					  << ", if condition " << name << "=");

			},[&](const condition_exists exists_node)
			{
				LOG_DEBUG("Checking " << name
					  << ", if condition " << name
					  << "exists");
			}
		}, condition);

	// Allow a value for each parameter in parameter_values

	std::vector<std::u32string> parameter_values;

	//! Create a lookup from each parameter, by its name, to its
	//! parser, and its value in parameter_values.
	std::unordered_map<std::string,
			   std::tuple<const parse_parameter *,
				      std::u32string *>>
		parameter_lookup;

	parameter_values.resize(parsed_parameters.size());
	size_t i=0;

	for (auto &p:parsed_parameters)
		parameter_lookup.emplace(p.parameter_name,
					 std::tuple{&p, &parameter_values[i++]}
					 );

	// If there's only one parameter and it's the same_xpath, this
	// parameter's value is obtained directly.
	if (parameter_lookup.size() == 1)
	{
		// Can only be one parameter to this function, like this.

		auto &[parameter, value] = parameter_lookup.begin()->second;

		if (parameter->same_xpath)
		{
			parameter_values[0]=root->get_u32text();

			// No more parameter, if this loop below finds even
			// one element it'll fail.

			parameter_lookup.clear();

			LOG_TRACE("Using value in the same xpath");
		}
	}

	// Examine the XML elements in the <function>, look them up in the
	// parameter_lookup table, save their value, then remove this parameter
	// from parameter_lookup.

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto name=root->name();

		LOG_TRACE("Node: <" << name << ">");

		// If this function has a specific condition, check to see if
		// we just found it.

		if (std::visit(x::visitor{
					[&](const no_condition &)
					{
						return false;
					},[&](const std::tuple<std::string,
					      std::string>
					      &name_value)
					{
						// Condition specified an
						// element with the given
						// name has the given value.
						//
						// Check if this is so.

						auto &[n, v]=
							name_value;

						if (!(name == n &&
						      root->get_text() ==
						      v))
							return false;

						condition_found=true;
						LOG_TRACE("Found condition "
							  << n << "=" << v);
						return true;

					},[&](const std::tuple<std::string,
					      bool> exists_node)
					{
						auto &[n, has_value]=
							exists_node;

						if (name != n)
							return false;

						condition_found=true;

						// If has_value, we need
						// to parse it, otherwise
						// we skip this element.
						LOG_TRACE("Found condition "
							  << n
							  << " exists ");
						return !has_value;
					}},
				condition))
			continue;

		auto parameter_iter=parameter_lookup.find(name);

		if (parameter_iter == parameter_lookup.end())
		{
			LOG_DEBUG("Node <" << name << "> not found");
			// Maybe some other function, the condition_found
			// is expected to be false, anyway.
			return false;
		}

		auto &[parameter, value] = parameter_iter->second;

		*value=root->get_u32text();
		parameter_lookup.erase(parameter_iter);
		LOG_TRACE("Found parameter " << name);
	}

	if (!condition_found)
	{
		LOG_DEBUG("Required condition not found");
		return false;
	}

	// We expect to have parsed all parameters.
	if (!parameter_lookup.empty())
	{
		LOG_DEBUG("Some parameters were missing (such as "
			  << parameter_lookup.begin()->first
			  << ")");
		return false;
	}
	LOG_DEBUG("Found");

	funcs.push_back(create(std::move(parameter_values)));
	return true;
}

appgenerator_function parse_function::create() const
{
	return create(std::vector<std::u32string>{parsed_parameters.size()});
}

appgenerator_function parse_function::create(std::vector<std::u32string> &&args)
	const
{
	return x::ref<plain_appgenerator_functionObj>::create
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
				if (b->second.parse_generator(root->clone(),
							      parsed))
					break;	// Parsed succesfully

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
	case appuigenerator_type::factory:
		return "factory";
	case appuigenerator_type::layoutmanager:
		return "layout";
	default:
		throw EXCEPTION("xml_node_name() is defined only for "
				"factory and layoutmanager");
	}
};
