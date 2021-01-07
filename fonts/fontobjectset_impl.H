/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontobjectset_impl_H
#define fontobjectset_impl_H

#include "fonts/fontconfigfwd.H"
#include "fonts/fontobjectset.H"
#include <x/mpobj.H>
#include <fontconfig/fontconfig.h>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! \ref objsectset "objsectset" implementation object.

class LIBCXX_HIDDEN objectsetObj::implObj : virtual public obj {

public:
	//! Constructor
	implObj(const config &configArg);

	//! Destructor
	~implObj();

	//! The fontconfig library.
	const config c;

	//! Thread-safe container for the FcObjectSet wrapper

	typedef mpobj<FcObjectSet * const> set_t;

	//! The object set

	set_t set;
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
