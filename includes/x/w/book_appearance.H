/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_book_appearance_h
#define x_w_book_appearance_h

#include <x/w/book_appearancefwd.H>
#include <x/w/book_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref book_appearance "book appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::book_appearance::base.

class book_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_book_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
