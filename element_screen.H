#ifndef element_screen_h
#define element_screen_h

#include "element.H"
#include "generic_window_handler.H"
#include "x/w/screen.H"

LIBCXXW_NAMESPACE_START

inline screen elementObj::implObj::get_screen()
{
	return get_window_handler().screenref;
}

inline const_screen elementObj::implObj::get_screen() const
{
	return get_window_handler().screenref;
}

LIBCXXW_NAMESPACE_END

#endif
