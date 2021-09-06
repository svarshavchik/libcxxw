/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_uicompiler_generatorsobj_h
#define creator_uicompiler_generatorsobj_h

#include "creator/uicompiler_generatorsfwd.H"
#include "creator/appgenerator_functionfwd.H"
#include "x/w/text_paramfwd.H"
#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/xml/readlockfwd.H>
#include <vector>
#include <tuple>
#include <string>

//! Implementation object.

//! \see uicompiler_generators

class uicompiler_generatorsObj: virtual public x::obj {

public:
	class implObj;

	//! Internal implementation details.

	const x::ref<implObj> impl;

	//! Constructor
	uicompiler_generatorsObj();

	//! Destructor
	~uicompiler_generatorsObj();

	//! Initialize this generator

	//! Parses it. Generator objects for all generators in uicompiler.xml
	//! get created first, then we parse each one individually.
	//!
	//! This allows us to check that references between generators are
	//! valid.
	//!
	//! After everything gets parsed, the generator get saved in a
	//! constant container, and all class members can only be accessed
	//! as const.

	void initialize(const x::xml::readlock &root,
			const uigenerators_t &generators);

	std::string name; //!< <name> in uicompiler.xml

	//! The type and category of this generator

	type_category_t type_category;

	//! The <config> generator

	//! All generators are in the same container, so this pointer will
	//! be valid.
	const uicompiler_generatorsObj *config=nullptr;

	//! The <use_common> generator

	//! All generators are in the same container, so this pointer will
	//! be valid.
	const uicompiler_generatorsObj *parent=nullptr;

	//! Parse this layout/factory.

	//! Takes this layout from the theme file and parses it into
	//! a list of appgenerator_functions.
	std::vector<appgenerator_function
		    > parse(const x::xml::readlock &root,
			    const uigenerators_t &all_generators)
		const;

	//! Return all available functions

	//! Returns a vector of tuples of all available functions in this
	//! layout/factory generator, and their description.

	std::vector<std::tuple<const_appgenerator_function, x::w::text_param>
		    > available_functions() const;
};

#endif
