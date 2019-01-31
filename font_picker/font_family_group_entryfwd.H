/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_family_group_entryfwd_h
#define x_w_font_family_group_entryfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN font_family_group_entryObj;

/*! A font family entry, for the font family combo-box in the font selection dialog popup.

The list of fonts available in the combo-box is kept in a list of these objects.
The font families are listed by their font_dialog_group_id, and this object
stores the id, and all the available fontpatterns.

*/

typedef ref<font_family_group_entryObj> font_family_group_entry;

//! A constant \ref font_family_group_entry "WHATITIS".

//! \see font_family_group_entry

typedef const_ref<font_family_group_entryObj> const_font_family_group_entry;

//! A nullable pointer reference to a \ref font_family_group_entry "WHATITIS".

//! \see font_family_group_entry

typedef ptr<font_family_group_entryObj> font_family_group_entryptr;

//! A nullable pointer reference to a const \ref font_family_group_entry "WHATITIS".

//! \see font_family_group_entry

typedef const_ptr<font_family_group_entryObj> const_font_family_group_entryptr;

LIBCXXW_NAMESPACE_END

#endif
