/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_appgenerator_functionsfwd_h
#define creator_appgenerator_functionsfwd_h

#include "x/w/namespace.H"
#include "creator/uicompiler_generatorsbasefwd.H"
#include "creator/appgenerator_functionfwd.H"
#include <x/ptrfwd.H>
#include <vector>

struct generator_info_lock;

//! What to initialize the appgenerator_functions from

//! This parameter to generator_values_initialize() specifies the parser
//! (compiler), and the compiled/parsed_functions. This vector gets created
//! just before generator_values_initialize() gets called, so there isn't
//! much of an extra copy happening in order to populate the parsed_functions
//! vector.

struct existing_appgenerator_functions {
	const_uicompiler_generatorsbase          compilerbase;
	std::vector<const_appgenerator_function> parsed_functions;
};

class appgenerator_functionsObj;

/*! A list of generator functions

Manages a list of appgenerator_function objects, represented as a list widget,
showing this list of functions; together with the associated UI elements
(the popup for the various operations on each appgenerator_function),
including editing and creating new ones.

The application object's current_generators points to this object,
representing the currently-selected generator's functions.

Some individual generator function have a nested list of generator functions,
that are represented as additional appgenerator_function objects. They are
typically attached to a dialog window.

*/

typedef x::ref<appgenerator_functionsObj> appgenerator_functions;

//! A constant \ref appgenerator_functions "list of generator functions".

//! \see appgenerator_functions

typedef x::const_ref<appgenerator_functionsObj> const_appgenerator_functions;

//! A nullable pointer reference to a \ref appgenerator_functions "list of generator functions".

//! \see appgenerator_functions

typedef x::ptr<appgenerator_functionsObj> appgenerator_functionsptr;

//! A nullable pointer reference to a const \ref appgenerator_functions "list of generator functions".

//! \see appgenerator_functions

typedef x::const_ptr<appgenerator_functionsObj> const_appgenerator_functionsptr;

#endif
