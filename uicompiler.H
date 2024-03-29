/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uicompiler_h
#define x_w_uicompiler_h

#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/menubarlayoutmanagerfwd.H"
#include "x/w/menubarfactoryfwd.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "x/w/booklayoutmanagerfwd.H"
#include "x/w/bookpagefactoryfwd.H"
#include "x/w/borderlayoutmanagerfwd.H"
#include "x/w/pagefactoryfwd.H"
#include "x/w/factoryfwd.H"
#include "x/w/uielementsfwd.H"
#include "x/w/shortcutfwd.H"
#include "x/w/border_argfwd.H"
#include "x/w/all_appearancesfwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/font_arg.H"
#include "x/w/screen_positions.H"
#include "x/w/elementfwd.H"
#include "x/w/progressbarfwd.H"
#include "x/w/uigenerators.H"
#include "x/w/appearance.H"
#include "x/w/containerfwd.H"
#include "x/w/synchronized_axisfwd.H"
#include "theme_parser_lockfwd.H"
#include "x/w/impl/uixmlparser.H"
#include "uicompilerfwd.H"

#include <x/vector.H>
#include <x/refptr_traits.H>
#include <tuple>
#include <optional>
#include <unordered_map>
#include <variant>

LIBCXXW_NAMESPACE_START

/*! Compiler for XML-specified display elements.

The bulk of the compiler gets robo-generated from uicompiler.xml. This
declares the uicompiler-generated methods, and provides some supporting
functions.

 */

struct uicompiler {

public:

	//! Constructor

	//! Take the XML node represented by lock, and populate the generators
	//! with the compiled code.
	//!
	//! Once constructed, this object is just an empty hulk that can be
	//! discarded.
	//!
	//! "allowthemerefs" gets set to false when parsing the default
	//! theme, and true when parsing a user-specified layout/theme.

	uicompiler(const ui::parser_lock &lock,
		   const uigenerators &generators,
		   bool allowthemerefs);

	//! Load font definitions.

	//! This is also used by the SXG file parser.
	//! lookup_functor searches for an existing font. If it's not found
	//! install_functor gets invoked to save the new font definition.

	template<typename install_functor, typename lookup_functor>
		static void load_fonts(const ui::parser_lock &lock,
				       install_functor &&install,
				       lookup_functor &&lookup)
	{
		do_load_fonts(lock,
			      make_function<void (const std::string &,
						  const font &)>
			      (std::forward<install_functor>(install)),
			      make_function<std::optional<font>
			      (const std::string &)>
			      (std::forward<lookup_functor>(lookup)));
	}

	//! Type-erased do_load_fonts

	static void do_load_fonts(const ui::parser_lock &lock,
				  const function<void(const std::string &,
						      const font &)> &install,
				  const function<std::optional<font>
				  (const std::string &)>
				  &lookup);


	//! What's being generated.
	const uigenerators generators;

	//! Whether this is a custom theme compiler

	//! Custom theme compilers will allow various entities (colors,
	//! fonts) referenced by widgets to be undefined in the theme file,
	//! and presumed to be referring to the default theme. When compiling
	//! the default theme this is false.
	const bool allowthemerefs;

private:

	//! The uncompiled_elements' container type.

	typedef std::unordered_map<std::string,
				   ui::parser_lock,
				   std::hash<std::string>,
				   std::equal_to<void>
				   > uncompiled_elements_t;

	//! Top level factory and layouts that aren't compiled yet.

	//! The constructor puts all <factory> and <layout>
	//! into this lookup map, keyed by their @id attribute.
	//!
	//! The constructor then repeatedly removes an element from this map
	//! and compiles, placing the result into the generators, until the
	//! map is empty.
	//!
	//! A generator may recursively invoke another generator. If it's
	//! already been generated it gets plucked from the generators,
	//! otherwise it's removed from uncompiled_elements and compiled,
	//! recursively.
	uncompiled_elements_t uncompiled_elements;

