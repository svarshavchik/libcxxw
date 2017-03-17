/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextcursorlocationfwd_H
#define richtextcursorlocationfwd_H

#include <x/ref.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class richtextcursorlocationObj;

//! A location in a \ref richtext "rich text object".

//! These objects get attached to a specific character in a rich text object.
//! When the contents of the rich text object are modified, the
//! \c richtextcursorlocation object stays attached to the same character in
//! its new position. If richtextcursorlocation's object gets removed, it'll
//! point to the next or the previous character.
//!
//! Like the \ref richtext "rich text object", this is not a thread-safe
//! object. A public object provides a thread-safe wrapper.
typedef ref<richtextcursorlocationObj> richtextcursorlocation;

LIBCXXW_NAMESPACE_END

#endif
