/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef tablelayoutmanager_impl_h
#define tablelayoutmanager_impl_h

#include "x/w/tablelayoutmanager.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN tablelayoutmanagerObj::implObj
	: public listlayoutmanagerObj::implObj {

 public:

	using listlayoutmanagerObj::implObj::implObj;

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
