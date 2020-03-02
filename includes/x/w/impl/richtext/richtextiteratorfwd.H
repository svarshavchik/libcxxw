/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextiteratorfwd_H
#define x_w_impl_richtext_richtextiteratorfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC richtextiteratorObj;

//! Iterator, or a pointer, into a \ref richtext "rich text object".

//! \ref richtext "rich text"'s begin(), end(), and at() methods create new
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

typedef ref<richtextiteratorObj> richtextiterator;

//! A nullable pointer reference to a \ref richtextiterator "rich text iterator".

typedef ptr<richtextiteratorObj> richtextiteratorptr;

//! A constant reference to a \ref richtextiterator "rich text iterator".

typedef const_ref<richtextiteratorObj> const_richtextiterator;

LIBCXXW_NAMESPACE_END

#endif
