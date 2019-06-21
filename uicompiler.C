/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "defaulttheme.H"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "x/w/label.H"
#include "x/w/focusable_label.H"
#include "x/w/canvas.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/synchronized_axis.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/shortcut.H"
#include "x/w/border_arg.H"
#include "x/w/text_param.H"
#include "x/w/all_appearances.H"
#include "x/w/scrollbar.H"
#include "x/w/theme_text.H"
#include "x/w/tooltip.H"
#include "x/w/button.H"
#include "x/w/input_field.H"
#include "x/w/input_field_config.H"
#include "x/w/date_input_field.H"
#include "x/w/date_input_field_config.H"
#include "x/w/image.H"
#include "x/w/image_button.H"
#include "x/w/radio_group.H"
#include "x/w/progressbar.H"
#include "x/w/color_picker.H"
#include "x/w/color_picker_config.H"
#include "x/w/font_picker.H"
#include "x/w/font_picker_config.H"
#include "x/w/scrollbar.H"
#include "theme_parser_lock.H"
#include "messages.H"
#include <x/functionalrefptr.H>
#include <x/visitor.H>
#include <algorithm>
#include <functional>
#include <type_traits>

LIBCXXW_NAMESPACE_START


const char * const rgb_channels[]={"r",
				   "g",
				   "b",
				   "a"};

rgb_component_t rgb::* const rgb_fields[]=
	{
	 &rgb::r,
	 &rgb::g,
	 &rgb::b,
	 &rgb::a};

void uicompiler::wrong_appearance_type(const std::string_view &name,
				       const char *element)
{
	throw EXCEPTION(gettextmsg(_("The appearance object \"%1\" (specified"
				     " for the \"%2%\" parameter)"
				     " is not the expected"
				     " appearance object type"),
				   name, element));
}

/////////////////////////////////////////////////////////////////////////////
//
// Supported layout manager functionality.

// Each supported layout manager defines a 'generators' member which holds
// a vector of compiled layout manager generator, and implements additional
// functionality:
//
// - create_container(): takes a factory object, and an elements object;
// creates a container and generates its contents.
//
// - generate - takes a container, gets its layout manager, and generates
// its contents.
//
// - new_layoutmanager - returns the corresponding new layoutmanager object.

// Grid layout manager functionality.

struct uicompiler::gridlayoutmanager_functions {

	// A vector of compiled grid layout manager generators

	struct generators {

		const_vector<gridlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const theme_parser_lock &lock,
			   const std::string &name)
			: generator_vector{compiler
					   .lookup_gridlayoutmanager_generators
					   (lock, name)}
		{
		}

		container create_container(const factory &f,
					   uielements &factories) const
		{
			return f->create_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 new_gridlayoutmanager{});
		}

		inline new_gridlayoutmanager new_layoutmanager() const
		{
			return {};
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			gridlayoutmanager glm=c->get_layoutmanager();

			for (const auto &g:*generator_vector)
			{
				g(glm, factories);
			}
		}
	};
};

// Parse generators for the contents of a new_listlayoutmanager

static const_vector<new_listlayoutmanager_generator>
create_newlistlayoutmanager_vector(uicompiler &compiler,
				   const theme_parser_lock &orig_lock)
{
	auto lock=orig_lock->clone();

	auto xpath=lock->get_xpath("config");

	if (xpath->count() == 0) // None, return an empty vector.
		return const_vector<new_listlayoutmanager_generator>::create();

	xpath->to_node();

	return compiler.new_listlayout_parseconfig(lock);
}

struct uicompiler::listlayoutmanager_functions {

	// A vector of compiled grid layout manager generators

	struct generators {

		const listlayoutstyle_impl &style;

		// Generators for the contents of the new_listlayoutmanager

		const_vector<new_listlayoutmanager_generator
			     > new_listlayoutmanager_vector;

		// Generators for the contents of the list layout manager.
		const_vector<listlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const theme_parser_lock &lock,
			   const std::string &name)
			: style{single_value_exists(lock, "style")
				? list_style_by_name(lowercase_single_value
						     (lock, "style",
						      "container"))
				: highlighted_list},
			  new_listlayoutmanager_vector
			{
			 create_newlistlayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_listlayoutmanager_generators
					   (lock, name)}
		{
		}

