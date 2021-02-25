/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_richtext_richtextcursorlocationfwd_H
#define x_w_richtext_richtextcursorlocationfwd_H

#include <x/ref.H>
#include <x/w/namespace.H>
#include <list>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtextcursorlocationObj;

//! A location in a \ref richtext "rich text object".

//! These objects get attached to a specific character in a rich text object.
//! When the contents of the rich text object are modified, the
//! \c richtextcursorlocation object stays attached to the same character in
//! its new position. If richtextcursorlocation's object gets removed, it'll
//! point to the next or the previous character.
//!
//! Like the \ref richtext "rich text object", this is not a thread-safe
//! object. The \ref richtextiterator "rich text iterator object" provides
//! a thread-safe wrapper for this object. This object is considered an
//! internal object owned by the rich text classes, and gets modified by
//! them.
//!
//! We thread a very fine needle here. In all cases, we can only get here
//! under protection of richtextobj's big lock. So, we're single-threaded
//! here in all cases. But, additionally, ONLY IN_THREAD we may also look
//! at my richtextfragmentObj's horiz_info in order to cache the horizontal
//! pixel offset that this location represents.

typedef ref<richtextcursorlocationObj> richtextcursorlocation;

//! Container for locations in a given richtextfragment

typedef std::list<richtextcursorlocation> fragment_cursorlocations_t;

LIBCXXW_NAMESPACE_END

#endif
