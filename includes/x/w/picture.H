/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_picture_h
#define x_w_picture_h

#include <x/w/picturefwd.H>
#include <x/w/pictureobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class of \ref picture "pictures".

class pictureBase : public ptrrefBase {

public:

	//! typedef alias

	typedef pictureObj::clip_mask clip_mask;

	//! typedef alias

	typedef pictureObj::fixedprec fixedprec;

	//! typedef alias

	typedef pictureObj::point point;

	//! typedef alias

	typedef pictureObj::triangle triangle;

};

LIBCXXW_NAMESPACE_END

#endif
