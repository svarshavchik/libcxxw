/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "defaulttheme.H"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "x/w/element_state.H"
#include "x/w/label.H"
#include "x/w/focusable_label.H"
#include "x/w/canvas.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/itemlayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/toolboxlayoutmanager.H"
#include "x/w/synchronized_axis.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/borderlayoutmanager.H"
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
#include "x/w/progressbar.H"
#include "x/w/color_picker.H"
#include "x/w/color_picker_config.H"
#include "x/w/font_picker.H"
#include "x/w/font_picker_config.H"
#include "x/w/scrollbar.H"
#include "theme_parser_lockfwd.H"
#include "x/w/impl/uixmlparser.H"
#include "messages.H"
#include <x/functionalrefptr.H>
#include <x/visitor.H>
#include <x/xml/xpath.H>
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
// - generators must inherit from generators_base. This captures the
// new container's name.
//
// - create_container(): takes a factory object, and an elements object;
// creates a container and generates its contents.

//
// - generate - takes a container, calls get_layoutmanager(), and generates
// its contents. It calls get_new_layoutmanager() from generators_base(), which
// captures the layout manager.
//
// - new_layoutmanager - returns the corresponding new layoutmanager object.
//
// Basic outline for setting up a new layout manager:
//
// - Define "new_{name}layoutmanager_plainptr" typedef in uigeneratorsfwd.H,
// to be used as a pseudo-ref by the new_{name}layoutmanager generator.
//
// - Define "new_{name}layoutmanager_generator" typedef in uigeneratorsfwd.H
//
// - Define "{name}layoutmanager_generator" typedef in uigeneratorsfwd.H
//
// - Define the parser in uicompiler.xml for new_{name}layout, create the
// "uicompiler_new_{name}layout.C" module, adding it to the Makefile
// (including the dependency on uicompiler.stamp.inc.H).
//
// - Define the parser in uicompiler.xml for {name}layout, create the
// "uicompiler_{name}layout.C" module, adding it to the Makefile.
//
// - Add {name}layoutmanager_generators map to uigeneratorsObj
//
// uicompiler.H:
//
// - Declare new_{name}layout_parseconfig() and new_{name}layout_parser()
//
// - Declare {name}layoutmanager_functions, and add it to the
//   layoutmanager_functions variant.
//
// - Declare {name}layout_parseconfig() and {name}layout_parser()
//
// - Declare lookup_{name}layoutmanager_generators().
//
// uicompiler.C:
//
// - Define {name}layoutmanager_functions, and add it to get_layoutmanager().
//
// uicompiler3.C:
//
// - Add code to uicompiler's constructor to recognize the new <layout> type
// and parse it.
//
// - Define lookup_{name}layoutmanager_generators().
//
// {name}layoutmanager.C
//
// - Implement generate() (in uielements.C).
//
// Then define the factory (if applicable):
//
// - Define {name}factory_generator typedef in uigeneratorsfwd.H
//
// - Add {name}factory_generators map to uigeneratorsObj
//
// - Add declarations for {name}factory_parseconfig() and
//   {name}factory_parser(), and lookup_{factory}_generators() in uicompiler.H
//
// - Implement lookup_{factory}_generators() in uicompiler3.C
//
// - Define the parser in uicompiler.xml for the {factory}, create the
// "uicompiler_{factory}.C" module, adding it to the Makefile
// (including the dependency on uicompiler.stamp.inc.H).
//
// - Add code to uicompiler's constructor to recognize the new <factory> type
// and parse it.

namespace {
#if 0
}
#endif

struct generators_base {

	std::string name;

	generators_base(const ui::parser_lock &lock,
			const std::string &name)
		: name{name} {}

	layoutmanager get_new_layoutmanager(const container &new_container,
					    uielements &elements) const
	{
		auto lm=new_container->get_layoutmanager();

		lm->notmodified();

		elements.new_layoutmanagers.insert_or_assign(name, lm);

		return lm;
	}
};

#if 0
{
#endif
}

#include "uicompiler.inc.H/uicompiler_configparser.H"

// Grid layout manager functionality.

