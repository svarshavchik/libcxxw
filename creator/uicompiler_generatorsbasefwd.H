/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_uicompiler_generatorsbasefwd_h
#define creator_uicompiler_generatorsbasefwd_h

#include <x/ptrfwd.H>
#include <string>
#include <vector>
#include <tuple>
#include "creator/appgenerator_functionfwd.H"
#include "x/w/text_paramfwd.H"

class uicompiler_generatorsbaseObj;

/*! Interface to a uicompiler_generators

*/

typedef x::ref<uicompiler_generatorsbaseObj> uicompiler_generatorsbase;

//! A constant \ref uicompiler_generators "uicompiler generator interface.

//! \see uicompiler_generators

typedef x::const_ref<uicompiler_generatorsbaseObj> const_uicompiler_generatorsbase;

//! A nullable pointer reference to a \ref uicompiler_generators "uicompiler generator interface.

//! \see uicompiler_generators

typedef x::ptr<uicompiler_generatorsbaseObj> uicompiler_generatorsbaseptr;

//! A nullable pointer reference to a const \ref uicompiler_generators "uicompiler generator interface.

//! \see uicompiler_generators

typedef x::const_ptr<uicompiler_generatorsbaseObj> const_uicompiler_generatorsbaseptr;

typedef std::vector<std::tuple<const_appgenerator_function, x::w::text_param>
	    > compiler_available_functions_t;

//! What kind of a generator

//! Initialized from the "type" attribute.

enum class appuigenerator_type {
	elements,
	list_items,
	factory,
	layoutmanager,
	new_layoutmanager
};

//! What kind of a generator it is, how it is identified.

struct type_category_t {

	//! Type of this generator.

	appuigenerator_type type=appuigenerator_type::elements;

	//! The XML node name, "layout" or "factory".

	const char *xml_node_name() const;

	//! The category of this generator

	//! This is the <category> label in uicompiler.xml.
	//!
	//! When parsing something like:
	//!
	//!     <factory type="grid" id="identifier">
	//!
	//! or
	//!
	//!     <layout type="grid" id="identifier"
	//!
	//! The "type" value specifies the <category> in uicompiler.xml
	//! that parses this factory or layout, and factory or layout
	//! is mapped to appuigenerator_type.

	std::string category;
};
#endif
