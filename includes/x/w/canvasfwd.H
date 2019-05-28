/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_canvasfwd_h
#define x_w_canvasfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC canvasObj;
struct canvas_config;

//! A mostly empty widget.

//! \code
//! canvas_config config;
//!
//! config.width={0, 50, 100};
//! config.height={0, 50, 100};
//!
//! factory->create_canvas(config);
//! \endcode
//!
//! The canvas object gets constructed, with specified
//! horizontal and vertical metrics.
//!
//! \code
//! c->update(h_metrics, v_metrics);
//! \endcode
//!
//! Update the canvas object's horizontal and vertical metrics.
//!
//! \see canvas_config

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
