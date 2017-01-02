/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef picture_h
#define picture_h

#include "x/w/picture.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

//! picture internal implementation details.

class LIBCXX_HIDDEN pictureObj::implObj
	: public xid_tObj<xcb_render_picture_t> {

 public:

	implObj(const connection_thread &thread_) LIBCXX_HIDDEN;

	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