	//! Look up something specific in the uncompiled_elements

	uncompiled_elements_t::iterator find_uncompiled(const std::string &name,
							const char *element,
							const char *type);

public:
#include "uicompiler_layoutparsers.inc.H"

	//! Generate a single layout manager instruction.r
	functionref<void (const layoutmanager &layout,  uielements &elements)>
	layout_parser(const ui::parser_lock &lock);

	vector<functionref<void (const layoutmanager &layout,
				 uielements &elements)>>
	layout_parseconfig(const ui::parser_lock &lock);

	//! Parser for base factory generator.
	vector<factory_generator
	       > factory_parseconfig(const ui::parser_lock &lock);

	//! Wrapper for factory_parseconfig()

	//! Parses a parameter that represents a creator that receives a
	//! a factory, and uses the factory to create one or more elements.
	//!
	//! Referenced from the theme file. Receives the name of the element
	//! that specifies what gets created.

	factory_generator factory_parseconfig(const ui::parser_lock &lock,
					      const char *element,
					      const char *parent);

	//! Parser for grid factory generator.
	vector<gridfactory_generator
	       > gridfactory_parseconfig(const ui::parser_lock &lock);

	//! Parser for pane factory generator.
	vector<panefactory_generator
	       > panefactory_parseconfig(const ui::parser_lock &lock);

	//! Parser for toolbox factory generator.
	vector<toolboxfactory_generator
	       > toolboxfactory_parseconfig(const ui::parser_lock &lock);

	//! Parser for menu bar factory generator.
	vector<menubarfactory_generator
	       > menubarfactory_parseconfig(const ui::parser_lock &lock);

	//! Parser for page factory generator.
	vector<pagefactory_generator
	       > pagefactory_parseconfig(const ui::parser_lock &lock);

	//! Parser for book page factory generator.
	vector<bookpagefactory_generator
	       > bookpagefactory_parseconfig(const ui::parser_lock &lock);

	//! Parser for singleton layout manager generator.
	vector<singletonlayoutmanager_generator
	       > singletonlayout_parseconfig(const ui::parser_lock &lock);

	//! Wrapper for listlayout_parseconfig()

	//! Parses a parameter that represents a creator that receives a
	//! a listlayoutmanager, and generates the list.
	//!
	//! Referenced from the theme file. Receives the name of the element
	//! that specifies what gets created.

	listlayoutmanager_generator
	listlayout_parseconfig(const ui::parser_lock &lock,
			       const char *element,
			       const char *parent);

	//! Parser for the new grid layout manager generator

	vector<new_gridlayoutmanager_generator
	       > new_gridlayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new book layout manager generator

	vector<new_booklayoutmanager_generator
	       > new_booklayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new list or combobox layout manager generator

	vector<new_list_or_combobox_layoutmanager_generator
	       > new_list_or_combobox_layout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new list layout manager generator

	vector<new_listlayoutmanager_generator
	       > new_listlayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new custom combobox layout manager generator

	vector<new_custom_comboboxlayoutmanager_generator
	       > new_custom_comboboxlayout_parseconfig(const ui::parser_lock
						       &lock);

	//! Parser for the new standard combobox layout manager generator

	vector<new_standard_comboboxlayoutmanager_generator
	       > new_standard_comboboxlayout_parseconfig(const ui::parser_lock &lock);
	//! Parser for the new editable combobox layout manager generator

	vector<new_editable_comboboxlayoutmanager_generator
	       > new_editable_comboboxlayout_parseconfig(const ui::parser_lock
							 &lock);

	//! Parser for the new table layout manager generator

	vector<new_tablelayoutmanager_generator
	       > new_tablelayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new pane layout manager generator

	vector<new_panelayoutmanager_generator
	       > new_panelayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new item layout manager generator

	vector<new_itemlayoutmanager_generator
	       > new_itemlayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new page layout manager generator

	vector<new_pagelayoutmanager_generator
	       > new_pagelayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new toolbox layout manager generator

