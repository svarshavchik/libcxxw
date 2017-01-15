/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_factoryfwd_h
#define x_w_factoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class factoryObj;
class child_elementObj;

//! An element factory

//! A factory object is created by a \ref layoutmanager "layout manager".
//!
//! Depending on the layout manager, the same factory object can be used
//! to repetitively create multiple display elements; or a factory object
//! can be used for creating no more than one element, with unspecified
//! results if the factory object gets reused.

typedef ref<factoryObj> factory;

//! A constant generic window handler

//! \see factory

typedef const_ref<factoryObj> const_factory;

LIBCXXW_NAMESPACE_END

#endif
