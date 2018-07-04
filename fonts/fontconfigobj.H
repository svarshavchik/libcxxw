/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontconfigobj_H
#define fontconfigobj_H

#include <x/obj.H>

#include "fonts/fontobjectsetfwd.H"
#include "fonts/fontpatternfwd.H"
#include "fonts/fontlistfwd.H"
#include "x/w/impl/fonts/fontcharsetfwd.H"
#include "x/w/fontfwd.H"

#include <set>
#include <string>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! Font configuration

//! A singleton object whose constructor loads and initializes the fontconfig
//! library, and whose destructor deinitializes it.

class LIBCXX_HIDDEN configObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	configObj(const ref<implObj> &implArg);

	//! Destructor
	~configObj();

	//! Create an internal object set

	objectset create_objectset(const std::set<std::string> &);

	//! Create an empty pattern object
	pattern create_pattern();

	//! Create a pattern object from \ref font "font".

	pattern create_pattern(const font &props,
			       double dpi);

	//! List available fonts
	list create_list();

	//! List available fonts
	list create_list(const pattern &patternArg);

	//! List available fonts
	list create_list(const pattern &patternArg,
			 const objectset &propertiesArg);

	//! Create an internal character set object.
	charset create_charset();
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