	vector<new_toolboxlayoutmanager_generator
	       > new_toolboxlayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new border layout manager generator

	vector<new_borderlayoutmanager_generator
	       > new_borderlayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for the new peephole layout manager generator

	vector<new_peepholelayoutmanager_generator
	       > new_peepholelayout_parseconfig(const ui::parser_lock &lock);

	//! Parser for uielements-based generator
	vector<elements_generator>
	elements_parseconfig(const ui::parser_lock &lock);

private:
	//! Implement <element>

	//! Executed by the compiled <element> code.

	static void generate(const factory &,
			     uielements &,
			     const std::string &name);

#include "uicompiler_factoryparsers.inc.H"

	//! Generate a single new_gridlayoutmanager instruction.
	new_gridlayoutmanager_generator
	new_gridlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_booklayoutmanager instruction.
	new_booklayoutmanager_generator
	new_booklayout_parser(const ui::parser_lock &lock);

	//! Generate a single singleton layout manager instruction.
	singletonlayoutmanager_generator
	singletonlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_list_or_combobox_layoutmanager instruction.
	new_list_or_combobox_layoutmanager_generator
	new_list_or_combobox_layout_parser(const ui::parser_lock &lock);

	//! Generate a single new_listlayoutmanager instruction.
	new_listlayoutmanager_generator
	new_listlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_custom_comboboxlayoutmanager instruction.
	new_custom_comboboxlayoutmanager_generator
	new_custom_comboboxlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_standard_comboboxlayoutmanager instruction.
	new_standard_comboboxlayoutmanager_generator
	new_standard_comboboxlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_editable_comboboxlayoutmanager instruction.
	new_editable_comboboxlayoutmanager_generator
	new_editable_comboboxlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_tablelayoutmanager instruction.
	new_tablelayoutmanager_generator
	new_tablelayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_panelayoutmanager instruction.
	new_panelayoutmanager_generator
	new_panelayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_itemlayoutmanager instruction.
	new_itemlayoutmanager_generator
	new_itemlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_pagelayoutmanager instruction.
	new_pagelayoutmanager_generator
	new_pagelayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_toolboxlayoutmanager instruction.
	new_toolboxlayoutmanager_generator
	new_toolboxlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_borderlayoutmanager instruction.
	new_borderlayoutmanager_generator
	new_borderlayout_parser(const ui::parser_lock &lock);

	//! Generate a single new_borderlayoutmanager instruction.
	new_peepholelayoutmanager_generator
	new_peepholelayout_parser(const ui::parser_lock &lock);

	//! Generate a single uielements instruction.
	elements_generator
	elements_parser(const ui::parser_lock &lock);

	struct list_items_parse_info_t;

	//! Generate parsers for new list items

	vector<functionref<void (const list_items_parse_info_t * &)>>
	list_items_parseconfig(const ui::parser_lock &lock);

	//! Generate a single parser for new list layout manager items
	functionref<void (const list_items_parse_info_t * &)>
	list_items_parser(const ui::parser_lock &lock);

public:
	//! Return compiled factory generators.

	//! Extract single_value() from element, and look it up in
	//! factory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into factory_generators, and return them.

	const_vector<factory_generator
		     > lookup_factory_generators(const ui::parser_lock &lock,
						 const char *element,
						 const char *parent);

	//! Return compiled grid factory generators.

	//! Extract single_value() from element, and look it up in
	//! gridfactory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into gridfactory_generators, and return them.

	const_vector<gridfactory_generator
	       > lookup_gridfactory_generators(const ui::parser_lock &lock,
					       const char *element,
					       const char *parent);

	//! Return compiled menu bar factory generators.

	//! Extract single_value() from element, and look it up in
	//! gridfactory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into menubarlayoutmanager_generators, and return them.

	const_vector<menubarlayoutmanager_generator>
	lookup_menubarlayoutmanager_generators(const ui::parser_lock &lock,
					       const char *element,
					       const char *parent);

