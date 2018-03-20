/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_handlerfwd_h
#define peepholed_toplevel_listcontainer_handlerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class peepholed_toplevel_listcontainer_handlerObj;
struct peepholed_toplevel_listcontainer_handler_args;

//! Subclass of the handler used by the \ref create_peepholed_toplevel_listcontainer_popup "peepholed list container popup".

//! Stores the background colors used for the list's background when the
//! list popup is above and below, or to the left or the right, of the
//! attached_to element.
//!
//! Overrides recalculate_popup_position, and captures the newly-calculated
//! popup_position_affinity, that determines which background color to set
//! for the list popup's background, which gets updated accordingly.

typedef ref<peepholed_toplevel_listcontainer_handlerObj> peepholed_toplevel_listcontainer_handler;

//! A constant \ref peepholed_toplevel_listcontainer_handler "peepholed list container popup handler".

//! \see peepholed_toplevel_listcontainer_handler

typedef const_ref<peepholed_toplevel_listcontainer_handlerObj> const_peepholed_toplevel_listcontainer_handler;

//! A nullable pointer reference to a \ref peepholed_toplevel_listcontainer_handler "peepholed list container popup handler".

//! \see peepholed_toplevel_listcontainer_handler

typedef ptr<peepholed_toplevel_listcontainer_handlerObj> peepholed_toplevel_listcontainer_handlerptr;

//! A nullable pointer reference to a const \ref peepholed_toplevel_listcontainer_handler "peepholed list container popup handler".

//! \see peepholed_toplevel_listcontainer_handler

typedef const_ptr<peepholed_toplevel_listcontainer_handlerObj> const_peepholed_toplevel_listcontainer_handlerptr;

LIBCXXW_NAMESPACE_END

#endif
