/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_object_appgenerator_functionfwd_h
#define creator_object_appgenerator_functionfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

struct object_generator_valueObj;
struct object_appgenerator_functionObj;

//! The current value of a generator for an object member.

//! The current value gets stored as a discrete object. The
//! \ref object_generator_function "parsed object member definition"
//! is constant.

typedef x::ref<object_generator_valueObj> object_generator_value;

/*! A generator for an <object> field or function

Represents some field or method in an <object>, when generating a UI.

*/

typedef x::ref<object_appgenerator_functionObj> object_appgenerator_function;

//! A constant \ref object_appgenerator_function "<object> generator".

//! \see object_appgenerator_function

typedef x::const_ref<object_appgenerator_functionObj> const_object_appgenerator_function;

//! A nullable pointer reference to a \ref object_appgenerator_function "<object> generator".

//! \see object_appgenerator_function

typedef x::ptr<object_appgenerator_functionObj> object_appgenerator_functionptr;

//! A nullable pointer reference to a const \ref object_appgenerator_function "<object> generator".

//! \see object_appgenerator_function

typedef x::const_ptr<object_appgenerator_functionObj> const_object_appgenerator_functionptr;

#endif