	//! Return compiled menu bar factory generators.

	//! Extract single_value() from element, and look it up in
	//! gridfactory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into menubarfactory_generators, and return them.

	const_vector<menubarfactory_generator
	       > lookup_menubarfactory_generators(const ui::parser_lock &lock,
					       const char *element,
					       const char *parent);

	//! Return compiled page factory generators.

	//! Extract single_value() from element, and look it up in
	//! pagefactory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into pagefactory_generators, and return them.

	const_vector<pagefactory_generator>
	lookup_pagefactory_generators(const ui::parser_lock &lock,
				      const char *element,
				      const char *parent);

	//! Return compiled book page factory generators.

	//! Extract single_value() from element, and look it up in
	//! bookpagefactory_generators, returning the generators. If it doesn't
	//! exist, search for them in uncompiled_elements, compile them, put
	//! them into bookpagefactory_generators, and return them.

	const_vector<bookpagefactory_generator>
	lookup_bookpagefactory_generators(const ui::parser_lock &lock,
					  const char *element,
					  const char *parent);

	//! Return compiled grid layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into gridlayoutmanager_generators, and return them.

	const_vector<gridlayoutmanager_generator>
	lookup_gridlayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled list layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into listlayoutmanager_generators, and return them.

	const_vector<listlayoutmanager_generator>
	lookup_listlayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled combobox layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into standard_comboboxlayoutmanager_generators, and return them.

	const_vector<standard_comboboxlayoutmanager_generator>
	lookup_standard_comboboxlayoutmanager_generators
	(const ui::parser_lock &lock,
	 const std::string &);

	//! Return compiled combobox layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into editable_comboboxlayoutmanager_generators, and return them.

	const_vector<editable_comboboxlayoutmanager_generator>
	lookup_editable_comboboxlayoutmanager_generators
	(const ui::parser_lock &lock,
	 const std::string &);

	//! Return compiled book layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into booklayoutmanager_generators, and return them.

	const_vector<booklayoutmanager_generator>
	lookup_booklayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled table layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into tablelayoutmanager_generators, and return them.

	const_vector<tablelayoutmanager_generator>
	lookup_tablelayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled pane layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into panelayoutmanager_generators, and return them.

	const_vector<panelayoutmanager_generator>
	lookup_panelayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled item layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into itemlayoutmanager_generators, and return them.

	const_vector<itemlayoutmanager_generator>
	lookup_itemlayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled page layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into pagelayoutmanager_generators, and return them.

	const_vector<pagelayoutmanager_generator>
	lookup_pagelayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled toolbox layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into toolboxlayoutmanager_generators, and return them.

	const_vector<toolboxlayoutmanager_generator>
	lookup_toolboxlayoutmanager_generators(const ui::parser_lock &lock,
					    const std::string &);

	//! Return compiled border layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into borderlayoutmanager_generators, and return them.

	const_vector<borderlayoutmanager_generator>
	lookup_borderlayoutmanager_generators(const ui::parser_lock &lock,
					      const std::string &);

	//! Return compiled peephole layout manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into peepholelayoutmanager_generators, and return them.

	const_vector<peepholelayoutmanager_generator>
	lookup_peepholelayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &);

	//! Return compiled pane factory manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into panefactory_generators, and return them.

	const_vector<panefactory_generator>
	lookup_panefactory_generators(const ui::parser_lock &lock,
				      const char *element,
				      const char *parent);

	//! Return compiled toolbox factory manager generators.

	//! If the generators are already compiled, return them. If not,
	//! search for them in uncompiled_elements, compile them, put them
	//! into toolboxfactory_generators, and return them.

	const_vector<toolboxfactory_generator>
	lookup_toolboxfactory_generators(const ui::parser_lock &lock,
				      const char *element,
				      const char *parent);

