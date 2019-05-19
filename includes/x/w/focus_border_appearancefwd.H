/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focus_border_appearancefwd_h
#define x_w_focus_border_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC focus_border_appearanceObj;
class focus_border_appearance_base;

/*! Appearance of a focus border.

Specifies a pair of borders, one border when there's no input focus, and
a border when there is an input focus. The default properties are
set by the current display theme. Typically, when there's no input focus
the border is an invisible border with the same size as the regular input
focus border; but some focus borders have a visible no-focus border, too.

\code
INSERT_LIBX_NAMESPACE::w::const_focus_border_appearance custom=INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::focus_border_appearance &custom)
             {
                    // Modify custom->...
             });
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::theme() returns
a cached default theme appearance for an invisible focus off border, and
a visible focus on border.
\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::menu_theme()
returns a cached default theme appearance for menu bar buttons.

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::thin_theme()
returns a cached fefault theme appearance for a smaller version of the
default theme(), thta's used for small checkboxes and radio buttons.

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::visible_thin_theme()
returns a cached default theme appearance for a smaller version of the
default theme(), but with a visible input focus off boder, so that the
border is visible all the time, and not only when the input focus gets
shown.

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::slider_theme()
returns a cached default theme appearance for a focus border for
\ref panelayoutmanager "pane sliders".

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::combobox_button_theme()
returns a cached default theme appearance for
\ref standard_comboboxlayoutmanager "combo-box buttons".

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::input_field_theme()
returns a cached default theme appearance for
\ref input_field "input fields".

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::list_theme()
returns a cached default theme appearance for
\ref listlayoutmanager "lists".

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::visible_list_theme()
returns a cached default theme appearance for
\ref listlayoutmanager "lists" that's always visible.

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::attached_button_theme()
returns a cached default theme appearance for focus borders of
miscellaneous buttons that open attached popups.

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::scrollbar_theme()
returns a cached default theme appearance for \ref scrollbar "scroll-bars".

\c INSERT_LIBX_NAMESPACE::w::focus_border_appearance::base::none_theme()
returns a cached default theme appearance for 0-width focus off and on
borders, this can be used to effectively remove visible focus borders.

*/

typedef ref<focus_border_appearanceObj,
	    focus_border_appearance_base> focus_border_appearance;

//! Constant \ref focus_border_appearance "WHATITIS".

//! \see focus_border_appearance

typedef const_ref<focus_border_appearanceObj,
		  focus_border_appearance_base> const_focus_border_appearance;

//! A nullable pointer reference to a \ref focus_border_appearance "WHATITIS".

//! \see focus_border_appearance

typedef ptr<focus_border_appearanceObj,
	    focus_border_appearance_base> focus_border_appearanceptr;

//! A nullable pointer reference to a const \ref focus_border_appearance "WHATITIS".

//! \see focus_border_appearance

typedef const_ptr<focus_border_appearanceObj,
		  focus_border_appearance_base> const_focus_border_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
