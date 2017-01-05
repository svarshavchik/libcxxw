/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "picture.H"
#include "pictformat.H"

LIBCXXW_NAMESPACE_START

pictureObj::pictureObj(const ref<implObj> &impl)
	: impl(impl)
{
}

pictureObj::~pictureObj()=default;

/////////////////////////////////////////////////////////////////////////////

pictureObj::implObj::implObj(const connection_thread &thread)
	: xid_t<xcb_render_picture_t>(thread)
{
}

pictureObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
