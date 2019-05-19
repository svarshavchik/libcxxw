/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_popup_list_appearancefwd_h
#define x_w_popup_list_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC popup_list_appearanceObj;
class popup_list_appearance_base;

/*! Popup list appearance

Specifies additional properties for a list in a popup.

Basicaly a subclass of \ref list_appearance "list appearance" with some
additional properties.

Like \ref list_appearance "INSERT_LIBX_NAMESPACE::w::list_appearance",
use modify() to make a copy of an existing
\c INSERT_LIBX_NAMESPACE::const_popup_list_appearance constant object,
using a closure to customize a new
\c INSERT_LIBX_NAMESPACE::popup_list_appearance object.

*/

typedef ref<popup_list_appearanceObj,
	    popup_list_appearance_base> popup_list_appearance;

//! Constant \ref popup_list_appearance "popup list appearance properties".

//! \see popup_list_appearance

typedef const_ref<popup_list_appearanceObj,
		  popup_list_appearance_base> const_popup_list_appearance;

//! A nullable pointer reference to a \ref popup_list_appearance "popup list appearance properties".

//! \see popup_list_appearance

typedef ptr<popup_list_appearanceObj,
	    popup_list_appearance_base> popup_list_appearanceptr;

//! A nullable pointer reference to a const \ref popup_list_appearance "popup list appearance properties".

//! \see popup_list_appearance

typedef const_ptr<popup_list_appearanceObj,
		  popup_list_appearance_base> const_popup_list_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
