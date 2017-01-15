/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryobj_h
#define x_w_factoryobj_h

#include <x/w/factoryfwd.H>
#include <x/w/containerobj.H>

LIBCXXW_NAMESPACE_START

//! A factory of display elements.

//! \see factory

class factoryObj : virtual public obj {

public:

	//! This factory is for the given container.

	const ref<containerObj::implObj> container_impl;

	//! Invoked by factory's methods, after creating a new element.

	virtual void created(const ref<child_elementObj> &)=0;

	//! Constructor
	factoryObj(const ref<containerObj::implObj> &container_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~factoryObj() LIBCXX_HIDDEN;

};

LIBCXXW_NAMESPACE_END

#endif