private:
	//! Execute <replace>

	static void singletonlayout_replace(const singletonlayoutmanager &,
					    uielements &,
					    const const_vector
					    <factory_generator> &);

	//! Execute <append_row>.

	//! Executed by the compiled <append_row>.
	static void gridlayout_append_row(const gridlayoutmanager &,
					  uielements &,
					  const const_vector
					  <gridfactory_generator> &
					  );

	//! Execute <insert_row>.

	//! Executed by the compiled <insert_row>.
	static void gridlayout_insert_row(const gridlayoutmanager &,
					  size_t,
					  uielements &,
					  const const_vector
					  <gridfactory_generator> &
					  );

	//! Execute <replace_row>.

	//! Executed by the compiled <insert_row>.
	static void gridlayout_replace_row(const gridlayoutmanager &,
					   size_t,
					   uielements &,
					   const const_vector
					   <gridfactory_generator> &
					   );

	//! Execute <append_columns>.

	//! Executed by the compiled <append_columns>.
	static void gridlayout_append_columns(const gridlayoutmanager &,
					      size_t,
					      uielements &,
					      const const_vector
					      <gridfactory_generator> &
					      );

	//! Execute <insert_columns>.

	//! Executed by the compiled <insert_columns>.
	static void gridlayout_insert_columns(const gridlayoutmanager &,
					      size_t,
					      size_t,
					      uielements &,
					      const const_vector
					      <gridfactory_generator> &);

	//! Execute <replace_cell>

	//! Executed by the compiled <replace_cell>.
	static void gridlayout_replace_cell(const gridlayoutmanager &,
					    size_t,
					    size_t,
					    uielements &,
					    const const_vector
					    <gridfactory_generator> &);

	//! Execute a grid factory generator.
	static void generate_gridfactory(const gridfactory &,
					 uielements &,
					 const const_vector
					 <gridfactory_generator>
					 &generators);

	//! Execute a menu bar factory generator.
	static void generate_menubarfactory(const menubarfactory &,
					    uielements &,
					    const const_vector
					    <menubarfactory_generator>
					    &generators);

	//! Execute <append_pages>.

	//! Executed by the compiled <append_pages>.
	static void booklayout_append_pages(const booklayoutmanager &,
					    uielements &,
					    const const_vector<
					    bookpagefactory_generator> &);

	//! Execute <insert_pages>.

	//! Executed by the compiled <insert_pages>.
	static void booklayout_insert_pages(const booklayoutmanager &,
					    size_t,
					    uielements &,
					    const const_vector<
					    bookpagefactory_generator> &);

	//! Execute <append_menus>.

	//! Executed by the compiled <append_menus>.
	static void menubarlayout_append_menus(const menubarlayoutmanager &,
					       uielements &,
					       const const_vector
					       <menubarfactory_generator> &);
	//! Execute <append_right_menus>.

	//! Executed by the compiled <append_right_menus>.
	static void menubarlayout_append_right_menus
	(const menubarlayoutmanager &,
	 uielements &,
	 const const_vector <menubarfactory_generator> &);

	//! Execute <insert_menus>.

	//! Executed by the compiled <insert_menus>.

	static void menubarlayout_insert_menus(const menubarlayoutmanager &,
					       uielements &,
					       size_t,
					       const const_vector
					       <menubarfactory_generator> &);
	//! Execute <insert_right_menus>.

	//! Executed by the compiled <insert_right_menus>.

	static void menubarlayout_insert_right_menus
	(const menubarlayoutmanager &,
	 uielements &,
	 size_t,
	 const const_vector <menubarfactory_generator> &);

	//! Supported grid layout manager functionality.

	struct gridlayoutmanager_functions;

	//! Supported book layout manager functionality.

	struct booklayoutmanager_functions;

	//! Supported list layout manager functionality.

	struct listlayoutmanager_functions;

	//! Supported standard combobox layout manager functionality.

	struct standard_comboboxlayoutmanager_functions;

	//! Supported editable combobox layout manager functionality.

	struct editable_comboboxlayoutmanager_functions;

	//! Supported table layout manager functionality.

	struct tablelayoutmanager_functions;

	//! Supported pane layout manager functionality.

	struct panelayoutmanager_functions;

	//! Supported item layout manager functionality.

	struct itemlayoutmanager_functions;

	//! Supported page layout manager functionality.

	struct pagelayoutmanager_functions;

	//! Supported toolbox layout manager functionality.

	struct toolboxlayoutmanager_functions;

	//! Supported border layout manager functionality.

	struct borderlayoutmanager_functions;

	//! Supporter peephole layout manager functionality

	struct peepholelayoutmanager_functions;

	//! All supported layout manager funcitonality.

	typedef std::variant<gridlayoutmanager_functions,
			     listlayoutmanager_functions,
			     standard_comboboxlayoutmanager_functions,
			     editable_comboboxlayoutmanager_functions,
			     tablelayoutmanager_functions,
			     panelayoutmanager_functions,
			     itemlayoutmanager_functions,
			     pagelayoutmanager_functions,
			     toolboxlayoutmanager_functions,
			     borderlayoutmanager_functions,
			     booklayoutmanager_functions,
			     peepholelayoutmanager_functions
			     > layoutmanager_functions;

	//! Convert "grid" or "book" to all_layoutmanagers

	layoutmanager_functions get_layoutmanager(const std::string &type);

