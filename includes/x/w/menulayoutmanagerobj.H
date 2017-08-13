/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menulayoutmanagerobj_h
#define x_w_menulayoutmanagerobj_h

#include <x/w/menulayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menulayoutmanager

class menulayoutmanagerObj : public listlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	menulayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~menulayoutmanagerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