struct uicompiler::gridlayoutmanager_functions {

	// A vector of compiled grid layout manager generators

	struct generators : generators_base {

		// Generators for the contents of the new_booklayoutmanager

		const_vector<new_gridlayoutmanager_generator
			     > new_gridlayoutmanager_vector;

		const_vector<gridlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_gridlayoutmanager_vector
			{
				create_newgridlayoutmanager_vector(compiler,
								   lock)
			},
			  generator_vector{compiler
					   .lookup_gridlayoutmanager_generators
					   (lock, name)}
		{
		}

		container create_container(const factory &f,
					   const std::string &id,
					   uielements &factories) const
		{
			auto nglm=new_layoutmanager(factories);

			return f->create_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nglm);
		}

		inline new_gridlayoutmanager new_layoutmanager(
			uielements &factories) const
		{
			new_gridlayoutmanager nglm;

			// Generate the contents of the new_gridlayoutmanager.

			for (const auto &g:*new_gridlayoutmanager_vector)
			{
				g(&nglm, factories);
			}

			return nglm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			gridlayoutmanager glm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(glm, factories);
			}
		}
	};
};

// Parse generators for the contents of a new_listlayoutmanager

struct uicompiler::listlayoutmanager_functions {

	// A vector of compiled list layout manager generators

	struct generators : generators_base {

		const listlayoutstyle_impl &style;

		// Generators for the contents of the new_listlayoutmanager

		const_vector<new_listlayoutmanager_generator
			     > new_listlayoutmanager_vector;

		// Generators for the contents of the list layout manager.
		const_vector<listlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  style{single_value_exists(lock, "config/style")
				? list_style_by_name(lowercase_single_value
						     (lock, "config/style",
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
						     const std::string &id,
						     uielements &factories)
			const
		{
		        auto nlm=new_layoutmanager(factories);

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nlm);
		}

		inline new_listlayoutmanager new_layoutmanager(
			uielements &factories) const
		{
			new_listlayoutmanager nlm{style};

			// Generate the contents of the new_listlayoutmanager.

			for (const auto &g:*new_listlayoutmanager_vector)
			{
				g(&nlm, factories);
			}

			return nlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			listlayoutmanager llm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(llm, factories);
			}
		}
	};
};

////////////////////////////////////////////////////////////////////////////
//
// Table layout manager functionality.
//

// Parse all header cell generators.

static std::vector<const_vector<factory_generator>>
create_table_header_generators(uicompiler &compiler,
			       const ui::parser_lock &orig_lock)
{
	auto lock=orig_lock->clone();

	auto xpath=lock->get_xpath("header");

	size_t n=xpath->count();

	std::vector<const_vector<factory_generator>> ret;

	ret.reserve(n);

	for (size_t i=1; i<=n; ++i)
	{
		xpath->to_node(i);

		ret.push_back(compiler.factory_parseconfig(lock));
	}

	return ret;
}

struct uicompiler::tablelayoutmanager_functions {

	struct generators : generators_base {

		const listlayoutstyle_impl &style;

		// Generators for the contents of the new_tablelayoutmanager

		const_vector<new_tablelayoutmanager_generator
			     > new_tablelayoutmanager_vector;

		std::vector<const_vector<factory_generator>> header_generators;

		// Generators for the contents of the list layout manager.
		const_vector<tablelayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  style{single_value_exists(lock, "config/style")
				? list_style_by_name(lowercase_single_value
						     (lock, "config/style",
						      "container"))
				: highlighted_list},
			  new_tablelayoutmanager_vector
			{
			 create_newtablelayoutmanager_vector(compiler, lock)
			},
			  header_generators
			{
			 create_table_header_generators(compiler, lock)
			},
			  generator_vector
			{
			 compiler.lookup_tablelayoutmanager_generators(lock,
								       name)
			}
		{
		}

		focusable_container create_container(const factory &f,
						     const std::string &id,
						     uielements &factories)
			const
		{
		        auto ntlm=new_layoutmanager(factories);

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 ntlm);
		}