public:
	struct container_generators_t;

	//! Validate container <type>, and look up name's generators.

	container_generators_t
	lookup_container_generators(const std::string &type,
				    const ui::parser_lock &lock,
				    const std::string &name,
				    bool,
				    const char *tag,

				    const char *config_tag="config");
private:
	//! Parsed "horizontal" or "vertical" scrollbar value.
	enum scrollbar_type {horizontal, vertical};

public:
	//! Parse <scrollbar> value.
	static scrollbar_type lookup_scrollbar_type(const std::string &value);

	//! Create the scrollbar.
	static scrollbar create_scrollbar(scrollbar_type type,
					  const factory &f,
					  const scrollbar_config &config,
					  const const_scrollbar_appearance &a);

	//! Find or create a new synchronized axis object.
	static synchronized_axis
	lookup_synchronized_axis(uielements &elements,
				 const std::string &name);

	//! Execute <container>

	//! In a factory.
	static container create_container(const factory &f,
					  uielements &elements,
					  const std::string &name,
					  const container_generators_t
					  &generators);

	static element
	create_progressbar(const factory &generic_factory,
			   const std::string &name,
			   const container_generators_t &generators,
			   uielements &elements,
			   const progressbar_config &config);

	//! Parse a color_arg value for an appearance in the theme file.
	color_arg to_color_arg(const ui::parser_lock &lock,
			       const char *element, const char *parent);

	//! Parse a border_arg value for an appearance in the theme file.
	border_arg to_border_arg(const ui::parser_lock &lock,
				 const char *element, const char *parent);

	//! Parse a dim_arg value for an appearance in the theme file.
	dim_arg to_dim_arg(const ui::parser_lock &lock,
			   const char *element, const char *parent);

	//! Parse a font_arg value for an appearance in the theme file.
	font_arg to_font_arg(const ui::parser_lock &lock,
			     const char *element, const char *parent);

	//! Parse a text_color_arg value for an appearance in the theme file.
	text_color_arg to_text_color_arg(const ui::parser_lock &lock,
					 const char *element,
					 const char *parent);

	//! Convert a color_arg to a text_color_arg

	static text_color_arg to_text_color_arg(const color_arg &color,
						const char *element,
						const char *parent);

	//! Parse a text_param value
	text_param text_param_value(const ui::parser_lock &lock,
				    const char *element,
				    const char *parent);

	//! Parse a shortcut value
	shortcut shortcut_value(const ui::parser_lock &lock,
				const char *element,
				const char *parent);

	//! Parse an rgb value

	//! Used only by the color picker.
	rgb rgb_value(const ui::parser_lock &lock,
		      const char *element, const char *parent);

	//! Parse a font value

	//! Used only by the font picker.
	font font_value(const ui::parser_lock &lock,
			const char *element, const char *parent);



	//! Parsed list_item_params

	struct list_items_params {

		//! We compile a list of creators for the new list items.

		typedef const_vector<functionref
				     <void (std::vector<list_item_param> &,
					    uielements &)>> creators_t;

		//! Creators for the new list items.

		creators_t creators;

		//! Create the new list items.
		std::vector<list_item_param> create(uielements &elements) const;

		//! Labels for the resulting listitemhandles
		const_vector<std::string> labels;
	};

	//! Parse list_item_params
	list_items_params list_items_param_value(const ui::parser_lock &lock,
						 const char *element,
						 const char *parent);

