/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_uicompiler_generatorsbaseobj_h
#define creator_uicompiler_generatorsbaseobj_h

#include "creator/uicompiler_generatorsfwd.H"
#include "creator/uicompiler_generatorsbasefwd.H"
#include "creator/appgenerator_functionfwd.H"
#include "x/w/text_paramfwd.H"
#include <x/obj.H>
#include <x/xml/readlockfwd.H>
#include <vector>

//! Implementation object.

//! \see uicompiler_generatorsbase

class uicompiler_generatorsbaseObj: virtual public x::obj {

public:
	//! Constructor
	uicompiler_generatorsbaseObj();

	//! Destructor
	~uicompiler_generatorsbaseObj();

	//! The type and category of this generator

	type_category_t type_category;

	//! Return all available functions

	//! Returns a vector of tuples of all available functions in this
	//! layout/factory generator, and their description.

	virtual compiler_available_functions_t available_functions() const=0;
};

#endif
