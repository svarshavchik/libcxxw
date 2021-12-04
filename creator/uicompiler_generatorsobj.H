/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_uicompiler_generatorsobj_h
#define creator_uicompiler_generatorsobj_h

#include "creator/uicompiler_generatorsbaseobj.H"
#include "creator/appgenerator_functionfwd.H"
#include <x/obj.H>
#include <x/ptrfwd.H>
#include <vector>
#include <string>

//! Implementation object.

//! \see uicompiler_generators

class uicompiler_generatorsObj: public uicompiler_generatorsbaseObj {

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

	//! The <config> generator

	//! All generators are in the same container, so this pointer will
	//! be valid.
	const uicompiler_generatorsObj *config=nullptr;

	//! The <use_common> generator

	//! All generators are in the same container, so this pointer will
	//! be valid.
	const uicompiler_generatorsObj *parent=nullptr;

	//! Implement parse()

	std::vector<const_appgenerator_function
		    > parse(const x::xml::readlock &root,
			    const uigenerators_t &all_generators)
		const;

	//! Implement available_functions

	compiler_available_functions_t available_functions() const override;
};

#endif
