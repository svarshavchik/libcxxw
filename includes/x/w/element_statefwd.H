/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_statefwd_H
#define x_w_element_statefwd_H

#include <x/w/namespace.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/busy.H>
#include <iostream>

LIBCXXW_NAMESPACE_START

struct element_state;

std::ostream &operator<<(std::ostream &o, const element_state &);

//! Callback type for updates to the \ref element_state "element_state".

typedef void element_state_callback_t(THREAD_CALLBACK,
				      const element_state &, const busy &);

LIBCXXW_NAMESPACE_END

#endif
