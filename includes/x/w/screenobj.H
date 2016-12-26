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
};

LIBCXXW_NAMESPACE_END

#endif
