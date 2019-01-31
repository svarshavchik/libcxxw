/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_depthinfo_h
#define screen_depthinfo_h

#include "screen.H"
#include "x/w/screen_depthinfo.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN screenObj::visualObj::implObj : virtual public obj {

public:
	const xcb_visualid_t visual_id;

	implObj(xcb_visualid_t visual_id);
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
