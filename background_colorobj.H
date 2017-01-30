/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_colorobj_H
#define background_colorobj_H

#include <x/obj.H>
#include "x/w/picturefwd.H"

LIBCXXW_NAMESPACE_START

class background_colorObj : virtual public obj {

public:
	background_colorObj();
	~background_colorObj();

	virtual const_picture get_current_color()=0;
};

LIBCXXW_NAMESPACE_END
#endif
