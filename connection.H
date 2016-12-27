#ifndef connection_h
#define connection_h

#include <vector>

#include "render.H"
#include "x/w/connection.H"
#include "x/w/screenobj.H"

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/render.h>

LIBCXXW_NAMESPACE_START

//! xcb_connection_t owner.

class LIBCXX_HIDDEN connectionObj::implObj : virtual public obj {

 public:

	class threadObj;
	class infoObj;

	//! The actual connection handle
	const ref<infoObj> info;

	//! The connection's thread.
	const ref<threadObj> thread;

	//! Server configuration

	const struct xcb_setup_t setup;

	//! Server configuration - screens

	const std::vector<ref<screenObj::implObj>> screens;

	//! Server configuration - RENDER extension.

	const render render_info;

	//! Constructor, connect to the given display.

	implObj(const std::experimental::string_view &display);

 private:

	//! Intenral constructor

	implObj(const ref<infoObj> &info);

	//! Internal constructor

	implObj(const ref<infoObj> &info,
		const xcb_setup_t *setup,
		const ref<threadObj> &threadArg);

 public:

	//! Wrap everything up, call xcb_disconnect()
	~implObj() noexcept;

	//! Return a brief identifier for a protocol error.

	static std::string get_error(const xcb_generic_error_t *e);

};

LIBCXXW_NAMESPACE_END

#endif
