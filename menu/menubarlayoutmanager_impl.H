/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menubarlayoutmanager_impl_h
#define menubarlayoutmanager_impl_h

#include "x/w/menubarlayoutmanager.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN menubarlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

 public:
	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
