/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_colorobj_h
#define background_colorobj_h

#include "connection_threadfwd.H"
#include <x/w/picturefwd.H>
#include <functional>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! An object whose mission in life is to return a background color

class LIBCXX_HIDDEN background_colorObj : virtual public obj {

 public:

	typedef std::function<const_picture ()> background_color_closure_t;

 private:
	background_color_closure_t current_background_color_thread_only;

 public:

	background_colorObj(const background_color_closure_t &initial_closure);
	~background_colorObj() noexcept;

	THREAD_DATA_ONLY(current_background_color);
};

LIBCXXW_NAMESPACE_END

#endif
