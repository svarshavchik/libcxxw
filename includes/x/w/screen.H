/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_h
#define x_w_screen_h

#include <x/w/screenfwd.H>
#include <x/w/screenobj.H>
#include <x/w/connectionfwd.H>
#include <x/ref.H>
#include <x/vector.H>

LIBCXXW_NAMESPACE_START

//! Base class for \ref screen "screen"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::screen::base

class screenBase {

public:

	//! Open the default screen on the default connection.

	static screen create();

	//! Open the specified screen on the default connection.

	static screen create(size_t);

	//! Open the default screen on the given display server

	static screen create(const connection &);

	//! Open the specified screen on the given display server

	static screen create(const connection &, size_t screen);

	//! Typedef alias

	typedef const_ref<screenObj::depthObj> depth_t;

	//! Typedef alias

	typedef const_ref<screenObj::visualObj> visual_t;

	//! The custom create() for screen refs.

	template<typename T> class objfactory {

	public:
		//! The custom create() method

		template<typename ...Args>
		static inline T create(Args &&...args)
		{
			return screenBase::create(std::forward<Args>(args)...);
		}
	};
};

#include <x/w/deps/screen_connection.H>

LIBCXXW_NAMESPACE_END

#endif
