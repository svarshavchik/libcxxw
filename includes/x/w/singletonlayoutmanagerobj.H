/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_singletonlayoutmanagerobj_h
#define x_w_singletonlayoutmanagerobj_h

#include <x/w/singletonlayoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/elementfwd.H>
#include <x/w/factoryfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see singletonlayoutmanager

class singletonlayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	singletonlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~singletonlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Return the current element
	elementptr get() const;

	//! Return a factory for replacing the singleton element.

	factory replace();
};

LIBCXXW_NAMESPACE_END

#endif