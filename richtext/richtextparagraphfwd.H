/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextparagraphfwd_H
#define richtextparagraphfwd_H

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN richtextparagraphObj;

//! A list of text fragments in a \ref richtext "rich text object".

//! A mandatory line break divides sections of rich text "fragments" into
//! paragraphs.

typedef ref<richtextparagraphObj> richtextparagraph;

LIBCXXW_NAMESPACE_END

#endif
