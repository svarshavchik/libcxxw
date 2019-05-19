/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_bookpage_appearancefwd_h
#define x_w_bookpage_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC bookpage_appearanceObj;
class bookpage_appearance_base;

/*! Appearance of \ref booklayoutmanager "book's page tabs".

Specifies various properties of a book's page's tab. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::bookpagefactory f;

INSERT_LIBX_NAMESPACE::w::const_bookpage_appearance custom=INSERT_LIBX_NAMESPACE::w::bookpage_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::bookpage_appearance &custom)
             {
                    // Modify custom->...
             });

f->appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::bookpage_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<bookpage_appearanceObj,
	    bookpage_appearance_base> bookpage_appearance;

//! Constant \ref bookpage_appearance "book page tab appearance".

//! \see bookpage_appearance

typedef const_ref<bookpage_appearanceObj,
		  bookpage_appearance_base> const_bookpage_appearance;

//! A nullable pointer reference to a \ref bookpage_appearance "book page tab appearance".

//! \see bookpage_appearance

typedef ptr<bookpage_appearanceObj,
	    bookpage_appearance_base> bookpage_appearanceptr;

//! A nullable pointer reference to a const \ref bookpage_appearance "book page tab appearance".

//! \see bookpage_appearance

typedef const_ptr<bookpage_appearanceObj,
		  bookpage_appearance_base> const_bookpage_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
