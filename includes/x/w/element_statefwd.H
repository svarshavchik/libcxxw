/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_statefwd_H
#define x_w_element_statefwd_H

#include <x/w/namespace.H>
#include <x/w/busy.H>
#include <x/functionalrefptrfwd.H>
#include <iostream>

LIBCXXW_NAMESPACE_START

struct element_state;

std::ostream &operator<<(std::ostream &o, const element_state &);

//! Callback type for updates to the \ref element_state "element_state".

typedef functionref<void (const element_state &,
			  const busy &)> element_state_update_handler_t;

LIBCXXW_NAMESPACE_END

#endif
