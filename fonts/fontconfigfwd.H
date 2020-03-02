/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontconfigfwd_H
#define fontconfigfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

class LIBCXX_HIDDEN configObj;
class configBase;

//! Font configuration

//! A wrapper for the fontconfig library. \ref config "config" behaves like
//! any other reference-counted object, but it's really a singleton. Each
//! create() returns the same object. This is a singleton that loads and
//! initializes the fontconfig library.

typedef ref<configObj, configBase> config;

//! A nullable pointer reference to the \ref config "font configuration".

typedef ptr<configObj, configBase> configptr;

//! A constant \ref config "font configuration".

typedef const_ref<configObj, configBase> const_config;

//! A nullable pointer reference to a constant \ref config "font configuration".

typedef const_ptr<configObj, configBase> const_configptr;

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
