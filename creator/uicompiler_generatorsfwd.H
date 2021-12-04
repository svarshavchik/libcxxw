/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_uicompiler_generatorsfwd_h
#define creator_uicompiler_generatorsfwd_h

#include <x/ptrfwd.H>
#include <string>
#include <unordered_map>
#include "creator/uicompiler_generatorsbasefwd.H"

class uicompiler_generatorsObj;

/*! Parsed <layout> or <factory> from uicompiler.xml

All of these objects are stored in \ref uicompiler "uicompiler"'s uigenerators,
so any intermediate references between generators that use each other can use
plain pointers.

*/

typedef x::ref<uicompiler_generatorsObj> uicompiler_generators;

//! A constant \ref uicompiler_generators "parsed generator".

//! \see uicompiler_generators

typedef x::const_ref<uicompiler_generatorsObj> const_uicompiler_generators;

//! A nullable pointer reference to a \ref uicompiler_generators "parsed generator".

//! \see uicompiler_generators

typedef x::ptr<uicompiler_generatorsObj> uicompiler_generatorsptr;

//! A nullable pointer reference to a const \ref uicompiler_generators "parsed generator".

//! \see uicompiler_generators

typedef x::const_ptr<uicompiler_generatorsObj> const_uicompiler_generatorsptr;

//! Look up generators by their name.

typedef std::unordered_map<std::string,
			   const_uicompiler_generators> uigenerators_t;

//! A hash of a <layout> or <factory>, plus its type.

struct generator_type_spec_hash {

	typedef void is_transparent;

	//! Hash function.

	size_t operator()(const const_uicompiler_generators &a) const noexcept;

	//! Hash function.

	size_t operator()(const type_category_t &a) const noexcept;
};

//! Compare the <layout>+<factory> for equality.

//! The transparent comparator supports compating const_uicompiler_generators
//! and type_category_t's.

struct generator_type_spec_equ {

	//! This is a transparent comparator.
	typedef void is_transparent;

	// Possible argument.

	struct arg {
		const type_category_t &c;

		arg(const const_uicompiler_generators &);
		arg(const type_category_t &);
	};

	//! Compare for equality.

	bool operator()(arg a, arg b) const noexcept;
};

#endif
