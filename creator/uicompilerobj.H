/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef uicompilerobj_h
#define uicompilerobj_h

#include "uicompilerfwd.H"
#include "uicompiler_generatorsfwd.H"

#include <x/obj.H>

#include <string>
#include <vector>
#include <unordered_set>

//! Implementation object.

//! \see uicompiler

class uicompilerObj: virtual public x::obj {

public:
	//! All supported generators, by their name.

	//! Holds references on all parsed generators. This ensures that
	//! plain pointers between generators remain valid for the lifetime
	//! of all objects.
	uigenerators_t uigenerators;

	//! layoutmanager and factory generators, in sorted order.
	std::vector<std::string> sorted_available_uigenerators;

	//! layoutmanager and factory generators lookup.

	//! Given the generator type (layout or factory), and its name
	//! this looks up the appropriate generator.

	std::unordered_set<const_uicompiler_generators,
			   generator_type_spec_hash,
			   generator_type_spec_equ> uigenerators_lookup;

	//! Constructor
	uicompilerObj();

	//! Destructor
	~uicompilerObj();
};

#endif