		inline new_tablelayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_tablelayoutmanager ntlm
				{
				 [&, this]
				 (const factory &f,
				  size_t column)
				 {
					 for (const auto &g :
						      *header_generators
						      .at(column))
					 {
						 g(f, factories);
					 }
				 },
				 style
				};

			// Generate the contents of the new_tablelayoutmanager.

			for (const auto &g:*new_tablelayoutmanager_vector)
			{
				g(&ntlm, factories);
			}

			if (ntlm.columns != header_generators.size())
				throw EXCEPTION(gettextmsg
						(_("number of <header>s (%1%) "
						   "is different from "
						   "<columns> (%2%)"),
						 header_generators.size(),
						 ntlm.columns));

			return ntlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			tablelayoutmanager llm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(llm, factories);
			}
		}
	};
};

////////////////////////////////////////////////////////////////////////////
//
// Pane layout manager functionality.
//

struct uicompiler::panelayoutmanager_functions {

	struct generators : generators_base {

		// Generators for the contents of the new_tablelayoutmanager

		const_vector<new_panelayoutmanager_generator
			     > new_panelayoutmanager_vector;

		// Generators for the contents of the pane layout manager.
		const_vector<panelayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_panelayoutmanager_vector
			{
			 create_newpanelayoutmanager_vector(compiler, lock)
			},
			  generator_vector
			{
			 compiler.lookup_panelayoutmanager_generators(lock,
								      name)
			}
		{
		}

		focusable_container create_container(const factory &f,
						     const std::string &id,
						     uielements &factories)
			const
		{
		        auto nplm=new_layoutmanager(factories);

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nplm);
		}

		inline new_panelayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_panelayoutmanager nplm{ {} };

			// Generate the contents of the new_panelayoutmanager.

			for (const auto &g:*new_panelayoutmanager_vector)
			{
				g(&nplm, factories);
			}

			return nplm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			panelayoutmanager plm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(plm, factories);
			}
		}
	};
};


////////////////////////////////////////////////////////////////////////////
//
// Item layout manager functionality.
//
// Parse new item layout manager generators from <config>

struct uicompiler::itemlayoutmanager_functions {

	struct generators : generators_base {

		// Generators for the contents of the new_tablelayoutmanager

		const_vector<new_itemlayoutmanager_generator
			     > new_itemlayoutmanager_vector;

		// Generators for the contents of the item layout manager.
		const_vector<itemlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_itemlayoutmanager_vector
			{
			 create_newitemlayoutmanager_vector(compiler, lock)
			},
			  generator_vector
			{
			 compiler.lookup_itemlayoutmanager_generators(lock,
								      name)
			}
		{
		}

		focusable_container create_container(const factory &f,
						     const std::string &id,
						     uielements &factories)
			const
		{
		        auto nilm=new_layoutmanager(factories);

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nilm);
		}

		inline new_itemlayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_itemlayoutmanager nilm;

			// Generate the contents of the new_itemlayoutmanager.

			for (const auto &g:*new_itemlayoutmanager_vector)
			{
				g(&nilm, factories);
			}

			return nilm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			itemlayoutmanager plm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(plm, factories);
			}
		}
	};
};

////////////////////////////////////////////////////////////////////////////
//
// Page layout manager functionality.
//

struct uicompiler::pagelayoutmanager_functions {

	struct generators : generators_base {

		// Generators for the contents of the new_tablelayoutmanager

		const_vector<new_pagelayoutmanager_generator
			     > new_pagelayoutmanager_vector;

		// Generators for the contents of the page layout manager.
		const_vector<pagelayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_pagelayoutmanager_vector
			{
			 create_newpagelayoutmanager_vector(compiler, lock)
			},
			  generator_vector
			{
			 compiler.lookup_pagelayoutmanager_generators(lock,
								      name)
			}
		{
		}

		container create_container(const factory &f,
					   const std::string &id,
					   uielements &factories) const
		{
		        auto nplm=new_layoutmanager(factories);

			return f->create_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nplm);
		}

		inline new_pagelayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_pagelayoutmanager nplm;

			// Generate the contents of the new_tablelayoutmanager.

