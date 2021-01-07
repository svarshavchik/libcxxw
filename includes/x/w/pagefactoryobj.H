/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagefactoryobj_h
#define x_w_pagefactoryobj_h

#include <x/w/pagefactoryfwd.H>
#include <x/w/factoryobj.H>
#include <x/w/alignment.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pagefactory

class pagefactoryObj : public factoryObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pagefactoryObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~pagefactoryObj() LIBCXX_HIDDEN;

	//! Implement get_container_impl()
	container_impl get_container_impl() override LIBCXX_HIDDEN;

	//! Implement get_element_impl()
	elementObj::implObj &get_element_impl() override LIBCXX_HIDDEN;

	//! Implement last_container_impl()
	container_impl last_container_impl() override LIBCXX_HIDDEN;

	//! Set new element's alignment

	pagefactoryObj &halign(LIBCXXW_NAMESPACE::halign);

	//! Set new element's alignment

	pagefactoryObj &valign(LIBCXXW_NAMESPACE::valign);

	//! Implement created()
	void created(const element &) override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
