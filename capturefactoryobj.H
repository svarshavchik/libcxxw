/*
** Copyright 2017 Double Precision, Inc.
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

 public:

	//! Constructor
	capturefactoryObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~capturefactoryObj();

	//! An element was created.
	void created(const element &) override;

	//! Retrieve the capture element.
	element get();
};

LIBCXXW_NAMESPACE_END

#endif
