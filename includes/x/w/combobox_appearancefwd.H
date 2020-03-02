/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_combobox_appearancefwd_h
#define x_w_combobox_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC combobox_appearanceObj;
class combobox_appearance_base;

/*! Combobox appearance

Visual appearance of a combo-box.

Derived from
\ref popup_list_appearance "INSERT_LIBX_NAMESPACE::w::popup_list_appearance"
which sets the visual appearance of the combo-box popup.

\code
INSERT_LIBX_NAMESPACE::w::const_combobox_appearance custom=INSERT_LIBX_NAMESPACE::w::combobox_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::combobox_appearance &custom)
             {
                    // Modify custom->...
             });
\endcode

The default popup appearance properties for combo-boxes
are specified by the current display theme,
and the default appearance object is cached for speed, and that's the
reason why it's constant.

Like \ref list_appearance "INSERT_LIBX_NAMESPACE::w::list_appearance",
and
\ref popup_list_appearance "INSERT_LIBX_NAMESPACE::w::popup_list_appearance",
use modify() creates a copy of the appearance object, which can be modified
further by the callable object, before returning a new constant appearance
object that can be safely accessed by the connection thread.

*/

typedef ref<combobox_appearanceObj,
	    combobox_appearance_base> combobox_appearance;

//! Constant \ref combobox_appearance "combo-box appearance settings".

//! \see combobox_appearance

typedef const_ref<combobox_appearanceObj,
		  combobox_appearance_base> const_combobox_appearance;

//! A nullable pointer reference to a \ref combobox_appearance "combo-box appearance settings".

//! \see combobox_appearance

typedef ptr<combobox_appearanceObj,
	    combobox_appearance_base> combobox_appearanceptr;

//! A nullable pointer reference to a const \ref combobox_appearance "combo-box appearance settings".

//! \see combobox_appearance

typedef const_ptr<combobox_appearanceObj,
		  combobox_appearance_base> const_combobox_appearanceptr;

//! Alias

typedef appearance_wrapper<const_combobox_appearance> const_combobox_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