			for (const auto &g:*new_pagelayoutmanager_vector)
			{
				g(&nplm, factories);
			}

			return nplm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			pagelayoutmanager plm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(plm, factories);
			}
		}
	};
};

////////////////////////////////////////////////////////////////////////////
//
// Toolbox layout manager functionality.
//

struct uicompiler::toolboxlayoutmanager_functions {

	struct generators : generators_base {

		// Generators for the contents of the new_toolboxlayoutmanager

		const_vector<new_toolboxlayoutmanager_generator
			     > new_toolboxlayoutmanager_vector;

		// Generators for the contents of the toolbox layout manager.
		const_vector<toolboxlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_toolboxlayoutmanager_vector
			{
			 create_newtoolboxlayoutmanager_vector(compiler, lock)
			},
			  generator_vector
			{
			 compiler.lookup_toolboxlayoutmanager_generators(lock,
								      name)
			}
		{
		}

		container create_container(const factory &f,
					   const std::string &id,
					   uielements &factories)
			const
		{
		        auto ntlm=new_layoutmanager(factories);

			return f->create_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 ntlm);
		}

		inline new_toolboxlayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_toolboxlayoutmanager ntlm;

			// Generate the contents of the new_tablelayoutmanager.

			for (const auto &g:*new_toolboxlayoutmanager_vector)
			{
				g(&ntlm, factories);
			}

			return ntlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			toolboxlayoutmanager plm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(plm, factories);
			}
		}
	};
};





// Parse generators for the contents of a new_standard_comboboxlayoutmanager

struct uicompiler::standard_comboboxlayoutmanager_functions {

	// A vector of compiled standard combo-box layout manager generators

	struct generators : generators_base {

		// Generators for the contents of the new_standard_comboboxlayoutmanager

		const_vector<new_standard_comboboxlayoutmanager_generator
			     > new_standard_comboboxlayoutmanager_vector;

		// Generators for the contents of the standard_combobox layout manager.
		const_vector<standard_comboboxlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_standard_comboboxlayoutmanager_vector
			{
			 create_newstandard_comboboxlayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_standard_comboboxlayoutmanager_generators
					   (lock, name)}
		{
		}

		focusable_container create_container(const factory &f,
						     const std::string &id,
						     uielements &factories)
			const
		{
		        auto nlm=new_layoutmanager(factories);

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nlm);
		}

		inline new_standard_comboboxlayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_standard_comboboxlayoutmanager nlm;

			// Generate the contents of the new_standard_comboboxlayoutmanager.

			for (const auto &g:*new_standard_comboboxlayoutmanager_vector)
			{
				g(&nlm, factories);
			}

			return nlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			standard_comboboxlayoutmanager llm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(llm, factories);
			}
		}
	};
};

// Parse generators for the contents of a new_editable_comboboxlayoutmanager

struct uicompiler::editable_comboboxlayoutmanager_functions {

	// A vector of compiled editable combo-box layout manager generators

	struct generators : generators_base {

		// Generators for the contents of the new_editable_comboboxlayoutmanager

		const_vector<new_editable_comboboxlayoutmanager_generator
			     > new_editable_comboboxlayoutmanager_vector;

		// Generators for the contents of the editable_combobox layout manager.
		const_vector<editable_comboboxlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_editable_comboboxlayoutmanager_vector
			{
			 create_neweditable_comboboxlayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_editable_comboboxlayoutmanager_generators
					   (lock, name)}
		{
		}

		focusable_container create_container(const factory &f,
						     const std::string &id,
						     uielements &factories)
			const;

		inline new_editable_comboboxlayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_editable_comboboxlayoutmanager nlm;

			// Generate the contents of the
			// new_editable_comboboxlayoutmanager.

			for (const auto &g:
				     *new_editable_comboboxlayoutmanager_vector)
			{
				g(&nlm, factories);
			}

			return nlm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			editable_comboboxlayoutmanager llm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(llm, factories);
			}
		}
	};
};

