/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontsortedlistobj_H
#define fontsortedlistobj_H

#include <x/ref.H>
#include <x/obj.H>

#include "fonts/fontlistobj.H"
#include "fonts/fontsortedlistfwd.H"

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! A sorted list of fonts that best matches a pattern.

class sortedlistObj : public listObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	sortedlistObj(const ref<implObj> &implArg);

	//! Destructor
	~sortedlistObj();
};


#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
