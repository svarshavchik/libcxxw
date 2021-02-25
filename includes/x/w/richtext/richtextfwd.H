/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_richtext_richtextfwd_H
#define x_w_richtext_richtextfwd_H

#include <x/ptrfwd.H>
#include <x/w/alignment.H>
#include <x/w/types.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/labelfwd.H>
#include <x/w/input_field_configfwd.H>

LIBCXXW_NAMESPACE_START

class richtextObj;

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
