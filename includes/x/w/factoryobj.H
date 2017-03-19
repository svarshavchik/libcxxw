/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryobj_h
#define x_w_factoryobj_h

#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/containerobj.H>
#include <x/w/canvasfwd.H>
#include <x/w/metrics/axis.H>
#include <x/w/metrics/mmaxisfwd.H>
#include <x/w/border_infommfwd.H>
#include <x/w/alignment.H>
#include <x/obj.H>
#include <x/functional.H>

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

	//! Create an empty \ref canvas "canvas".
	template<typename functor>
	canvas create_canvas(functor &&creator,
			     const metrics::mmaxis &horiz,
			     const metrics::mmaxis &vert,
			     halign h=halign::center,
			     valign v=valign::middle)
	{
		return do_create_canvas(make_function<void (const canvas &)>
					(creator),
					horiz, vert, h, v);
	}

	//! Create an empty \ref canvas "canvas".

	canvas do_create_canvas(const function<void (const canvas &)> &creator,
				const metrics::mmaxis &horiz,
				const metrics::mmaxis &vert,
				halign h,
				valign v);

};

LIBCXXW_NAMESPACE_END

#endif
