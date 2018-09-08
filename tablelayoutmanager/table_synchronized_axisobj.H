/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_synchronized_axisobj_h
#define x_w_table_synchronized_axisobj_h

#include "synchronized_axis_impl.H"
#include "tablelayoutmanager/table_synchronized_axisfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see table_synchronized_axis

class LIBCXX_HIDDEN tablelayoutmanagerObj::table_synchronized_axisObj
	: public synchronized_axisObj::implObj {

public:
	//! Constructor
	table_synchronized_axisObj();

	//! Destructor
	~table_synchronized_axisObj();
};

LIBCXXW_NAMESPACE_END

#endif
