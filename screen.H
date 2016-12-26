#ifndef screen_h
#define screen_h

#include "x/w/screen.H"
#include "connection.H"
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

//! xcb_screen_t owner.

class LIBCXX_HIDDEN screenObj::implObj : virtual public obj {

 public:

	const xcb_screen_t * const xcb_screen;
	const ref<connectionObj::implObj::infoObj> info;

	implObj(const xcb_screen_t *xcb_screen,
		const ref<connectionObj::implObj::infoObj> &info);
	~implObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
