/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextfragmentfwd_H
#define x_w_impl_richtext_richtextfragmentfwd_H

#include <x/ptrfwd.H>
#include <x/w/impl/richtext/richtextparagraphfwd.H>
#include <x/w/impl/richtext/richtextiteratorfwd.H>
#include <x/w/namespace.H>
#include <x/w/types.H>

LIBCXXW_NAMESPACE_START

dim_t adjust_descender_for_underline(dim_t ascender, dim_t descender)
	LIBCXX_HIDDEN;

class LIBCXX_HIDDEN richtextfragmentObj;
class LIBCXX_HIDDEN fragment_list;
class LIBCXX_HIDDEN const_fragment_list;
class LIBCXX_HIDDEN paragraph_list;

//! A fragment of unicode text, ending at an allowed or a mandatory line break.

typedef ref<richtextfragmentObj> richtextfragment;

//! Reference to a constant rich text fragment.
typedef const_ref<richtextfragmentObj> const_richtextfragment;

//! A nullable pointer reference to a text fragment.
typedef ptr<richtextfragmentObj> richtextfragmentptr;

LIBCXXW_NAMESPACE_END

#endif
