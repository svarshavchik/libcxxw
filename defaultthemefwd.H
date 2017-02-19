/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef defaultthemefwd_h
#define defaultthemefwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"
#include <string>

LIBCXXW_NAMESPACE_START

//! Location of the directory with all the themes

std::string themedirroot();

class defaultthemeObj;
class defaultthemeBase;

// A loaded application theme.

typedef ref<defaultthemeObj, defaultthemeBase> defaulttheme;

LIBCXXW_NAMESPACE_END

#endif
