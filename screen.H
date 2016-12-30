#ifndef screen_h
#define screen_h

#include "x/w/screen.H"
#include "x/w/screen_depthinfo.H"
#include "connection.H"
#include <x/logger.H>
#include <x/vector.H>
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

class render;

//! xcb_screen_t owner.

class LIBCXX_HIDDEN screenObj::implObj : virtual public obj {

 public:

	LOG_CLASS_SCOPE;

	const xcb_screen_t * const xcb_screen;
	const ref<connectionObj::implObj::infoObj> info;

	// Same screen_depths as the public object.
	const vector<const_ref<depthObj>> screen_depths;

	// Create the screen_depths. Called from the constructor.

	static vector<const_ref<depthObj>>
		create_screen_depths(const xcb_screen_t *,
				     const render &,
				     size_t);

	implObj(const xcb_screen_t *xcb_screen,
		size_t screen_number,
		const render &render_info,
		const ref<connectionObj::implObj::infoObj> &info);
	~implObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
