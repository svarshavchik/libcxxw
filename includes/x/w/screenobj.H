#ifndef x_w_screenobj_h
#define x_w_screenobj_h

#include <x/w/screenfwd.H>
#include <x/w/connectionfwd.H>
#include <x/w/rectangle.H>
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


	//! Screen metrics
	dim_t width_in_pixels() const;
	//! Screen metrics
	dim_t height_in_pixels() const;
	//! Screen metrics
	dim_t width_in_millimeters() const;
	//! Screen metrics
	dim_t height_in_millimeters() const;

	//! Return the screen's work area.

	//! The screen's window manager might reserve some border space for
	//! its panels. This returns the actual area of the screen for
	//! applications' windows.
	//!
	//! Returns true if this information is available, false if the
	//! screen's window manager does not provide that information, in
	//! which case x & y are 0, and width and height are the same values
	//! returned by width_in_pixels() and height_in_pixels().

	rectangle get_workarea() const;

	//! Get the default size of a top level window's borders, according to the window manager.

	//! Returns true if this information is provided by the window manager.
	//! Returns false if unavailable, and the parameters have not been
	//! updated. The values are in pixels.

	bool get_frame_extents(dim_t &left,
			       dim_t &right,
			       dim_t &top,
			       dim_t &bottom) const;
};

LIBCXXW_NAMESPACE_END

#endif
