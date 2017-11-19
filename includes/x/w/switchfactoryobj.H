/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_switchfactoryobj_h
#define x_w_switchfactoryobj_h

#include <x/w/switchfactoryfwd.H>
#include <x/w/factoryobj.H>
#include <x/w/alignment.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see switchfactory

class switchfactoryObj : public factoryObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	switchfactoryObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~switchfactoryObj() LIBCXX_HIDDEN;

	//! Implement get_container_impl()
	ref<containerObj::implObj> get_container_impl() override LIBCXX_HIDDEN;

	//! Implement get_element_impl()
	elementObj::implObj &get_element_impl() override LIBCXX_HIDDEN;

	//! Implement last_container_impl()
	ref<containerObj::implObj> last_container_impl() override LIBCXX_HIDDEN;

	//! Set new element's alignment

	switchfactoryObj &halign(LIBCXXW_NAMESPACE::halign);

	//! Set new element's alignment

	switchfactoryObj &valign(LIBCXXW_NAMESPACE::valign);

	//! Implement created()
	void created(const element &) override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
