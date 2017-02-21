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
//! \par Initializer lambdas
//!
//! The factory provides various methods to create different kinds of
//! display elements. Each method returns the newly-created display element,
//! and most methods' first argument is a "initializer lambda".
//!
//! The initializer lambda gets invoked just before the factory method
//! finishes
//! creating a new element, and the parameter to the lambda is the newly-
//! created element. The lambda's job is to finalize any needed
//! initialization of the new display element. This results in optimal
//! performance. The new display element's container integrates the new
//! element after the creator returns and before the factory method itself
//! returns. The container generally ignores the new display element until
//! its creator returns.
//!
//! If the lambda does not initialize the contents of the new display
//! element (where appropriate), and that occurs after the factory method
//! returns, the new display element's container will often need to make
//! expensive recalculations if the new display element's size changes
//! (likely). By having the lambda handle the initialization, these calculation
//! occur, for the first time, after the creator returns with a
//! fully-initialized display element,
//!
//! \code
//! #include <INSERT_LIBX_NAMESPACE/w/canvas.H>
//! #include <INSERT_LIBX_NAMESPACE/w/metrics/mmaxis.H>
//!
//! INSERT_LIBX_NAMESPACE::w::metrics::mmaxis width{4}, height{4};
//!
//! INSERT_LIBX_NAMESPACE::w::canvas new_canvas=
//!    factory->create_canvas([]
//!                           (const auto &new_canvas)
//!                           {
//!                                 // Initializer lambda
//!                           }, width, height);
//! \endcode
//!
//! create_canvas() creates an empty display element that does nothing but
//! take up the alloted space. This is typically used to fill in the unused
//! elements in a \ref gridlayoutmanager "grid".
//!
//! Besides its initializer lambda (which doesn't reall have much to
//! initializer), create_canvas()'s remaining two parameters specify the
//! \ref metrics::mmaxis "size" of the new display element.

typedef ref<factoryObj> factory;

//! A constant generic window handler

//! \see factory

typedef const_ref<factoryObj> const_factory;

LIBCXXW_NAMESPACE_END

#endif
