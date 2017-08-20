/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef freetypefont_H
#define freetypefont_H

#include <x/ref.H>
#include "fonts/freetypefontfwd.H"
#include "fonts/freetypefontobj.H"
#include <x/namespace.h>

LIBCXXW_NAMESPACE_START

//! Base class for \ref freetypefont "Freetype font"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::freetypefont::base

class freetypefontBase : public ptrref_base {
public:

};

LIBCXXW_NAMESPACE_END

#endif
