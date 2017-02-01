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

//! An element factory

//! A factory object is created by a \ref layoutmanager "layout manager".
//!
//! Depending on the layout manager, the same factory object can be used
//! to repetitively create multiple display elements; or a factory object
//! can be used for creating no more than one element, with unspecified
//! results if the factory object gets reused.
//!
//! The factory provides various methods to create different kinds of
//! display elements. Each method returns the newly-created display element,
//! and most methods' first argument is a "creator lambda".
//!
//! The creator lambda gets invoked before the factory method returns a
//! newly-created element, and the parameter to the lambda is the newly-
//! created element. The creator lambda's job is to finalize any needed
//! initialization of the new display element. This results in optimal
//! performance. The new display element's container integrates the new
//! element after the creator returns and before the factory method itself
//! returns. The container generally ignores the new display element until
//! its creator returns.
//!
//! If the creator does not initialize the contents of the new display
//! element (where appropriate), and that occurs after the factory method
//! returns, the new display element's container will often need to make
//! expensive recalculations if the new display element's size changes
//! (likely). By having the creator handle the initialization, these calculation
//! occur, for the first time, after the creator returns with a
//! fully-initialized display element,

typedef ref<factoryObj> factory;

//! A constant generic window handler

//! \see factory

typedef const_ref<factoryObj> const_factory;

LIBCXXW_NAMESPACE_END

#endif
