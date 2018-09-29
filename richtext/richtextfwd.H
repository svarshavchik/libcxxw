/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextfwd_H
#define richtextfwd_H

#include <x/ptrfwd.H>
#include "x/w/input_fieldfwd.H"
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtextObj;

//! A list of text paragraphs.

//! This object represents one or more paragraphs of "rich text", text with
//! colors and fonts.
//!
//! The implementation object is protected by a mutex, because it is not
//! thread safe. finish_initialization() must be invoked every time the mutex
//! protecting this object gets locked, except in order to obtain the
//! current contents of the rich text. This is required in order to finish
//! construction of this object.

typedef ref<richtextObj> richtext;

//! A nullable pointer reference to a \ref richtext "rich text string".

typedef ptr<richtextObj> richtextptr;

LIBCXXW_NAMESPACE_END

#endif
