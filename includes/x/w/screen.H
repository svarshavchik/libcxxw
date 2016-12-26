#ifndef x_w_screen_h
#define x_w_screen_h

#include <x/w/screenfwd.H>
#include <x/w/screenobj.H>
#include <x/w/connectionfwd.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for \ref screen "screen"s.

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
};

LIBCXXW_NAMESPACE_END

#endif
