/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uigeneratorsobj_h
#define x_w_uigeneratorsobj_h

#include <x/w/uigeneratorsfwd.H>
#include <x/w/rgb.H>
#include <x/w/border_argfwd.H>
#include <x/w/border_infomm.H>
#include <x/w/font_arg.H>
#include <x/w/all_appearancesfwd.H>
#include <x/functionalrefptrfwd.H>
#include <x/functionalrefptrobj.H>
#include <x/obj.H>
#include <x/visitor.H>
#include <x/vectorfwd.H>
#include <x/messagesfwd.H>
#include <x/refptr_traits.H>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see uigenerators

class uigeneratorsObj: virtual public obj {

public:
	//! Constructor
	uigeneratorsObj() LIBCXX_HIDDEN;

	//! Destructor
	~uigeneratorsObj() LIBCXX_HIDDEN;

	//! Optional message catalog
	const_messagesptr catalog;

	//! Parsed dimensions

	std::unordered_map<std::string, double> dims;

	//! Look up a parsed dimension.
	dim_arg lookup_dim(const std::string &name,
			   bool allowthemerefs,
			   const char *tag) const;

	//! Custom colors loaded from this layout.

	std::unordered_map<std::string, theme_color_t> colors;

	//! Look up a parsed color
	color_arg lookup_color(const std::string &name,
			       bool allowthemerefs,
			       const char *tag) const;

	//! Borders

	std::unordered_map<std::string, border_infomm> borders;

	//! Look up a parsed border.
	//! TODO: should be std::string_view in C++20
	border_arg lookup_border(const std::string &name,
				 bool allowthemerefs,
				 const char *tag) const;

	//! Fonts
	std::unordered_map<std::string, font> fonts;

	//! Look up a parsed font.
	font_arg lookup_font(const std::string &name,
			     bool allowthemerefs,
			     const char *tag) const;

	//! Loaded appearance objects.
	std::unordered_map<std::string, const_appearance> loaded_appearances;

	//! Retrieve an appearance object
	const_appearance lookup_appearance(const std::string_view &name) const;

	//! Generators used by the grid layout manager

	std::unordered_map<std::string,
			   const_vector<gridlayoutmanager_generator>
			   > gridlayoutmanager_generators;

	//! Generators used by the generic factory

	std::unordered_map<std::string,
			   const_vector<factory_generator>
			   > factory_generators;

	//! Generators used by the grid factory
	std::unordered_map<std::string,
			   const_vector<gridfactory_generator>
			   > gridfactory_generators;

	//! Generators used by the menu bar factory
	std::unordered_map<std::string,
			   const_vector<menubarfactory_generator>
			   > menubarfactory_generators;

	//! Generators used by the list layout manager

	std::unordered_map<std::string,
			   const_vector<listlayoutmanager_generator>
			   > listlayoutmanager_generators;

	//! Generators used by the table layout manager

	std::unordered_map<std::string,
			   const_vector<tablelayoutmanager_generator>
			   > tablelayoutmanager_generators;

	//! Generators used by the pane factory
	std::unordered_map<std::string,
			   const_vector<panefactory_generator>
			   > panefactory_generators;

	//! Generators used by the pane layout manager

	std::unordered_map<std::string,
			   const_vector<panelayoutmanager_generator>
			   > panelayoutmanager_generators;

	//! Generators used by the item layout manager

	std::unordered_map<std::string,
			   const_vector<itemlayoutmanager_generator>
			   > itemlayoutmanager_generators;

	//! Generators used by the page layout manager

	std::unordered_map<std::string,
			   const_vector<pagelayoutmanager_generator>
			   > pagelayoutmanager_generators;

	//! Generators used by the toolbox layout manager

	std::unordered_map<std::string,
			   const_vector<toolboxlayoutmanager_generator>
			   > toolboxlayoutmanager_generators;

	//! Generators used by the standard combobox layout manager

	std::unordered_map<std::string,
			   const_vector<standard_comboboxlayoutmanager_generator
					>
			   > standard_comboboxlayoutmanager_generators;

	//! Generators used by the editable combobox layout manager

	std::unordered_map<std::string,
			   const_vector<editable_comboboxlayoutmanager_generator
					>
			   > editable_comboboxlayoutmanager_generators;

	//! Generators used by the menu bar layout manager

	std::unordered_map<std::string,
			   const_vector<menubarlayoutmanager_generator>
			   > menubarlayoutmanager_generators;

	//! Generators used by the book layout manager
	std::unordered_map<std::string,
			   const_vector<booklayoutmanager_generator>
			   > booklayoutmanager_generators;

	//! Generators used by the border layout manager
	std::unordered_map<std::string,
			   const_vector<borderlayoutmanager_generator>
			   > borderlayoutmanager_generators;

	//! Generators used by the peephole layout manager
	std::unordered_map<std::string,
			   const_vector<peepholelayoutmanager_generator>
			   > peepholelayoutmanager_generators;

	//! Generators used by the page factory
	std::unordered_map<std::string,
			   const_vector<pagefactory_generator>
			   > pagefactory_generators;

	//! Generators used by the book page factory
	std::unordered_map<std::string,
			   const_vector<bookpagefactory_generator>
			   > bookpagefactory_generators;

	//! Generators used by the toolbox factory

	std::unordered_map<std::string,
			   const_vector<toolboxfactory_generator>
			   > toolboxfactory_generators;

	//! Generic elements generators

	std::unordered_map<std::string,
			   const_vector<elements_generator>
			   > elements_generators;


};

LIBCXXW_NAMESPACE_END

#endif
