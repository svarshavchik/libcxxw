/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef capturefactoryobj_h
#define capturefactoryobj_h

#include "x/w/factoryfwd.H"
#include "x/w/elementfwd.H"
#include "x/w/factoryobj.H"
#include "x/w/containerobj.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! A \ref capturefactory "factory that captures the new display element".

class LIBCXX_HIDDEN capturefactoryObj : public factoryObj {

	//! Container for the capture element.

	typedef mpobj<elementptr> capture_element_t;

	//! The capture element.
	capture_element_t capture_element;

	const container_impl factory_container_impl;
 public:

	//! Constructor
	capturefactoryObj(const container_impl &factory_container_impl);

	//! Destructor
	~capturefactoryObj();

	//! Implement get_container_impl()

	container_impl get_container_impl() override;

	//! Return get_element_impl()

	elementObj::implObj &get_element_impl() override;

	//! An element was created.
	void created(const element &) override;

	//! Retrieve the capture element.
	element get();
};

LIBCXXW_NAMESPACE_END

#endif