private:
	//! Top level appearance objects that aren't compiled yet.

	//! The constructor puts all <appearance> elements
	//! into this lookup map, keyed by their @id attribute.
	//!
	//! The constructor then repeatedly removes an element from this map
	//! and compiles, placing the result into the generators, until the
	//! map is empty.
	//!
	//! An appearance object may recursively reference another one. If it's
	//! already been generated it gets plucked from the generators,
	//! otherwise it's removed from uncompiled_appearances and compiled,
	//! recursively.
	std::unordered_map<std::string,
			   ui::parser_lock> uncompiled_appearances;

	//! Return a compiled appearance

	//! Check if the given name is an uncompiled_appearance, if so
	//! remove it, and compile_appearance(). Otherwise we expect this
	//! appearance to already be generated, so we return it.

	template<typename appearance_type>
	appearance_type get_compiled_appearance(const std::string &name);

	//! Compile an uncompiled appearance.

	//! If the specified name is an uncompiled_appearance, then compile
	//! it, and put it into the generators.

	void compile_uncompiled_appearance(const std::string &name);

	//! Compile an <appearance> in the theme file.

	//! "type" is it's type attribute.

	const_appearance compile_appearance(const ui::parser_lock &parent,
					    const std::string &type);

	//! lookup_appearance() found a wrong appearance.
	static void wrong_appearance_type(const std::string_view &,
					  const char *)
		__attribute__((noreturn));

public:
	//! Look up a compiled appearance

	//! An appearance requested for use by an element. Invoked by
	//! robo-generated parser to set the value of a particular appearance
	//! object.

	template<typename appearance_type>
	appearance_type lookup_appearance(const std::string_view &name,
					  bool ignore,
					  const char *element)
	{
		auto ret=generators->lookup_appearance(name);

		if (!ret->isa<appearance_type>())
			wrong_appearance_type(name, element);

		return ret;
	}
private:

	struct LIBCXX_HIDDEN tooltip_infoObj : virtual public x::obj {

		virtual void create_custom_tooltip(const tooltip_factory &f)
			const=0;
	};

	struct LIBCXX_HIDDEN tooltip_infoObj_impl;

	struct generate_info;

	struct compiler_functions {
		static const_ptr<tooltip_infoObj>
		get_optional_tooltip(
			uicompiler &compiler,
			const ui::parser_lock &lock
		);

		static void install_tooltip(
			const element &e,
			uielements &elements,
			const const_ptr<tooltip_infoObj> &optional_tooltip
		);

		typedef std::tuple<listlayoutmanager_generator,
				   std::string,
				   shortcut> contextpopup_t;

		static std::optional<contextpopup_t>
		get_optional_contextpopup(uicompiler &compiler,
					  const ui::parser_lock &lock);

		static void install_contextpopup(uielements &elements,
						 const element &new_element,
						 const contextpopup_t
						 &popup_info);

		static std::optional<named_element_factory>
		get_optional_elements(uicompiler &compiler,
				    const ui::parser_lock &lock);
	};

	std::tuple<text_param, label_config>
	get_label_parameters(const ui::parser_lock &lock);

public:
#include "uicompiler_appearance_proto.inc.H"
};

LIBCXXW_NAMESPACE_END

#endif
