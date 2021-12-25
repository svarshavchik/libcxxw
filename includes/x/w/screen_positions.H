/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_positions_h
#define x_w_screen_positions_h

#include <x/w/screen_positionsfwd.H>
#include <x/w/screen_positionsobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for \ref screen_positions "screen_positions".

//! Implements custom create() functions that cache screen_positions objects
//! by configuration filename. Instantiating a default screen_positions
//! object ends up returning the same actual object.
class screen_positionsBase {

public:

	//! Instantiate the default screen_positions object.

	static screen_positions create();

	//! Initialize a custom screen_positions object.
	static screen_positions create(const std::string &filename,
				       const std::string &version);

	//! Object creation factory.

	template<typename ptrrefType> class objfactory {
	public:

		//! Forward create() to the internal implementation

		static inline ptrrefType create()
		{
			return screen_positionsBase::create();
		}

		//! Forward create() to the internal implementation

		static inline ptrrefType create(const std::string &filename,
						const std::string &version)
		{
			return screen_positionsBase::create(filename,
							    version);
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