		focusable_container create_container(const factory &f,
						     uielements &factories)
			const
		{
		        auto nlm=new_layoutmanager();

			// Generate the contents of the new_listlayoutmanager.

			for (const auto &g:*new_listlayoutmanager_vector)
			{
				g(&nlm, factories);
			}

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nlm);
		}

		inline new_listlayoutmanager new_layoutmanager() const
		{
			new_listlayoutmanager nlm{style};

			return nlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			listlayoutmanager llm=c->get_layoutmanager();

			for (const auto &g:*generator_vector)
			{
				g(llm, factories);
			}
		}
	};
};

// Parse generators for the contents of a new_standard_comboboxlayoutmanager

static const_vector<new_standard_comboboxlayoutmanager_generator>
create_newstandard_comboboxlayoutmanager_vector(uicompiler &compiler,
						const theme_parser_lock
						&orig_lock)
{
	auto lock=orig_lock->clone();

	auto xpath=lock->get_xpath("config");

	if (xpath->count() == 0) // None, return an empty vector.
		return const_vector<new_standard_comboboxlayoutmanager_generator
				    >::create();

	xpath->to_node();

	return compiler.new_standard_comboboxlayout_parseconfig(lock);
}

struct uicompiler::standard_comboboxlayoutmanager_functions {

	// A vector of compiled grid layout manager generators

	struct generators {

		// Generators for the contents of the new_standard_comboboxlayoutmanager

		const_vector<new_standard_comboboxlayoutmanager_generator
			     > new_standard_comboboxlayoutmanager_vector;

		// Generators for the contents of the standard_combobox layout manager.
		const_vector<standard_comboboxlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const theme_parser_lock &lock,
			   const std::string &name)
			: new_standard_comboboxlayoutmanager_vector
			{
			 create_newstandard_comboboxlayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_standard_comboboxlayoutmanager_generators
					   (lock, name)}
		{
		}

		focusable_container create_container(const factory &f,
						     uielements &factories)
			const
		{
		        auto nlm=new_layoutmanager();

			// Generate the contents of the new_standard_comboboxlayoutmanager.

			for (const auto &g:*new_standard_comboboxlayoutmanager_vector)
			{
				g(&nlm, factories);
			}

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nlm);
		}

		inline new_standard_comboboxlayoutmanager new_layoutmanager()
			const
		{
			new_standard_comboboxlayoutmanager nlm;

			return nlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			standard_comboboxlayoutmanager llm=
				c->get_layoutmanager();

			for (const auto &g:*generator_vector)
			{
				g(llm, factories);
			}
		}
	};
};

// Parse generators for the contents of a new_editable_comboboxlayoutmanager

static const_vector<new_editable_comboboxlayoutmanager_generator>
create_neweditable_comboboxlayoutmanager_vector(uicompiler &compiler,
						const theme_parser_lock
						&orig_lock)
{
	auto lock=orig_lock->clone();

	auto xpath=lock->get_xpath("config");

	if (xpath->count() == 0) // None, return an empty vector.
		return const_vector<new_editable_comboboxlayoutmanager_generator
				    >::create();

	xpath->to_node();

	return compiler.new_editable_comboboxlayout_parseconfig(lock);
}

struct uicompiler::editable_comboboxlayoutmanager_functions {

	// A vector of compiled grid layout manager generators

	struct generators {

		// Generators for the contents of the new_editable_comboboxlayoutmanager

		const_vector<new_editable_comboboxlayoutmanager_generator
			     > new_editable_comboboxlayoutmanager_vector;

		// Generators for the contents of the editable_combobox layout manager.
		const_vector<editable_comboboxlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const theme_parser_lock &lock,
			   const std::string &name)
			: new_editable_comboboxlayoutmanager_vector
			{
			 create_neweditable_comboboxlayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_editable_comboboxlayoutmanager_generators
					   (lock, name)}
		{
		}

		focusable_container create_container(const factory &f,
						     uielements &factories)
			const
		{
		        auto nlm=new_layoutmanager();

			// Generate the contents of the new_editable_comboboxlayoutmanager.

			for (const auto &g:*new_editable_comboboxlayoutmanager_vector)
			{
				g(&nlm, factories);
			}

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nlm);
		}

		inline new_editable_comboboxlayoutmanager new_layoutmanager()
			const
		{
			new_editable_comboboxlayoutmanager nlm;

			return nlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			editable_comboboxlayoutmanager llm=
				c->get_layoutmanager();

			for (const auto &g:*generator_vector)
			{
				g(llm, factories);
			}
		}
	};
};

// Book layout manager functionality
// Parse generators for the contents of a new_listlayoutmanager

