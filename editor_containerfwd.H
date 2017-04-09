/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_containerfwd_h
#define editor_containerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class editor_containerObj;

//! A container with a \ref editor "editor element"

//! An internal display element, a subclass of a \ref peephole "peephole"
//! display element. The peephole contains the actual editor element.
//!
//! This is just a peephole container, with the \c editor_container setting
//! the peephole's size as specified by the
//! \ref input_field_config "input_field_config".

typedef ref<editor_containerObj> editor_container;

//! A nullable pointer reference to an editor container.

//! \see editor_container

typedef ptr<editor_containerObj> editor_containerptr;

//! A reference to a constant editor container.

//! \see editor_container

typedef const_ref<editor_containerObj> const_editor_container;

//! A nullable pointer reference to a constant editor container.

//! \see editor_container

typedef const_ptr<editor_containerObj> const_editor_containerptr;

LIBCXXW_NAMESPACE_END

#endif
