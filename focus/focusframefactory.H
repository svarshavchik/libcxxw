/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframefactory_h
#define focusframefactory_h

#include "x/w/factory.H"
#include "x/w/container.H"
#include "x/w/elementfwd.H"
#include "x/w/gridlayoutmanagerfwd.H"

LIBCXXW_NAMESPACE_START

//! Factory for the contents of a \ref focusframecontainer "focus frame container".

//! This factory gets returned by get_focusable(). The focusframecontainer
//! uses the grid layout manager internally. When a new display element
//! gets created
//! by this factory, all contents are remove()d from the underlying grid, then
//! the new element is inserted into the grid.

class LIBCXX_HIDDEN focusframefactoryObj : public factoryObj {

 public:

	const gridlayoutmanager glm;

	focusframefactoryObj(const container &ffc);

	void created(const element &e) override;

	~focusframefactoryObj();

	//! Return the container for a new element.

	virtual ref<containerObj::implObj> get_container_impl();

	//! Return the container's element.

	virtual elementObj::implObj &get_element_impl();

};

typedef ref<focusframefactoryObj> focusframefactory;

LIBCXXW_NAMESPACE_END

#endif
