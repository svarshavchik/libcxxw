/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontcharset_impl_H
#define fontcharset_impl_H

#include "fonts/fontconfigfwd.H"
#include "fonts/fontcharset.H"
#include <x/mpobj.H>
#include <fontconfig/fontconfig.h>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! charset implementation object.

class charsetObj::implObj : virtual public obj {

public:
	//! Normal constructor. Creates a new FcCharset.

	implObj(const config &configArg);

	//! Constructor for a given FcCharSet

	implObj(const config &configArg, FcCharSet *charsetArg,
		bool autodestroyArg);

	//! Destructor
	~implObj();

	//! The fontconfig library.
	const config c;

	//! A mutex-protected charset.

	typedef mpobj<FcCharSet * const, std::recursive_mutex> charset_t;

	//! The underlying charset.

	charset_t charset;

	//! Whether we own the FcCharset in question.

	//! This object can be constructed for a charset that's owned by
	//! another object, like a pattern, in which case we are not
	//! responsible for destroying it.
	const bool autodestroy;
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
