/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_appgenerator_functionfwd_h
#define creator_appgenerator_functionfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>

class appgenerator_functionObj;

//! Object returned by create_ui().

//! \see appgenerator_function
//!
//!
typedef x::functionref< bool () > generator_create_ui_ret_t;

//! Formatting returned by a generator function's description()

//! \see appgenerator_function

enum class description_format {
	//! For list of functions

	list,

	//! Title of the popup where this function is set.

	title
};

/*! A single generator function

Represents a function called when generating a UI.

Defines an abstract interface to a UI generator function.

*/

typedef x::ref<appgenerator_functionObj> appgenerator_function;

//! A constant \ref appgenerator_function "generator function".

//! \see appgenerator_function

typedef x::const_ref<appgenerator_functionObj> const_appgenerator_function;

//! A nullable pointer reference to a \ref appgenerator_function "generator function".

//! \see appgenerator_function

typedef x::ptr<appgenerator_functionObj> appgenerator_functionptr;

//! A nullable pointer reference to a const \ref appgenerator_function "generator function".

//! \see appgenerator_function

typedef x::const_ptr<appgenerator_functionObj> const_appgenerator_functionptr;

#endif
