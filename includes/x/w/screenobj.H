#ifndef x_w_screenobj_h
#define x_w_screenobj_h

#include <x/w/screenfwd.H>
#include <x/w/connectionfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A screen on the display server.

//! \see screen

class screenObj : virtual public obj {

 public:
	class implObj;

	//! Internal implementation details

	const ref<implObj> impl;

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
	inline connection conn() { return connref; }

	//! The screen's connection
	inline const_connection conn() const { return connref; }

	//! The connection mcguffin

	//! \see connection for more information, this is forwarded to
	//! \c conn()->mcguffin().

	ref<obj> mcguffin() const;
};

LIBCXXW_NAMESPACE_END

#endif
