/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listitemhandleobj_h
#define x_w_listitemhandleobj_h

#include <x/w/listitemhandlefwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see listitemhandle

class listitemhandleObj: virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Constructor
	listitemhandleObj() LIBCXX_HIDDEN;

	//! Destructor
	~listitemhandleObj() LIBCXX_HIDDEN;

	//! Whether this list item is enabled.
	virtual bool enabled()=0;

	//! Enable or disable this list item.
	virtual void enabled(bool)=0;

	//! Enable or disable this list item.
	virtual void enabled(ONLY IN_THREAD, bool)=0;
};

LIBCXXW_NAMESPACE_END

#endif
