/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_appgenerator_save_h
#define creator_appgenerator_save_h

#include "creator/appgenerator_savefwd.H"
#include "creator/appgenerator_functionsfwd.H"
#include "creator/app.H"
#include "x/w/connection_threadfwd.H"
#include <x/vector.H>
#include <x/functionalrefptr.H>
#include <vector>

//! Information used when saving/updating a generator

//! Information about any additional work that needs to happen.
struct appgenerator_save {

	//! Save additional document information.

	//! Additional callbacks to invoke after the XML document was
	//! updated with the updated generator's contents.
	//!
	//! This is used to automatically create a new layour or factory if
	//! it was referenced by the updated generator but it does not exist
	//! yet.

	std::vector<x::functionref<void (const x::xml::writelock &,
					 appgenerator_save &)>
		    > extra_saves;

	//! New layouts and factories that were created when saving generators

	//! If the new theme gets successfully insalled, these ids get
	//! added to the generator name combo-box.

	std::vector<std::string> new_layouts_and_factories;
};

#endif
