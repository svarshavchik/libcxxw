/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef singletonlayoutmanagerobj_h
#define singletonlayoutmanagerobj_h

#include "singletonlayoutmanagerfwd.H"
#include "x/w/layoutmanagerobj.H"
#include "x/w/factoryfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see singletonlayoutmanager

class LIBCXX_HIDDEN singletonlayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	singletonlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~singletonlayoutmanagerObj() LIBCXX_HIDDEN;


	//! Return a factory for replacing the singleton element.

	factory replace();
};

LIBCXXW_NAMESPACE_END

#endif