focusable_container uicompiler::editable_comboboxlayoutmanager_functions
::generators::create_container(const factory &f,
			       const std::string &id,
			       uielements &factories)
	const
{
	auto nlm=new_layoutmanager(factories);

	// Do we have a validator, if so create an editable combo-box
	// with a validated input field.

	if (!id.empty())
	{
		auto iter=factories.input_field_validators.find(id);

		if (iter != factories.input_field_validators.end())
		{
			const auto &[callback, contents] = iter->second;
			text_param initial_value;

			// Obtain the initial value of the validated input
			// field, and create it.

			auto validated=contents->to_text_param(initial_value);

			const auto &[container, field]=
				f->do_create_focusable_container(
					make_function<void (
						const focusable_container &
								 )>
					(
						[&, this]
						(const auto &container)
						{
							generate(container,
								 factories);
						}
					),
					nlm,
					initial_value,
					callback,
					validated
				);

			auto validated_value=
				contents->create_validated_input_field(field);

			if (!factories.new_validated_input_fields.emplace(
				    id,
				    validated_value
			    ).second)
			{
				throw EXCEPTION(
					gettextmsg(
						_("Input field validator "
						  "\"%1%\" already defined"),
						id)
				);
			}

			return container;
		}
	}

	return f->create_focusable_container(
		[&, this]
		(const auto &container)
		{
			generate(container, factories);
		},
		nlm
	);
}

// Book layout manager functionality

struct uicompiler::booklayoutmanager_functions {

	// A vector of compiler book layout manager generators

	struct generators : generators_base {

		// Generators for the contents of the new_booklayoutmanager

		const_vector<new_booklayoutmanager_generator
			     > new_booklayoutmanager_vector;

		const_vector<booklayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_booklayoutmanager_vector
			{
			 create_newbooklayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_booklayoutmanager_generators
					   (lock, name)}
		{
		}

		focusable_container create_container(const factory &f,
						     const std::string &id,
						     uielements &factories)
			const
		{
		        auto nblm=new_layoutmanager(factories);

			return f->create_focusable_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nblm);
		}

		inline new_booklayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_booklayoutmanager nblm;

			// Generate the contents of the new_booklayoutmanager.

			for (const auto &g:*new_booklayoutmanager_vector)
			{
				g(&nblm, factories);
			}

			return nblm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			booklayoutmanager blm=
				get_new_layoutmanager(c, factories);

			for (const auto &g:*generator_vector)
			{
				g(blm, factories);
			}
		}
	};
};

struct uicompiler::borderlayoutmanager_functions {

	// A vector of compiler border layout manager generators

	struct generators : generators_base {

		// Generators for the contents of the new_borderlayoutmanager

		const_vector<new_borderlayoutmanager_generator
			     > new_borderlayoutmanager_vector;

		const_vector<borderlayoutmanager_generator> generator_vector;

		generators(uicompiler &compiler,
			   const ui::parser_lock &lock,
			   const std::string &name)
			: generators_base{lock, name},
			  new_borderlayoutmanager_vector
			{
			 create_newborderlayoutmanager_vector(compiler, lock)
			},
			  generator_vector{compiler
					   .lookup_borderlayoutmanager_generators
					   (lock, name)}
		{
		}

		container create_container(const factory &f,
					   const std::string &id,
					   uielements &factories)
			const
		{
			auto nblm=new_layoutmanager(factories);

			return f->create_container
				([&, this]
				 (const auto &container)
				 {
					 generate(container, factories);
				 },
				 nblm);
		}

		inline new_borderlayoutmanager
		new_layoutmanager(uielements &factories) const
		{
			new_borderlayoutmanager nblm;

			// Generate the contents of the new_borderlayoutmanager.

			for (const auto &g:*new_borderlayoutmanager_vector)
			{
				g(&nblm, factories);
			}

			return nblm;
		}

