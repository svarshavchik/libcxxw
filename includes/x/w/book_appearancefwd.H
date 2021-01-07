/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_book_appearancefwd_h
#define x_w_book_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC book_appearanceObj;
class book_appearance_base;

/*! Appearance of a \ref new_booklayoutmanager "book".

Specifies various properties of a container that uses the
book layout manager. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::new_booklayoutmanager nblm;

INSERT_LIBX_NAMESPACE::w::const_book_appearance custom=nblm.appearance
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::book_appearance &custom)
             {
                    // Modify custom->...
             });

nblm.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed. Use modify() to make a copy of an existing constant
appearance object, and pass it to a closure to customize it.

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

//! Alias

typedef appearance_wrapper<const_book_appearance> const_book_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
