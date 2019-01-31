/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_toolboxfactoryobj_h
#define x_w_toolboxfactoryobj_h

#include <x/w/toolboxfactoryfwd.H>
#include <x/w/factoryobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see toolboxfactory

class toolboxfactoryObj: public factoryObj {

public:
	//! Constructor
	toolboxfactoryObj() LIBCXX_HIDDEN;

	//! Destructor
	~toolboxfactoryObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
