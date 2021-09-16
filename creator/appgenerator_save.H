/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_appgenerator_save_h
#define creator_appgenerator_save_h

#include "creator/appgenerator_savefwd.H"
#include "creator/appgenerator_functionsfwd.H"
#include "creator/app.H"
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

	//! The container for extra_updates.

	typedef x::vector<x::functionref<void (appObj *, generator_info_lock &)>
			  > extra_updates_t;

	//! Additional callbacks to update the saved XML document

	//! Called after the new XML document passed validation. If a new
	//! layout or factory was added, this updates the list of layouts
	//! and factories displayed in the app.

	extra_updates_t extra_updates=extra_updates_t::create();
};

#endif
