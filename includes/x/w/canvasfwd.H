/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_canvasfwd_h
#define x_w_canvasfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class canvasObj;

//! A mostly empty display element.

//! \code
//! INSERT_LIBX_NAMESPACE::w::metrics::axis h_metrics{0, 50, 100};
//! INSERT_LIBX_NAMESPACE::w::metrics::axis v_metrics{0, 50, 100};
//!
//! factory->create_canvas([]
//!                        (const canvas &c)
//!                        {
//!                            // ...
//!                        },
//!                        h_metrics, v_metrics);
//! \endcode
//!
//! The canvas object gets constructed, with specified
//! horizontal and vertical metrics.

typedef ref<canvasObj> canvas;

//! A nullable pointer reference to an canvas.

//! \see canvas

typedef ptr<canvasObj> canvasptr;

//! A reference to a constant canvas object.

//! \see canvas

typedef const_ref<canvasObj> const_canvas;

//! A nullable pointer reference to a constant canvas object.

//! \see canvas

typedef const_ptr<canvasObj> const_canvasptr;

LIBCXXW_NAMESPACE_END

#endif
