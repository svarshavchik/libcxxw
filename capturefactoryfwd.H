/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef capturefactoryfwd_h
#define capturefactoryfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN capturefactoryObj;

//! A factory for creating a single element.

//! A factory that saves the created element internally. It is typically
//! passed to a callback that takes a factory as a parameter, and the callback
//! is expected to use the factory to create a single display element.
//!
//! get() retrieves the new element, after the callback returns.
//!
//! An exception gets thrown if an attempt is made to create another element.
//!
//! get() throws an exception if no element was created.

typedef ref<capturefactoryObj> capturefactory;

LIBCXXW_NAMESPACE_END

#endif