		void generate(const container &c,
			      uielements &factories) const
		{
			borderlayoutmanager blm=
				get_new_layoutmanager(c, factories);

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

	typedef std::tuple<
		std::enable_if_t<std::is_base_of_v<generators_base,
						   typename Args::generators>>
		...> generators_must_inherit_from_generators_base;

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
				       decltype(generators.new_layoutmanager
						(elements))
				       >>)
			 {
				auto nlm=generators.new_layoutmanager(elements);

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

	if (type == "table")
		return layoutmanager_functions{
			std::in_place_type_t<tablelayoutmanager_functions>{}
		};

	if (type == "page")
		return layoutmanager_functions{
			std::in_place_type_t<pagelayoutmanager_functions>{}
		};

	if (type == "pane")
		return layoutmanager_functions{
			std::in_place_type_t<panelayoutmanager_functions>{}
		};

	if (type == "item")
		return layoutmanager_functions{
			std::in_place_type_t<itemlayoutmanager_functions>{}
		};

	if (type == "toolbox")
		return layoutmanager_functions{
			std::in_place_type_t<toolboxlayoutmanager_functions>{}
		};

	if (type == "border")
		return layoutmanager_functions{
			std::in_place_type_t<borderlayoutmanager_functions>{}
		};

	throw EXCEPTION(gettextmsg(_("\"%1%\" is not a known layout/container"),
				   type));
}

const_screen_positions uicompiler::positions_to_restore() const
{
	if (!saved_positions)
		throw EXCEPTION(_("<restore> requires saved screen_positions"));

	return saved_positions;
}

namespace {
#if 0
}
#endif

static std::string get_id_to_restore(const ui::parser_lock &lock)
{
	auto id=lock->get_any_attribute("id");

	if (id.empty())
		throw EXCEPTION(_("<restore> requires <element> to specify an"
				  " id attribute"));

	return id;
}

template<typename object_type>
inline void invoke_restore(object_type &object,
			   const ui::parser_lock &lock,
			   uicompiler &compiler)
{
	object.restore(compiler.positions_to_restore(),
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

std::optional<named_element_factory>
uicompiler::compiler_functions
::get_optional_elements(uicompiler &compiler,
			const ui::parser_lock &lock)
{
	auto clone=lock->clone();

	auto xpath=clone->get_xpath("elements");
	if (xpath->count() == 0)
		return {};

	xpath->to_node();

	auto &generators=compiler.generators;
	auto &uncompiled_elements=compiler.uncompiled_elements;

	auto name=clone->get_text();

	{
		auto iter=generators->elements_generators.find(name);

		if (iter != generators->elements_generators.end())
			return {{name, iter->second}};
	}

	auto iter=compiler.find_uncompiled(name, "factory", "elements");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=compiler.elements_parseconfig(new_lock);

	generators->elements_generators.insert_or_assign(name, ret);

	return {{name, ret}};
}

uicompiler::uncompiled_elements_t
::iterator uicompiler::find_uncompiled(const std::string &name,
				       const char *element,
				       const char *type)
{
	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != element
	    || iter->second->get_any_attribute("type") != type)
	{
		throw EXCEPTION(gettextmsg
				(_("Layout or factory \"%1%\" "
				   "is not found or cannot be removed.  "
				   "Possible reasons are: It is the wrong "
				   "<type>, "
				   "it is referenced by another layout "
				   "or factory, or it is "
				   "now a part of a recursive sequence "
				   "of layouts and factories"),
				 name));
	}

	return iter;
}

functionptr<void (THREAD_CALLBACK, const tooltip_factory &)>
uicompiler::compiler_functions::get_optional_tooltip(uicompiler &compiler,
						     const ui::parser_lock
						     &lock)
{
	functionptr<void (THREAD_CALLBACK,
			  const tooltip_factory &)> optional_tooltip;

	auto id=lock->get_any_attribute("tooltip");

	if (!id.empty())
	{
		auto iter=compiler.generators->tooltip_generators.find(id);

		if (iter == compiler.generators->tooltip_generators.end())
			throw EXCEPTION(gettextmsg
					(_("Tooltip \"%1%\" is not defined"),
					 id));
		optional_tooltip=iter->second;
	}

	return optional_tooltip;
}

void uicompiler::compiler_functions
::install_tooltip(const element &e,
		  const functionptr<void (THREAD_CALLBACK,
					  const tooltip_factory &)>
		  &optional_tooltip)
{
	if (!optional_tooltip)
		return;

	e->create_custom_tooltip(optional_tooltip);
}

std::optional<uicompiler::compiler_functions::contextpopup_t>
uicompiler::compiler_functions::get_optional_contextpopup
(uicompiler &compiler, const ui::parser_lock &lock)
{
	auto clone=lock->clone();

	auto xpath=clone->get_xpath("context");

	if (xpath->count() == 0)
		return std::nullopt;

	xpath->to_node();
	auto id=clone->get_any_attribute("id");

	return std::tuple{compiler.listlayout_parseconfig(clone,
							  "menu",
							  "element"),
			id,
			compiler.shortcut_value(clone,
						"shortcut",
						"element")
			};
}

void uicompiler::compiler_functions
::install_contextpopup(uielements &elements,
		       const element &new_element,
		       const contextpopup_t &popup_info)
{
	const auto &[generator, id, sc]=popup_info;

	// Determine whether the context popup has cut/copy/paste
	// menu items, and if so we'll take care of update()ing them.

	auto orig_ccp=elements.new_copy_cut_paste_menu_items;

	elements.new_copy_cut_paste_menu_items=nullptr;

	auto menu=new_element->create_popup_menu
		([&]
		 (const auto &lm)
		 {
			 generator(lm, elements);
		 });

	if (!id.empty())
		elements.new_elements.insert_or_assign(id, menu);

	auto ccp=elements.new_copy_cut_paste_menu_items;
	elements.new_copy_cut_paste_menu_items=orig_ccp;

	// Install an on_state_update that automatically calls
	// copy/cut/paste update() before the popup gets shown.

	if (ccp)
		menu->on_state_update
			([ccp=copy_cut_paste_menu_items{ccp}]
			 (ONLY IN_THREAD,
			  const auto &new_state,
			  const auto &busy)
			 {
				 if (new_state.state_update ==
				     new_state.before_showing)
				 {
					 ccp->update(IN_THREAD);
				 }
			 });

	new_element->install_contextpopup_callback
		([menu]
		 (ONLY IN_THREAD,
		  const auto &me,
		  const auto &trigger,
		  const auto &mcguffin)
		 {
			 menu->show();
		 },
		 sc);
}

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
					const ui::parser_lock &lock,
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

container uicompiler::create_container(const factory &f,
				       uielements &factories,
				       const std::string &name,
				       const container_generators_t &generators)
{
	// std::visit needs a variant to work with.
	const container_generators_t::variant_t &v=generators;

	auto c=std::visit([&]
			  (const auto &generators) -> container
			  {
				  return generators
					  .create_container(f, name, factories);
			  }, v);

	factories.new_elements.insert_or_assign(name, c);

	return c;
}

static element create_input_field(
	const factory &generic_factory,
	uielements &elements,
	const std::string &id,
	const text_param &input_field_value,
	const input_field_config &config_value)
{
	// Check if an input field validator was provided.

	if (!id.empty())
	{
		auto iter=elements.input_field_validators.find(id);

		if (iter != elements.input_field_validators.end())
		{
			const auto &[callback, contents] = iter->second;
			text_param initial_value;

			// Obtain the initial value of the validated input
			// field, and create it.

			auto validated=contents->to_text_param(initial_value);

			auto field=generic_factory->create_input_field(
				initial_value,
				validated,
				callback,
				config_value);

			// Created the validated value object and save it
			// in uielements.

			auto validated_value=
				contents->create_validated_input_field(field);

			if (!elements.new_validated_input_fields.emplace(
				    id,
				    validated_value
			    ).second)
			{
				throw EXCEPTION(
					gettextmsg(
						_("Input field validator "
						  "\"%1%\" already defined"),
						id)
				);
			}
			return field;
		}
	}
	return generic_factory->create_input_field(
		input_field_value,
		config_value
	);
}
#include "uicompiler.inc.H/factory_parse_parameters.H"
#include "uicompiler.inc.H/factory_parser.H"

std::tuple<text_param, label_config>
uicompiler::get_label_parameters(const ui::parser_lock &lock)
{
	return get_create_label_parameters(*this, lock);
}

LIBCXXW_NAMESPACE_END
