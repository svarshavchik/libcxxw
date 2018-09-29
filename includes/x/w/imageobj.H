/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_imageobj_h
#define x_w_imageobj_h

#include <x/w/imagefwd.H>
#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

//! A loaded image

//! \see image

class imageObj : public elementObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details.

	//! Constructor
	imageObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~imageObj() LIBCXX_HIDDEN;

};

LIBCXXW_NAMESPACE_END

#endif
