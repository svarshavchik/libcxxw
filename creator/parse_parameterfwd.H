/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_parse_parameterfwd_h
#define creator_parse_parameterfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include "creator/setting_handlerfwd.H"

class parse_parameterObj;

struct setting_handler;
struct parameter_value;

/*! Parsed \ref "uicompiler_generators" information

Internal data, parsed contents of a <parameter> specified in uicompiler.xml

*/

typedef x::ref<parse_parameterObj> parse_parameter;

//! A constant \ref parse_parameter "parameter parser".

//! \see parse_parameter

typedef x::const_ref<parse_parameterObj> const_parse_parameter;

//! A nullable pointer reference to a \ref parse_parameter "parameter parser".

//! \see parse_parameter

typedef x::ptr<parse_parameterObj> parse_parameterptr;

//! A nullable pointer reference to a const \ref parse_parameter "parameter parser".

//! \see parse_parameter

typedef x::const_ptr<parse_parameterObj> const_parse_parameterptr;

#endif
