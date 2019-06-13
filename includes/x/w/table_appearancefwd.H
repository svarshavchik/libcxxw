/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_appearancefwd_h
#define x_w_table_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC table_appearanceObj;
class table_appearance_base;

/*! Table header appearance

Specifies the appearance of the header row of a
\ref tablelayoutmanager "table".

\ref new_tablelayoutmanager "new_tablelayoutmanager" inherits from
\ref new_listlayoutmanager "new_listlayoutmanager" whose \c appearance member
specifies the list's appearance.

\ref new_tablelayoutmanager "new_tablelayoutmanager"'s \c extra_table_appearance
member refers to this object that specifies additional properties of the
table's header row.

\code
INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager lm{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_table_appearance custom=INSERT_LIBX_NAMESPACE::w::table_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::table_appearance &custom)
             {
                    // Modify custom->...
             });

lm.extra_table_appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::table_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<table_appearanceObj,
	    table_appearance_base> table_appearance;

//! Constant \ref table_appearance "table header row appearance".

//! \see table_appearance

typedef const_ref<table_appearanceObj,
		  table_appearance_base> const_table_appearance;

//! A nullable pointer reference to a \ref table_appearance "table header row appearance".

//! \see table_appearance

typedef ptr<table_appearanceObj,
	    table_appearance_base> table_appearanceptr;

//! A nullable pointer reference to a const \ref table_appearance "table header row appearance".

//! \see table_appearance

typedef const_ptr<table_appearanceObj,
		  table_appearance_base> const_table_appearanceptr;

//! Alias

typedef appearance_wrapper<const_table_appearance> const_table_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