static const_vector<new_booklayoutmanager_generator>
create_newbooklayoutmanager_vector(uicompiler &compiler,
				   const theme_parser_lock &orig_lock)
{
	auto lock=orig_lock->clone();

	auto xpath=lock->get_xpath("config");

	if (xpath->count() == 0) // None, return an empty vector.
		return const_vector<new_booklayoutmanager_generator>::create();

	xpath->to_node();

	return compiler.new_booklayout_parseconfig(lock);
}

struct uicompiler::booklayoutmanager_functions {

	// A vector of compiler book layout manager generators

	struct generators {

		// Generators for the contents of the new_booklayoutmanager

		const_vector<new_booklayoutmanager_generator
			     > new_booklayoutmanager_vector;

		const_vector<booklayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const theme_parser_lock &lock,
			   const std::string &name)
			: new_booklayoutmanager_vector
			{
			 create_newbooklayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_booklayoutmanager_generators
					   (lock, name)}
		{
		}

		focusable_container create_container(const factory &f,
						     uielements &factories)
			const
		{
		        auto nblm=new_layoutmanager();

			// Generate the contents of the new_listlayoutmanager.

			for (const auto &g:*new_booklayoutmanager_vector)
			{
				g(&nblm, factories);
			}

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nblm);
		}

		inline new_booklayoutmanager new_layoutmanager() const
		{
			return {};
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			booklayoutmanager blm=c->get_layoutmanager();

			for (const auto &g:*generator_vector)
			{
				g(blm, factories);
			}
		}
	};
};

//
// "container_generators_t" is a variant of all these "generators' classes.
//
// Define the container_generators_t variant directly based on the
// layoutmanager_functions variant.

template<typename variant_t> struct all_generators;

template<typename ...Args> struct all_generators<std::variant<Args...>> {

	typedef std::variant<typename Args::generators...> type;
};

struct uicompiler::container_generators_t
	: all_generators<layoutmanager_functions>::type {

	typedef all_generators<layoutmanager_functions>::type variant_t;

	using variant_t::variant_t;
};

// Used by compiler progressbar cod

// Used by compiler progressbar code

element uicompiler::create_progressbar(const factory &generic_factory,
				       const container_generators_t
				       &generators_arg,
				       uielements &elements,
				       const progressbar_config &config)
{
	const container_generators_t::variant_t &generators=generators_arg;

	return std::visit
		([&]
		 (const auto &generators) -> progressbar
		 {
			 if constexpr (std::is_base_of_v<new_layoutmanager,
				       std::remove_reference_t<
				       decltype(generators.new_layoutmanager())
				       >>)
			 {
				const auto &nlm=generators.new_layoutmanager();

				return generic_factory->create_progressbar
					([&]
					 (const auto &c)
					 {
						 generators.generate(c,
								     elements);
					 },
					 config,
					 nlm);
			 }
			 else
			 {
				 throw EXCEPTION(_("Invalid layout manager"
						   " for progress bars"));
			 }
		 },
		 generators);
}

uicompiler::layoutmanager_functions
uicompiler::get_layoutmanager(const std::string &type)
{
	if (type == "grid")
		return layoutmanager_functions{
			std::in_place_type_t<gridlayoutmanager_functions>{}
		};

	if (type == "book")
		return layoutmanager_functions{
			std::in_place_type_t<booklayoutmanager_functions>{}
		};

	if (type == "list")
		return layoutmanager_functions{
			std::in_place_type_t<listlayoutmanager_functions>{}
		};

	if (type == "standard_combobox")
		return layoutmanager_functions{
			std::in_place_type_t<
				standard_comboboxlayoutmanager_functions>{}
		};

	if (type == "editable_combobox")
		return layoutmanager_functions{
			std::in_place_type_t<
				editable_comboboxlayoutmanager_functions>{}
		};

	throw EXCEPTION(gettextmsg(_("\"%1%\" is not a known layout/container"),
				   type));
}

namespace {
#if 0
}
#endif

static std::string get_id_to_restore(const theme_parser_lock &lock)
{
	auto id=lock->get_any_attribute("id");

	if (id.empty())
		throw EXCEPTION(_("<restore> requires <element> to specify an"
				  " id attribute"));

	return id;
}

static const_screen_positions positions_to_restore(uicompiler &compiler)
{
	if (!compiler.saved_positions)
		throw EXCEPTION(_("<restore> requires saved screen_positions"));

	return compiler.saved_positions;
}

