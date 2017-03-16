/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_colorobj_H
#define background_colorobj_H

#include <x/obj.H>
#include "x/w/picturefwd.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

class background_colorObj : virtual public obj {

public:
	background_colorObj();
	~background_colorObj();

	virtual const_picture get_current_color(IN_THREAD_ONLY)=0;
	virtual void theme_updated(IN_THREAD_ONLY)=0;
};

LIBCXXW_NAMESPACE_END
#endif
