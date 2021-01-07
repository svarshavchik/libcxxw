/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_batch_queuefwd_h
#define x_w_batch_queuefwd_h

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class batch_queueObj;

//! Batches execution batch_queue for the connection thread.

//! \internal
//!
//! This object is returned by a connection thread's get_batch_queue() method.
//! The connection thread object holds a weak pointer on this object,
//! and get_batch_queue returns the weakly-held object, if there is one, or
//! creates a new task object.
//!
//! A job to be performed by the connection thread gets added to this object
//! When this object goes out of scope and gets destroyed, it sends a
//! run_as message to the connection thread, which will execute all the
//! collected batch_queue, as a single batch.
//!
//! This is used to batch up changes to the UI, for optimization purposes.
//! The jobs will be executed in the order received.
//!
//! The batch queue is used for:
//!
//! - batching request_visibility() calls (show/hide).
//!
//! - held by the layoutmanager object, for the duration of its existence.
//!
//! - internally held during create_mainwindow().

typedef ref<batch_queueObj> batch_queue;

//! A nullable pointer reference to the batch_queue object.

//! \see batch_queue.

typedef ptr<batch_queueObj> batch_queueptr;

LIBCXXW_NAMESPACE_END

#endif
