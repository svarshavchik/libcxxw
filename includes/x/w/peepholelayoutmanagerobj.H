/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholelayoutmanagerobj_h
#define x_w_peepholelayoutmanagerobj_h

#include <x/w/peepholelayoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see peepholelayoutmanager

class peepholelayoutmanagerObj: public layoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	peepholelayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~peepholelayoutmanagerObj() LIBCXX_HIDDEN;

	//! Return the peepholed widget
	element get() const;
};

LIBCXXW_NAMESPACE_END

#endif