template<typename object_type>
inline void invoke_restore(object_type &object,
			   const theme_parser_lock &lock,
			   uicompiler &compiler)
{
	object.restore(positions_to_restore(compiler),
		       get_id_to_restore(lock));
}

#if 0
{
#endif
}

uicompiler::scrollbar_type
uicompiler::lookup_scrollbar_type(const std::string &value)
{
	if (value == "horizontal")
		return horizontal;

	if (value == "vertical")
		return vertical;

	throw EXCEPTION(_("<scrollbar> is not \"horizontal\" or \"vertical\""));
}

scrollbar uicompiler::create_scrollbar(uicompiler::scrollbar_type type,
				       const factory &f,
				       const scrollbar_config &config,
				       const const_scrollbar_appearance &a)
{
	return type==uicompiler::scrollbar_type::horizontal
		? f->create_horizontal_scrollbar(config, a)
		: f->create_vertical_scrollbar(config, a);
}

struct uicompiler::compiler_functions {

	static functionptr<void (THREAD_CALLBACK,
				 const tooltip_factory &)>
	get_optional_tooltip(uicompiler &compiler,
			     const theme_parser_lock &lock)
	{
		functionptr<void (THREAD_CALLBACK,
				  const tooltip_factory &)> optional_tooltip;

		auto id=lock->get_any_attribute("tooltip");

		if (!id.empty())
		{
			auto iter=compiler.generators
				->tooltip_generators.find(id);

			if (iter == compiler.generators
			    ->tooltip_generators.end())
				throw EXCEPTION(gettextmsg
						(_("Tooltip \"%1%\" is not"
						   " defined"),
						 id));
			optional_tooltip=iter->second;
		}

		return optional_tooltip;
	}

	//! Install the tooltip for the new element.
	static void install_tooltip(const element &e,
				    const functionptr
				    <void (THREAD_CALLBACK,
					   const tooltip_factory &)>
				    &optional_tooltip)
	{
		if (!optional_tooltip)
			return;

		e->create_custom_tooltip(optional_tooltip);
	}
};

///////////////////////////////////////////////////////////////////////////
//
// Find a generator for a layout manager or a factory.
//
// Look in the compiled generators first. If not found, it must be in
// uncompiled_elements. Find it, recursively compile it now, put it into
// generators, and return it.
//
// In this manner we end up compiling any generators that are referenced
// from other generators recursively, and because we remove the definition
// from uncompiled_elements beforehand, this detects and fails infinite
// recursion.
//
// This is called from the stylesheet-generator parser upon encountering
// an element that references a generator, by name. This ends up effectively
// translating the named reference in the stylesheet into a compiled generator.
// The referenced element then gets passed as a parameter to something like
// container_gridlayoutmanager, which ends up
// invokes something that executes the generator, when it runs, like
// container_gridlayoutmanager that creates a new container with the grid
// layout manager, then executes the generator for it.
//
// A factory reference ends up

uicompiler::container_generators_t
uicompiler::lookup_container_generators(const std::string &type,
					const theme_parser_lock &lock,
					const std::string &name,
					bool,
					const char *tag)
{
	auto functions=get_layoutmanager(type);

	return std::visit([&]
			  (auto &functions) -> container_generators_t
			  {
				  typedef typename
					  std::remove_reference_t
					  <decltype(functions)>
					  ::generators generators;

				  return container_generators_t{
					  std::in_place_type_t<generators>{},
					  *this, lock, name
				  };
			  }, functions);
}

void uicompiler::create_container(const factory &f,
				  uielements &factories,
				  const std::string &name,
				  const container_generators_t &generators)
{
	// std::visit needs a variant to work with.
	const container_generators_t::variant_t &v=generators;

	factories.new_elements.emplace
		(name,
		 std::visit([&]
			    (const auto &generators) -> container
			    {
				    return generators
					    .create_container(f, factories);
			    }, v));
}

static radio_group lookup_radio_group(uielements &elements,
				      const std::string &name)
{
	auto iter=elements.new_radio_groups.find(name);

	if (iter != elements.new_radio_groups.end())
		return iter->second;

	auto g=radio_group::create();

	elements.new_radio_groups.emplace(name, g);

	return g;
}

#include "uicompiler.inc.H/factory_parse_parameters.H"
#include "uicompiler.inc.H/factory_parser.H"

std::tuple<text_param, label_config>
uicompiler::get_label_parameters(const theme_parser_lock &lock)
{
	return get_create_label_parameters(*this, lock);
}

LIBCXXW_NAMESPACE_END
