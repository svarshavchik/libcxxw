/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_book_appearancefwd_h
#define x_w_book_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class book_appearanceObj;
class book_appearance_base;

/*! Appearance of a \ref new_booklayoutmanager "book".

Specifies various properties of a container that uses the
book layout manager. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::new_booklayoutmanager nblm;

INSERT_LIBX_NAMESPACE::w::book_appearance custom=nblm.appearance->clone();

nblm.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::book_appearance::base::theme() returns
a cached default theme appearance, the default appearance initialized
for the \ref new_booklayoutmanager "book layout manager".

*/

typedef ref<book_appearanceObj,
	    book_appearance_base> book_appearance;

//! Constant \ref book_appearance "book container's appearance".

//! \see book_appearance

typedef const_ref<book_appearanceObj,
		  book_appearance_base> const_book_appearance;

//! A nullable pointer reference to a \ref book_appearance "book container's appearance".

//! \see book_appearance

typedef ptr<book_appearanceObj,
	    book_appearance_base> book_appearanceptr;

//! A nullable pointer reference to a const \ref book_appearance "book container's appearance".

//! \see book_appearance

typedef const_ptr<book_appearanceObj,
		  book_appearance_base> const_book_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
