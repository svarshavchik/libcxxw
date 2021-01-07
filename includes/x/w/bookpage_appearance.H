/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpage_appearance_h
#define x_w_bookpage_appearance_h

#include <x/w/bookpage_appearancefwd.H>
#include <x/w/bookpage_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref bookpage_appearance "bookpage appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::bookpage_appearance::base.

class bookpage_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_bookpage_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
