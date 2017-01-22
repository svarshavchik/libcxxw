/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryobj_h
#define x_w_factoryobj_h

#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/containerobj.H>
#include <x/w/metrics/axis.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A factory of display elements.

//! \see factory

class factoryObj : virtual public obj {

public:

	//! This factory is for the given container.

	const ref<containerObj::implObj> container_impl;

private:
	//! Invoked by factory's methods, after creating a new element.

	virtual void created(const element &)=0;
public:
	//! Constructor
	factoryObj(const ref<containerObj::implObj> &container_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~factoryObj() LIBCXX_HIDDEN;

	//! Create empty filler
	element create_empty_element(const metrics::axis &horiz_axis=metrics::axis(),
				     const metrics::axis &vert_axis=metrics::axis());
};

LIBCXXW_NAMESPACE_END

#endif
