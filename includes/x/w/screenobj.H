#ifndef x_w_screenobj_h
#define x_w_screenobj_h

#include <x/w/screenfwd.H>
#include <x/w/connectionfwd.H>
#include <x/obj.H>
#include <x/vectorfwd.H>

LIBCXXW_NAMESPACE_START

//! A screen on the display server.

//! \see screen

class screenObj : virtual public obj {

public:
	class implObj;
	class depthObj;
	class visualObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Information about available depths on this screen.

	//! Note that this is an \c INSERT_LIBX_NAMESPACE::vector.
	//!
	//! Refer to this class as INSERT_LIBX_NAMESPACE::vector<INSERT_LIBX_NAMESPACE::w::screen::base::depth_t>

	const vector<const_ref<depthObj>> screen_depths;

private:
	//! Screen's connection
	const connection connref;

public:
	//! Constructor
	screenObj(const ref<implObj> &impl,
		  const connection &conn) LIBCXX_HIDDEN;

	//! Destructor
	~screenObj() noexcept LIBCXX_HIDDEN;

	//! The screen's connection
	connection conn();

	//! The screen's connection
	const_connection conn() const;

	//! The connection mcguffin

	//! \see connection for more information, this is forwarded to
	//! \c conn()->mcguffin().

	ref<obj> mcguffin() const;
};

LIBCXXW_NAMESPACE_END

#endif
