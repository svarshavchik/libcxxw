/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontconfig_impl_H
#define fontconfig_impl_H

#include <x/mpobj.H>
#include "fonts/fontconfig.H"
#include <fontconfig/fontconfig.h>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! The mutex-protected handle to the fontconfig library.

typedef mpobj<FcConfig * const, std::recursive_mutex> fontconfig_t;

class LIBCXX_HIDDEN configObj::implObj : virtual public obj {

public:
	//! Constructor
	implObj();

	//! Destructor
	~implObj();

	//! fontconfig library handle.

	fontconfig_t fc;
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
