/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_appearance_h
#define x_w_scrollbar_appearance_h

#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/scrollbar_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref scrollbar_appearance "scrollbar appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::scrollbar_appearance::base.

class scrollbar_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_scrollbar_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
