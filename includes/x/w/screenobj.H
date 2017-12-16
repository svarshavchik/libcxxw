/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screenobj_h
#define x_w_screenobj_h

#include <x/w/screenfwd.H>
#include <x/w/connectionfwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/pictformatfwd.H>
#include <x/w/rectangle.H>
#include <x/w/picturefwd.H>
#include <x/w/pictformatfwd.H>
#include <x/w/scratch_bufferfwd.H>
#include <x/w/main_windowfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/obj.H>
#include <x/vectorfwd.H>
#include <x/functional.H>

#include <unordered_set>
#include <string>
#include <string_view>
#include <utility>

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

	//! Screen's connection
	const connection connref;

	//! Constructor
	screenObj(const ref<implObj> &impl,
		  const connection &conn) LIBCXX_HIDDEN;

	//! Destructor
	~screenObj() LIBCXX_HIDDEN;

	//! The screen's connection
	connection get_connection() LIBCXX_INLINE;

	//! The screen's connection
	const_connection get_connection() const LIBCXX_INLINE;

	//! The connection mcguffin

	//! \see connection for more information, this is forwarded to
	//! \c conn()->mcguffin().

	ref<obj> connection_mcguffin() const;

	//! Screen metrics
	dim_t width_in_pixels() const;
	//! Screen metrics
	dim_t height_in_pixels() const;
	//! Screen metrics
	dim_t width_in_millimeters() const;
	//! Screen metrics
	dim_t height_in_millimeters() const;

	//! Supported extended window manager protocols
	const std::unordered_set<std::string> &supported() const;

	//! Whether the given protocol is supported
	bool supported(const std::string_view &) const;

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

	//! Create a solid color fill picture object.

	const_picture create_solid_color_picture(const rgb &color) const;

	//! Create a linear gradient picture object.

	const_picture create_linear_gradient_picture(const rgb_gradient &g,
						     coord_t x1,
						     coord_t y1,
						     coord_t x2,
						     coord_t y2);
	//! Create a new pixmap on this screen.

	//! Mostly for internal use.

	pixmap create_pixmap(const const_pictformat &pf,
			     dim_t width,
			     dim_t height);

	//! Creator for create_mainwindow()

	typedef void main_window_creator_t(const main_window &);

	//! SFINAE selector for a creator functor.

	template<typename functor>
	using is_main_window_creator=
		std::void_t<decltype(std::declval<functor &&>()
				     (std::declval<const main_window &>
				     ()))>;

	//! Create a new top level window, using the default layout manager

	template<typename functor,
		 typename =is_main_window_creator<functor>>
	main_window create_mainwindow(functor &&f)
	{
		return do_create_mainwindow
			(make_function<main_window_creator_t>
			 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<typename functor,
		 typename =is_main_window_creator<functor>>
	main_window create_mainwindow(functor &&f,
				      const new_layoutmanager &layout_factory)
	{
		return do_create_mainwindow
			(make_function<main_window_creator_t>
			 (std::forward<functor>(f)),
			 layout_factory);
	}

	//! Create a new top level window, using the default layout manager

	template<typename functor,
		 typename =is_main_window_creator<functor>>
	main_window create_mainwindow(const screen_positions_t &pos,
				      const std::string_view &name,
				      functor &&f)
	{
		return do_create_mainwindow
			(pos, name, make_function<main_window_creator_t>
			 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<typename functor,
		 typename =is_main_window_creator<functor>>
	main_window create_mainwindow(const screen_positions_t &pos,
				      const std::string_view &name,
				      functor &&f,
				      const new_layoutmanager &layout_factory)
	{
		return do_create_mainwindow
			(pos, name, make_function<main_window_creator_t>
			 (std::forward<functor>(f)),
			 layout_factory);
	}

	//! create_mainwindow() implementation.

	main_window do_create_mainwindow(const function<main_window_creator_t>
					 &f);

	//! create_mainwindow() implementation.

	main_window do_create_mainwindow(const function<main_window_creator_t>
					 &f,
					 const new_layoutmanager &factory)
		LIBCXX_PUBLIC;

	//! create_mainwindow() implementation.

	main_window do_create_mainwindow(const screen_positions_t &pos,
					 const std::string_view &name,
					 const function<main_window_creator_t>
					 &f);

	//! create_mainwindow() implementation.

	main_window do_create_mainwindow(const screen_positions_t &pos,
					 const std::string_view &name,
					 const function<main_window_creator_t>
					 &f,
					 const new_layoutmanager &factory)
		LIBCXX_PUBLIC;

	//! create_mainwindow() implementation.
	main_window do_create_mainwindow(const std::optional<screen_position>
					 &pos,
					 const function<main_window_creator_t>
					 &f,
					 const new_layoutmanager &factory)
		LIBCXX_HIDDEN;

	//! Shortcut to render data.

	const_pictformat find_alpha_pictformat_by_depth(depth_t d) const;

	////////////////////////////////////////////////////////////////////

	//! Internal method.

	//! \internal

	scratch_buffer create_scratch_buffer(const std::string &identifier,
					     const const_pictformat &pf,
					     dim_t initial_width,
					     dim_t initial_height)
		LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
