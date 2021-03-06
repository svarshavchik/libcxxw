/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_richtext_richtextiteratorfwd_H
#define x_w_richtext_richtextiteratorfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC richtextiteratorObj;

//! Iterator, or a pointer, into a \ref richtext "rich text object".

//! Rich text iterators expose an iterator-like access to the underlying
//! \ref richtext "rich text" object, whose begin(), end(), and at() methods
//! create new
//! iterators. Each iterator owns a reference on its richtext object, and
//! a reference to the \ref richtextcursorlocation "richtextcursorlocation",
//! an internal placeholder for
//! the iterator, that the rich text classes update themselves.
//!
//! The \ref richtextcursorlocation "richtextcursorlocation" can only be
//! accessed while holding a lock on the underlying rich text object.
//! The iterator's method invoke \ref richtext "rich text"'s methods to do
//! that.
//!
//! richtextiterators have a special relationship to their richtexts. Their
//! methods acquire locks on the richtext, and are able to access info that
//! other classes can't.
//!
//! \par iterator positions.
//!
//! \code
//! auto q=p->begin();
//! auto q=p->end();
//! auto q=p->clone();
//! size_t offset=p->pos();
//! auto q=p->pos(offset);
//! \endcode
//!
//! An existing richtextiterator's begin() and end() method create another
//! iterator to the first or the last character in the underlying rich text.
//! clone() clones the iterator, returning another one at the same location.
//! swap() swaps the locations of two existing iterators, and pos()
//! returns the iterator's position index in the underlying text, or
//! return a new iterator for the specified location offset.
//!
//! \note
//! The contents of the underlying object can be changed at any by the
//! connection thread. A consistent view of a richtextiterator to an
//! \ref input_field "input field" requires obtaining an
//! \ref input_lock "input field lock".

typedef ref<richtextiteratorObj> richtextiterator;

//! A nullable pointer reference to a \ref richtextiterator "rich text iterator".

typedef ptr<richtextiteratorObj> richtextiteratorptr;

//! A constant reference to a \ref richtextiterator "rich text iterator".

typedef const_ref<richtextiteratorObj> const_richtextiterator;

//! How to intepret a specified cursor location, a location in richtext

//! The cursor location gets set according left-to-right render order, and the
//! cursor location's in the line fragment gets calculated. If the
//! paragraph embedding left is right to left, and this option is bidi,
//! the line position gets mirrored (location 0 becomes the location at
//! the end of the fragment, and increasing values get counted to the
//! beginning of the line).
//!
//! This is normally specified as "bidi". "lr" designation does not adjust the
//! location. This is typically done when the location is based on another
//! location in the text, which was already adjusted for right to left
//! embedding level, if appropriate.

enum class new_location { lr, bidi};

//! Get the location index offset

//! Analogous to new_location, when we read the existing cursor location, and
//! compute the offset to the beginning of the text, the cursor location
//! in its line fragment gets calculated based on this option.

enum class get_location { lr, bidi};

LIBCXXW_NAMESPACE_END

#endif
