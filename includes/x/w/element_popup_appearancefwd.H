/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_popup_appearancefwd_h
#define x_w_element_popup_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class element_popup_appearanceObj;
class element_popup_appearance_base;

/*! Appearance of a display element with a button that opens an attached popup.

Specifies various properties of a display element that has a button that
opens an attached popup. The default properties are set by the current
display theme.

\code

INSERT_LIBX_NAMESPACE::w::const_element_popup_appearance custom=INSERT_LIBX_NAMESPACE::w::element_popup_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::element_popup_appearance &custom)
             {
                    // Modify custom->...
             });
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::element_popup_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<element_popup_appearanceObj,
	    element_popup_appearance_base> element_popup_appearance;

//! Constant \ref element_popup_appearance "attached popup appearance".

//! \see element_popup_appearance

typedef const_ref<element_popup_appearanceObj,
		  element_popup_appearance_base> const_element_popup_appearance;

//! A nullable pointer reference to a \ref element_popup_appearance "attached popup appearance".

//! \see element_popup_appearance

typedef ptr<element_popup_appearanceObj,
	    element_popup_appearance_base> element_popup_appearanceptr;

//! A nullable pointer reference to a const \ref element_popup_appearance "attached popup appearance".

//! \see element_popup_appearance

typedef const_ptr<element_popup_appearanceObj,
		  element_popup_appearance_base> const_element_popup_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
