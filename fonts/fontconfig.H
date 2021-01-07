/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_fontconfig_H
#define x_w_fontconfig_H

#include <x/ref.H>
#include "fonts/fontconfigfwd.H"
#include "fonts/fontconfigobj.H"

LIBCXXW_NAMESPACE_START

//! Font configuration

namespace fontconfig {
#if 0
}
#endif

//! Base class for the \ref config "font configuration" singleton.

class configBase {

public:

	//! Return the singleton instance.

	static config create() LIBCXX_PUBLIC;

	//! Factor for the create() method.

	template<typename ref_type> class objfactory {
	public:

		//! create() returns a singleton.
		static inline ref_type create()
		{
			return configBase::create();
		}
	};
};

//! Create a config object

inline config create()
{
	return config::create();
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
