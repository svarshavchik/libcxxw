/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uigenerators_h
#define x_w_uigenerators_h

#include <x/w/uigeneratorsfwd.H>
#include <x/w/uigeneratorsobj.H>
#include <x/functionalrefptr.H>
#include <x/vector.H>
#include <x/ref.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Base class for \ref uigenerators "parsed UI generators".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::generators::base

class uigeneratorsBase {

public:

	//! Load template from file.
	static const_uigenerators create(const std::string_view &filename);

	//! Load template from file.

	//! \overload
	static const_uigenerators create(const std::string_view &filename,
					 const const_messages &catalog);

	//! Factory for objects.

	//! Implements custom create().
	template<typename T> class objfactory {

	public:

		//! create() for uigenerators

		//! Always return a const_uigenerators handle.

		static inline const_uigenerators
		create(const std::string_view &filename)
		{
			return uigeneratorsBase::create(filename);
		}

		//! create() for uigenerators

		//! \overload

		static inline const_uigenerators
		create(const std::string_view &filename,
		       const const_messages &catalog)
		{
			return uigeneratorsBase::create(filename, catalog);
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
